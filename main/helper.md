# AI Helper Guide — MG_GUI Project (ESP32 + LVGL 9.3)

## 1. Project Overview

| Item | Value |
|------|-------|
| **MCU** | ESP32 |
| **Framework** | ESP-IDF (v5.5.1) |
| **UI Framework** | LVGL 9.3 (SquareLine Studio 1.6.0) |
| **Display** | GC9A01 240×240, SPI, RGB565, BGR endian |
| **Keys** | Direct GPIO input (no PCF8574) |
| **Bluetooth** | Classic SPP (Legacy Pairing, PIN-based) |
| **Storage** | LittleFS (saved scans, flash partition "storage") |
| **Battery** | ADC-based monitoring |

## 2. GPIO Pin Mappings

### Display (SPI / GC9A01)
| Signal | GPIO | Notes |
|--------|------|-------|
| SCLK | 18 | SPI2_HOST clock |
| MOSI | 19 | SPI2_HOST data |
| DC (Data/Command) | 21 | |
| CS | 3 | Chip select |
| RST | -1 | Not used (hardware reset via power) |

### Keys (Direct GPIO, Active-Low with Pull-Up)
| Key | GPIO | Bit Mask | Event |
|-----|------|----------|-------|
| UP | 13 | (1<<0) | KEY_UP |
| DOWN | 12 | (1<<1) | KEY_DOWN |
| OK | 14 | (1<<2) | KEY_OK / KEY_OK_HOLD |
| BACK | 27 | (1<<3) | KEY_BACK / KEY_BACK_HOLD |
| TRIG | 26 | (1<<4) | KEY_TRIG |

**Key timing constants:**
- `KEY_DEBOUNCE_MS` = 40ms
- `KEY_HOLD_MS` = 800ms
- `KEY_SCAN_MS` = 10ms

**Key state logic:**
- Pin level LOW (0) = pressed
- Pin level HIGH (1) = released
- Pull-up enabled internally

## 3. Storage System (LittleFS)

### Partition
| Item | Value |
|------|-------|
| **Partition label** | `storage` (defined in `main/partitions.csv`) |
| **Mount point** | `/littlefs` |
| **Format on mount** | Yes (if mount fails) |

### File Structure
| File | Purpose |
|------|---------|
| `/littlefs/scan_index.dat` | Index file: header + array of all scan records |
| `/littlefs/scan_XXXX.bin` | Individual scan data files (XXXX = 4-digit ID, e.g., scan_0001.bin) |
| `/littlefs/scan_index.tmp` | Temp file for atomic index writes |

### Index File Format
```
+-----------------------------+
| scan_index_header_t         |  16 bytes
+-----------------------------+
| scan_index_item_t [count]   |  count × 20 bytes
+-----------------------------+

scan_index_header_t:
  magic:      0x58444E49 ("INDX")
  version:    1
  reserved:   0
  count:      number of saved scans
  next_id:    next scan ID to assign

scan_index_item_t (20 bytes each):
  id:         uint32_t — unique scan ID
  mode:       uint32_t — scan mode at save time
  point_count: uint32_t — number of samples
  timestamp_sec: uint32_t — Unix timestamp
```

### Scan File Format (scan_XXXX.bin)
```
+-----------------------------+
| scan_record_header_t        |  20 bytes
+-----------------------------+
| int16_t samples [point_count]|  point_count × 2 bytes
+-----------------------------+

scan_record_header_t:
  magic:      0x4E414353 ("SCAN")
  version:    1
  reserved:   0
  id:         uint32_t
  mode:       uint32_t
  point_count: uint32_t
  timestamp_sec: uint32_t
```

### API Reference
| Function | Purpose |
|----------|---------|
| `storage_littlefs_init()` | Mount LittleFS, validate/create index file |
| `storage_littlefs_deinit()` | Unmount LittleFS |
| `storage_littlefs_is_ready()` | Check if mounted |
| `storage_littlefs_save_scan(record, &id)` | Save scan → creates scan file + updates index |
| `storage_littlefs_load_index(items, max, &count)` | Load all scan index entries |
| `storage_littlefs_load_scan_header(id, &header)` | Load scan file header |
| `storage_littlefs_load_scan_samples(id, samples, max, &count)` | Load scan samples |
| `storage_littlefs_delete_scan(id)` | Delete scan file + update index |

### Save Flow (MEM modes)
```
brain_stop_scan_and_save() [brain.c:487]
    │
    ▼
storage_littlefs_save_scan() [storage_littlefs.c:86]
    │
    ├── 1. Read index file → header + items[]
    ├── 2. Write scan_XXXX.bin (header + samples)
    ├── 3. Append new item to index array
    ├── 4. Increment next_id
    ├── 5. Write temp index file → rename to scan_index.dat
    └── 6. Free temp memory
```

### Data Persistence Guarantees
- **Atomic index writes**: Write to `.tmp` → rename (atomic on most FS)
- **Atomic scan writes**: Write to `XXXX.bin.tmp` → rename
- **Auto format**: If LittleFS mount fails, formats partition on next boot
- **Index validation**: Magic number + version check on every read

---

---

## 4. Architecture — Brain-Centric (3 Layers)

```
┌──────────────────────────────────────────────────────────────┐
│                      main.c (Bootstrap)                      │
│  SPI/LCD init → LVGL init → brain_init() → bluetooth_init  │
│  → key_task (priority 2) → lvgl_task (priority 3)           │
└──────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌──────────────────────────────────────────────────────────────┐
│                    brain.c — THE BRAIN                        │
│  • Owner of ALL state: current_page, loaded_page             │
│  • Scan state: scan_sub_state, trigger_phase, scan_mode      │
│  • Settings: g_settings (NVS persistence)                    │
│  • Event system: APP_EVENT_SCAN_CHANGED, APP_EVENT_BATTERY_CHANGED │
│  • Navigation: brain_handle_key() sets state only            │
│  • UI bridge: brain_process_ui_cmds() calls LVGL APIs        │
│  • Bluetooth policy: auto enable/disable based on scan mode  │
└──────────────────────────────────────────────────────────────┘
                              │
              ┌───────────────┴───────────────┐
              ▼                               ▼
┌─────────────────────────┐      ┌─────────────────────────────┐
│  scan_process.c         │      │  ui/ (LVGL screens)         │
│  • EXECUTOR only        │      │  • RENDERER only            │
│  • No decision logic    │      │  • No decision logic        │
│  • calibration, pulse   │      │  • Read getters from brain  │
│    capture, buffer      │      │  • Only display state       │
└─────────────────────────┘      └─────────────────────────────┘
```

### GOLDEN RULE
> **Only `brain.c` decides navigation.** No other file should call `lv_screen_load()` for main navigation.

---

## 5. Page States & Navigation

### app_page_t enum
```
PAGE_NONE → PAGE_SPLASH → PAGE_MAIN_MENU → PAGE_SCAN → PAGE_SCAN_PAGE → (BACK) → PAGE_SCAN → (BACK) → PAGE_MAIN_MENU
                                                                        ↓ (other)
                                                                PAGE_SEND / PAGE_MEMORY / PAGE_SETTING / PAGE_ABOUT
```

### Page Transition Mechanism
1. **`brain_handle_key(evt)`** — only updates state variables (current_page, selected_menu, scan_selected)
2. **`brain_process_ui_cmds()`** — called inside lvgl_task under mutex lock
   - If current_page != loaded_page → `brain_transition_to_page(current_page)`
   - On failure → current_page reverts to loaded_page
3. **`brain_transition_to_page(target)`** — calls prepare → lv_screen_load → destroy old
4. **`brain_apply_focus_if_needed()`** — syncs menu/scan focus after page load

### Navigation Rules Per Page
| Current Page | Key | Action (sets state only) |
|--------------|-----|--------------------------|
| PAGE_SPLASH | KEY_OK & splash_done | current_page = PAGE_MAIN_MENU |
| PAGE_MAIN_MENU | KEY_UP/DOWN | selected_menu ±1 (mod 5) |
| PAGE_MAIN_MENU | KEY_OK (item 0=Scan) | current_page = PAGE_SCAN |
| PAGE_MAIN_MENU | KEY_OK (item 1=Send) | current_page = PAGE_SEND |
| PAGE_MAIN_MENU | KEY_OK (item 2=Memory) | current_page = PAGE_MEMORY |
| PAGE_MAIN_MENU | KEY_OK (item 3=Setting) | current_page = PAGE_SETTING |
| PAGE_MAIN_MENU | KEY_OK (item 4=About) | current_page = PAGE_ABOUT |
| PAGE_SCAN | KEY_UP/DOWN | scan_selected ±1 (mod 4) |
| PAGE_SCAN | KEY_BACK | current_page = PAGE_MAIN_MENU |
| PAGE_SCAN | KEY_OK | Init scan, set PAGE_SCAN_PAGE |
| PAGE_SCAN_PAGE | KEY_BACK (first press) | Stop scan → SCAN_STATE_STOPPED_WAIT_BACK |
| PAGE_SCAN_PAGE | KEY_BACK (second press) | current_page = PAGE_SCAN |
| PAGE_SCAN_PAGE | KEY_TRIG | Calibration + capture (see §4) |
| PAGE_SETTING | KEY_UP/DOWN | Navigate setting items |
| PAGE_SETTING | KEY_OK | Open detail view |
| PAGE_SETTING | KEY_BACK | Close detail / go to MainMenu |

---

## 6. Scan System Deep Dive

### Scan States
```
scan_sub_state_t:
  SCAN_STATE_IDLE = 0
  SCAN_STATE_RUNNING = 1
  SCAN_STATE_STOPPED_WAIT_BACK = 2

trigger_phase (owned by brain):
  phase 0 = ready (initial, waiting for first trigger)
  phase 1 = calibrated/arm (calibration done, waiting for pulse trigger)
  phase 2 = auto task running (multi-pulse capture in progress)
```

### Scan Modes
```
scan_mode_t:
  SCAN_MODE_MANPC = 0  (manual, PC transfer)
  SCAN_MODE_AUTOPC = 1 (auto, PC transfer)
  SCAN_MODE_AUTOMEM = 2 (auto, memory storage)
  SCAN_MODE_MANMEM = 3 (manual, memory storage)
```

### Trigger Behavior in PAGE_SCAN_PAGE on KEY_TRIG

| Mode | Phase | Action |
|------|-------|--------|
| **MANPC / MANMEM** | 0 + auto_cal=true | `scan_process_calibrate_now()` → phase=1 |
| **MANPC / MANMEM** | 0 + auto_cal=false | phase=1 directly |
| **MANPC / MANMEM** | 1+ | `scan_process_capture_one_pulse()` → pulse_count++ |
| **AUTOPC / AUTOMEM** | 0 + auto_cal=true | `scan_process_calibrate_now()` → phase=1 |
| **AUTOPC / AUTOMEM** | 0 + auto_cal=false | phase=1 directly |
| **AUTOPC / AUTOMEM** | 1 | `scan_process_capture_multi_pulse(delay_ms)` → phase=2, starts task |
| **AUTOPC / AUTOMEM** | 2 + stop_trg=true | `brain_stop_scan_and_save()` → stop task |
| **AUTOPC / AUTOMEM** | 2 + stop_trg=false | Ignore trigger |

### Auto-Stop Condition
In `brain_process_ui_cmds()`: when phase==2 && scan_mode is AUTO && pulse_count >= g_settings.puls_max → auto stop.

### Bluetooth Policy
- Bluetooth enabled ONLY when current_page == PAGE_SCAN_PAGE
- If g_settings.bl_auto_off == true AND scan_mode is MEM (AUTOMEM/MANMEM) → Bluetooth DISABLED
- Otherwise in Scan Page → Bluetooth ENABLED
- Function: `brain_apply_bluetooth_policy()` called every loop

---

## 7. Module Function Reference

### 7.1 app_settings.c/h — App Settings (NVS)

| Function | Returns | Purpose |
|----------|---------|---------|
| `app_settings_defaults(settings)` | void | Set all fields to default values (auto_cal=true, pulse_count=16, max=200, delay=1000ms, bt_name="ESP32_Device", bt_pass="33138", auto_connect=true) |
| `app_settings_load()` | esp_err_t | Load settings from NVS namespace "app_cfg", key "settings"; apply defaults on miss/version mismatch |
| `app_settings_save()` | esp_err_t | Save g_app_settings blob to NVS with version tracking |
| `app_settings_reset()` | esp_err_t | Reset to defaults and save to NVS |

**Global:** `extern app_settings_t g_app_settings`

---

### 7.2 battery_process.c/h — Battery Monitoring

| Function | Returns | Purpose |
|----------|---------|---------|
| `battery_process_init()` | void | Reset all battery state to zero/empty |
| `battery_process_update()` | void | Read ADC → voltage (mV) → percent → level. Log all values |

**Getters:**

| Function | Returns | Purpose |
|----------|---------|---------|
| `battery_process_get_adc_value()` | uint16_t | Raw ADC value |
| `battery_process_get_voltage_mv()` | uint16_t | Battery voltage in mV |
| `battery_process_get_percent()` | uint8_t | Percentage 0-100 |
| `battery_process_get_level()` | battery_level_t | EMPTY/25/50/75/FULL |

**Constants:** MIN_MV=3300, MAX_MV=4200. Levels: EMPTY(0-10%), 25(11-35%), 50(36-60%), 75(61-85%), FULL(86-100%)

---

### 7.3 storage_littlefs.c/h — Flash Storage

| Function | Returns | Purpose |
|----------|---------|---------|
| `storage_littlefs_init()` | esp_err_t | Mount LittleFS on "storage" partition, validate/create index |
| `storage_littlefs_deinit()` | esp_err_t | Unmount LittleFS |
| `storage_littlefs_is_ready()` | bool | Returns mount status |
| `storage_littlefs_save_scan(record, &id)` | esp_err_t | Save scan → creates scan_XXXX.bin + updates index |
| `storage_littlefs_load_index(items, max, &count)` | esp_err_t | Load all scan index entries |
| `storage_littlefs_load_scan_header(id, &header)` | esp_err_t | Load header of specific scan |
| `storage_littlefs_load_scan_samples(id, samples, max, &count)` | esp_err_t | Load raw ADC samples |
| `storage_littlefs_delete_scan(id)` | esp_err_t | Remove scan file + update index |

**Note:** Detailed storage info is in Section 3 (Storage System).

---

### 7.4 pcf8574.c/h — Key Reader (Direct GPIO)

| Function | Returns | Purpose |
|----------|---------|---------|
| `pcf8574_init()` | esp_err_t | Configure GPIO 12/13/14/26/27 as input with pull-up |
| `pcf8574_read(&value)` | esp_err_t | Read all key GPIOs, return raw byte |
| `key_init()` | void | Initialize key state machine |
| `key_scan()` | void | Scan keys: debounce, press/release, hold detection |
| `key_get()` | key_evt_t | Get next pending event, clear queue |

**Events:** KEY_NONE, KEY_UP, KEY_DOWN, KEY_OK, KEY_BACK, KEY_TRIG, KEY_OK_HOLD, KEY_BACK_HOLD

---

### 7.5 bluetooth.c/h — Bluetooth Classic SPP

| Function | Returns | Purpose |
|----------|---------|---------|
| `bluetooth_init(name)` | esp_err_t | Store device name |
| `bluetooth_enable()` | esp_err_t | Full BT init: controller → bluedroid → SPP → GAP |
| `bluetooth_disable()` | esp_err_t | Disconnect → deinit SPP/bluedroid/controller |
| `bluetooth_is_enabled()` | bool | Returns enabled state |
| `bluetooth_is_connected()` | bool | Returns connection state |
| `bluetooth_send_int32(value)` | esp_err_t | Queue single int32, paced by WRITE_EVT |
| `bluetooth_send_array(data, count)` | esp_err_t | Direct comma-separated send |
| `bluetooth_send_int32_stream_begin(data, count)` | esp_err_t | Paced stream, one per WRITE_EVT |
| `bluetooth_stream_is_active()` | bool | Check stream in progress |
| `bluetooth_stream_abort()` | void | Stop active stream |
| `bluetooth_connect_last_peer()` | esp_err_t | Connect to last saved BDA |
| `bluetooth_has_last_peer()` | bool | Check if last peer BDA stored |

---

### 7.6 scan_process.c/h — Scan Executor

| Function | Returns | Purpose |
|----------|---------|---------|
| `scan_process_init()` | void | Reset internal state |
| `scan_process_start(mode)` | void | Begin scan in specified mode |
| `scan_process_stop()` | void | Stop capture, reset to IDLE |
| `scan_process_calibrate_now()` | bool | Read ADC baseline, store offset |
| `scan_process_capture_one_pulse()` | void | Sample one pulse, calc arcs/angle |
| `scan_process_capture_multi_pulse(delay_ms)` | void | Start FreeRTOS task for periodic capture |
| `scan_process_get_current_adc_value()` | int32_t | Current raw ADC |
| `scan_process_get_signed_value()` | int32_t | Calibrated signed value |
| `scan_process_get_positive_arc_value()` | int32_t | Positive arc magnitude |
| `scan_process_get_negative_arc_value()` | int32_t | Negative arc magnitude |
| `scan_process_get_needle_angle()` | int32_t | ±900 (±90.0° ×10) |
| `scan_process_get_pulse_count()` | uint32_t | Captured pulse count |
| `scan_process_is_calibrated()` | bool | True if calibrated |
| `scan_process_get_buffer_data(&count)` | const int16_t* | Pointer to ADC buffer |
| `scan_process_add_point_to_buffer(val)` | bool | Add point, false if full |

---

### 7.7 brain.c/h — Central State Machine

**Init:**

| Function | Returns | Purpose |
|----------|---------|---------|
| `brain_init()` | void | NVS init, settings load, LittleFS init, state reset |
| `brain_settings_set_defaults()` | void | memset + default values |
| `brain_settings_save()` | esp_err_t | Save g_settings to NVS |
| `brain_settings_load()` | esp_err_t | Load from NVS or set defaults |

**Key:**

| Function | Returns | Purpose |
|----------|---------|---------|
| `brain_handle_key(evt)` | void | Main state machine for keys |
| `brain_update_battery()` | void | Read battery, emit if changed |
| `brain_set_bool_setting(idx, val)` | void | Set bool setting by index |
| `brain_toggle_bool_setting(idx)` | void | Toggle bool setting |
| `brain_setting_detail_step(increase)` | void | Step numeric setting |
| `brain_setting_next()/prev()` | int | Navigate settings |

**Getters:**

| Function | Returns |
|----------|---------|
| `brain_get_current_page()` | app_page_t |
| `brain_get_selected_menu()` | int (0-4) |
| `brain_get_scan_mode()` | scan_mode_t |
| `brain_get_scan_sub_state()` | scan_sub_state_t |
| `brain_get_battery_level()` | battery_level_t |
| `brain_get_battery_percent()` | uint8_t |
| `brain_get_settings()` | const system_settings_t* |
| `brain_get_setting_index()` | int (0-9) |
| `brain_should_bluetooth_be_enabled()` | bool |

**Events:** `brain_emit_event()` / `brain_consume_events()`

**UI Bridge:**

| Function | Returns | Purpose |
|----------|---------|---------|
| `brain_process_ui_cmds()` | void | Transition pages, focus, render |
| `brain_transition_to_page(target)` | esp_err_t | Prepare → lv_screen_load → destroy old |
| `brain_prepare_page(page, &screen)` | esp_err_t | Init screen if not ready |
| `brain_destroy_page(page)` | void | Destroy screen |
| `brain_apply_focus_if_needed()` | void | Sync menu/scan/setting focus |

**Scan:**

| Function | Returns | Purpose |
|----------|---------|---------|
| `brain_stop_scan_and_save()` | void | Stop scan, save to LittleFS if MEM |
| `brain_save_current_scan(&id)` | esp_err_t | Write scan to LittleFS |

---

### 7.8 ui.c/h — Global UI + Splash

| Function | Purpose |
|----------|---------|
| `ui_init()` | Create theme, init Splash, load it |
| `ui_destroy()` | Destroy all screens |
| `splash_finished()` | Set splash_done = true |
| `startGoldenLabAnimation(delay)` | Text fade-in + scale |
| `startArcAnimations(delay)` | 4 arc draw animations |
| `startRaysPhase(parent, delay)` | 18 random rays |
| `ui_Screen1_cleanup_and_destroy()` | Delete timer, animations, screen |

---

### 7.9 ui_Screen1.c/h — Splash

| Function | Purpose |
|----------|---------|
| `ui_Screen1_screen_init()` | Create root, label, arcs, dots, animations |
| `ui_Screen1_screen_destroy()` | Delete screen |
| `ui_Screen1_is_ready()` | Check root != NULL |

---

### 7.10 ui_MainMenu.c/h — Main Menu

| Function | Purpose |
|----------|---------|
| `ui_MainMenu_screen_init()` | Create root + 5 menu items |
| `ui_MainMenu_screen_destroy()` | Delete screen |
| `ui_MainMenu_is_ready()` | Check readiness |
| `menu_set_focused_index(idx)` | Highlight menu item |

---

### 7.11 ui_ScanMenu.c/h — Scan Mode Selection

| Function | Purpose |
|----------|---------|
| `ui_ScanMenu_screen_init()` | Create root + 4 mode items |
| `ui_ScanMenu_screen_destroy()` | Delete screen |
| `scan_set_focused_index(idx)` | Highlight mode item |

---

### 7.12 ui_ScanPage.c/h — Real-time Scan Display

| Function | Purpose |
|----------|---------|
| `ui_ScanPage_screen_init()` | Create meter, arcs, needle, labels, icons |
| `ui_ScanPage_screen_destroy()` | Delete screen |
| `ui_ScanPage_is_ready()` | Check readiness |
| `ui_scanpage_render()` | Read getters, update widgets |
| `ui_ScanPage_update_bluetooth_icon()` | Show/hide BT icon |
| `ui_ScanPage_update_calibration_icon()` | Show/hide calibration icon |

---

### 7.13 ui_Setting.c/h — Settings Screen

| Function | Purpose |
|----------|---------|
| `ui_Setting_screen_init()` | Create list + detail widgets |
| `ui_Setting_screen_destroy()` | Delete screen, free animations |
| `ui_Setting_is_ready()` | Check readiness |
| `ui_Setting_update_view(index)` | Highlight selected item |
| `ui_Setting_focus_open(idx)` | Start open animation |
| `ui_Setting_focus_open_done()` | Check open animation done |
| `ui_Setting_focus_close(idx)` | Start close animation |
| `ui_Setting_focus_close_done()` | Check close animation done |
| `ui_Setting_render_detail(index)` | Show/hide detail widgets |
| `ui_Setting_hide_all_details()` | Hide all detail widgets |
| `ui_Setting_force_refresh()` | Force full UI refresh |

---

### 7.14 ui_SendData.c/h — Send Data Screen

| Function | Purpose |
|----------|---------|
| `ui_SendData_screen_init()` | Create send data UI |
| `ui_SendData_screen_destroy()` | Delete screen |
| `ui_SendData_is_ready()` | Check readiness |

---

### 7.15 ui_Memory.c/h — Memory Viewer

| Function | Purpose |
|----------|---------|
| `ui_Memory_screen_init()` | Create memory viewer UI |
| `ui_Memory_screen_destroy()` | Delete screen |
| `ui_Memory_is_ready()` | Check readiness |

---

### 7.16 ui_About.c/h — About Screen

| Function | Purpose |
|----------|---------|
| `ui_About_screen_init()` | Create about UI |
| `ui_About_screen_destroy()` | Delete screen |
| `ui_About_is_ready()` | Check readiness |

---

## 8. Settings Structure (g_settings)

```c
typedef struct {
    bool auto_cal;           // index 0: auto calibration before capture
    int32_t auto_cal_pls;    // index 1: calibration pulse count (0-9999)
    int32_t puls_max;        // index 2: max pulses for auto stop (10-9999)
    int32_t delay_time;      // index 3: delay between pulses in auto mode (100-9999ms)
    bool stop_trg;           // index 4: TRIG key stops auto scan
    bool beep;               // index 5: beep on events
    bool bl_auto_off;        // index 6: auto-disable BT in memory modes
    bool bl_auto_connect;    // index 7: auto-connect to last peer
    int32_t bl_pass;         // index 8: Bluetooth PIN (0-999999)
    char bl_name[32];        // index 9: Bluetooth device name
} system_settings_t;
```

Persistence: NVS namespace "brain_cfg", key "settings" (blob).

---

## 12. Brain Functions Reference (Critical API)

### Initialization
| Function | Location | Purpose |
|----------|----------|---------|
| `brain_init()` | brain.c:1020 | NVS init, settings load, LittleFS init, state reset, battery init |
| `brain_settings_load()` | brain.c:291 | Load from NVS or set defaults |
| `brain_settings_save()` | brain.c:266 | Save g_settings to NVS |
| `brain_settings_set_defaults()` | brain.c:247 | memset + default values |

### Key Handling
| Function | Location | Purpose |
|----------|----------|---------|
| `brain_handle_key(evt)` | brain.c:1079 | Main state machine for key events |
| `brain_update_battery()` | brain.c:206 | Read battery, emit event if changed |
| `brain_set_bool_setting(idx, val)` | brain.c:626 | Set boolean setting by index |
| `brain_toggle_bool_setting(idx)` | brain.c:677 | Toggle boolean setting |
| `brain_setting_detail_step(increase)` | brain.c:518 | Step numeric setting up/down |
| `brain_setting_next()/prev()` | brain.c:344/349 | Navigate setting items |

### State Queries (Getters)
| Function | Returns |
|----------|---------|
| `brain_get_current_page()` | app_page_t |
| `brain_get_selected_menu()` | int (0-4) |
| `brain_get_scan_mode()` | scan_mode_t |
| `brain_get_scan_sub_state()` | scan_sub_state_t |
| `brain_get_battery_level()` | battery_level_t |
| `brain_get_battery_percent()` | uint8_t |
| `brain_get_settings()` | const system_settings_t* |
| `brain_get_setting_index()` | int (0-9) |
| `brain_should_bluetooth_be_enabled()` | bool |

### Event System
| Function | Purpose |
|----------|---------|
| `brain_emit_event(event)` | Set bit in pending_events (critical section) |
| `brain_consume_events()` | Return and clear all pending events |

### UI Bridge
| Function | Location | Purpose |
|----------|----------|---------|
| `brain_process_ui_cmds()` | brain.c:1316 | Transition pages, apply focus, render on events |
| `brain_transition_to_page(target)` | brain.c:916 | Prepare → lv_screen_load → destroy old |
| `brain_prepare_page(page, &screen)` | brain.c:835 | Call UI screen init if not ready |
| `brain_destroy_page(page)` | brain.c:769 | Call UI screen destroy function |
| `brain_apply_focus_if_needed()` | brain.c:958 | Sync menu/scan/setting focus |

### Scan Control
| Function | Location | Purpose |
|----------|----------|---------|
| `brain_stop_scan_and_save()` | brain.c:487 | Stop scan process, save to LittleFS if MEM mode |
| `brain_save_current_scan(&id)` | brain.c:463 | Write scan data to LittleFS |

---

## 13. Scan Process Functions (Executor Layer)

| Function | Purpose |
|----------|---------|
| `scan_process_init()` | Reset all internal state |
| `scan_process_start(mode)` | Begin scan operation |
| `scan_process_stop()` | Stop capture task, reset to IDLE |
| `scan_process_calibrate_now()` | Read ADC baseline, store offset, return true/false |
| `scan_process_capture_one_pulse()` | Sample one pulse, calc arcs/angle, increment pulse_count |
| `scan_process_capture_multi_pulse(delay_ms)` | Start FreeRTOS task for periodic capture |
| `scan_process_get_current_adc_value()` | Current raw ADC |
| `scan_process_get_signed_value()` | Calibrated signed value |
| `scan_process_get_positive_arc_value()` | Positive arc magnitude |
| `scan_process_get_negative_arc_value()` | Negative arc magnitude |
| `scan_process_get_needle_angle()` | ±900 (±90.0 degrees, ×10) |
| `scan_process_get_pulse_count()` | Number of captured pulses |
| `scan_process_is_calibrated()` | true if calibration done |
| `scan_process_get_buffer_data(&count)` | Pointer to ADC buffer, set count |
| `scan_process_add_point_to_buffer(val)` | Add point, return false if full |

---

## 14. UI Layer Functions

### ui.c (Global UI)
| Function | Purpose |
|----------|---------|
| `ui_init()` | Create theme, init Splash screen, load it |
| `ui_destroy()` | Destroy all screens |
| `splash_finished()` | Set splash_done = true |
| `startGoldenLabAnimation(delay)` | Text fade-in + scale animation |
| `startArcAnimations(delay)` | 4 arc draw animations (red, green, blue, yellow) |
| `startRaysPhase(parent, delay)` | 18 random ray lines from center |
| `ui_Screen1_cleanup_and_destroy()` | Delete timer, animations, screen object |

### ui_Screen1 (Splash)
| Function | Purpose |
|----------|---------|
| `ui_Screen1_screen_init()` | Create root, label, arcs, dots, start animations |
| `ui_Screen1_screen_destroy()` | Delete screen |
| `ui_Screen1_is_ready()` | Check if root != NULL |

### ui_MainMenu
| Function | Purpose |
|----------|---------|
| `ui_MainMenu_screen_init()` | Create root + 5 menu items |
| `ui_MainMenu_screen_destroy()` | Delete screen |
| `ui_MainMenu_is_ready()` | Check readiness |
| `menu_set_focused_index(idx)` | Highlight menu item |

### ui_ScanMenu (Scan Mode Selection)
| Function | Purpose |
|----------|---------|
| `ui_ScanMenu_screen_init()` | Create root + 4 mode items |
| `ui_ScanMenu_screen_destroy()` | Delete screen |
| `scan_set_focused_index(idx)` | Highlight mode item |

### ui_ScanPage (Real-time Display)
| Function | Purpose |
|----------|---------|
| `ui_ScanPage_screen_init()` | Create meter, arcs, needle, labels, icons |
| `ui_ScanPage_screen_destroy()` | Delete screen |
| `ui_ScanPage_is_ready()` | Check readiness |
| `ui_scanpage_render()` | Read getters, update all widgets |
| `ui_ScanPage_update_bluetooth_icon()` | Show/hide Bluetooth icon |
| `ui_ScanPage_update_calibration_icon()` | Show/hide calibration icon |

### ui_Setting (Settings Screen)
| Function | Purpose |
|----------|---------|
| `ui_Setting_screen_init()` | Create list of setting items + detail widgets |
| `ui_Setting_screen_destroy()` | Delete screen, free animations |
| `ui_Setting_is_ready()` | Check readiness |
| `ui_Setting_update_view(index)` | Highlight selected item |
| `ui_Setting_focus_open(idx)` | Start open animation for detail |
| `ui_Setting_focus_open_done()` | Check if open animation done |
| `ui_Setting_focus_close(idx)` | Start close animation |
| `ui_Setting_focus_close_done()` | Check if close animation done |
| `ui_Setting_render_detail(index)` | Show/hide detail widgets for item |
| `ui_Setting_hide_all_details()` | Hide all detail widgets |
| `ui_Setting_force_refresh()` | Force full UI refresh |

---

## 15. Bluetooth Functions

### Public API (bluetooth.h)
| Function | Purpose |
|----------|---------|
| `bluetooth_init(name)` | Store device name, return OK |
| `bluetooth_enable()` | Full BT init: controller, bluedroid, SPP server, GAP callback |
| `bluetooth_disable()` | Disconnect, deinit SPP/bluedroid/controller |
| `bluetooth_is_enabled()` | Returns s_enabled |
| `bluetooth_is_connected()` | Returns s_connected |
| `bluetooth_send_int32(value)` | Queue single int32, send paced by WRITE_EVT |
| `bluetooth_send_array(data, count)` | Direct comma-separated send |
| `bluetooth_send_int32_stream_begin(data, count)` | Paced stream send, one item per WRITE_EVT |
| `bluetooth_stream_is_active()` | Check if stream in progress |
| `bluetooth_stream_abort()` | Stop active stream |
| `bluetooth_connect_last_peer()` | Connect to last saved BDA |
| `bluetooth_has_last_peer()` | Check if last peer BDA stored |

### Key Callbacks (internal)
| Callback | Events Handled |
|----------|----------------|
| `bt_spp_cb()` | INIT, START, SRV_OPEN, CLOSE, WRITE, CONG |
| `bt_gap_cb()` | AUTH_CMPL, PIN_REQ, MODE_CHG |

### Data Protocol
- Single value: `"%ld\r\n"` (e.g., "123\r\n")
- Array: "val1,val2,val3\r\n"
- Stream: each item sent as `"%ld\r\n"` paced by WRITE_EVT success

---

## 16. Splash Screen Animation Sequence

```
ui_Screen1_screen_init()
    │
    ├── Create root, GoldenLab label, 4 arcs, 4 dots
    │
    ├── startGoldenLabAnimation(0)
    │   ├── fade-in: transparent → opaque (300ms)
    │   └── scale: 150→256 with overshoot (600ms)
    │       └── completed: splash_finished() → splash_done = true
    │
    └── startArcAnimations(0)
        ├── Draw 4 arcs 0→100 (500ms each, ease_out)
            └── last arc completed: arcs_draw_done_cb()
                └── arc_spin_scale_timer_cb() runs every 33ms
                    ├── Rotate arcs: +25° per tick
                    ├── Scale down: 1.0 → 0.03
                    ├── Color morph: original → gold
                    ├── After complete: delete dots, then delete arcs
                    └── startRaysPhase() → 18 random rays
                        ├── Each ray: fade-in (250ms) → length animation (450ms) → fade-out (400ms) → delete
```

---

## 17. Task Structure

| Task | Priority | Stack | Loop |
|------|----------|-------|------|
| key_task | 2 | 4096 bytes | key_scan() → key_get() → brain_handle_key() → delay |
| lvgl_task | 3 | 8192 bytes | lock → brain_process_ui_cmds() → ui_scanpage_update_*() → lv_timer_handler() → unlock → delay |

### Lock Pattern
```c
if (lvgl_lock(1000)) {
    // LVGL API calls here
    lvgl_unlock();
}
```

### lvgl_task Detailed Order
1. xSemaphoreTake(lvgl_mux, 20ms timeout)
2. brain_process_ui_cmds()
3. ui_ScanPage_update_bluetooth_icon()
4. ui_ScanPage_update_calibration_icon()
5. lv_timer_handler() — returns delay ms
6. xSemaphoreGive(lvgl_mux)
7. vTaskDelay(clamped delay, 5-30ms)

---

## 18. Critical Values & Constants

| Constant | Value | Source |
|----------|-------|--------|
| LCD_W / LCD_H | 240 × 240 | main.c |
| LCD_SPI_MHZ | 40 | main.c |
| LVGL_TICK_PERIOD_MS | 1 | main.c |
| LVGL_BUF_HEIGHT | 60 | main.c |
| KEY_SCAN_MS | defined in pcf8574.h | key scan interval |
| KEY_NAV_DEBOUNCE_MS | 500 | main.c |
| MAX_SCAN_POINTS | 512 | scan_process.h |
| ADC_MAX_RESOLUTION | 4095 | scan_process.h |
| SCAN_NEEDLE_MAX_ANGLE | 900 (±90.0°) | scan_process.h |
| SETTING_ITEM_COUNT | 10 | brain.h |
| BT_TX_QUEUE_SIZE | 64 | bluetooth.c |
| RAY_COUNT | 18 | ui.c |

---

## 19. File Locations Quick Reference

| File | Role |
|------|------|
| `main/main.c` | Bootstrap, hardware init, task creation |
| `main/main.h` | Shared types: app_state_t, ui_cmd_t |
| `main/brain.c/h` | Central state machine, navigation, settings, events |
| `main/scan_process.c/h` | Executive layer for scan operations |
| `main/battery_process.c/h` | Battery ADC reading |
| `main/bluetooth.c/h` | Bluetooth Classic SPP |
| `main/pcf8574.c/h` | GPIO key reader (adapted for direct GPIO, not I²C) |
| `main/storage_littlefs.c/h` | Flash storage for saved scans |
| `main/ui.c/h` | Global UI init, splash animations |
| `main/ui/screens/ui_Screen1.c/h` | Splash screen |
| `main/ui/screens/ui_MainMenu.c/h` | Main menu (5 items) |
| `main/ui/screens/ui_ScanMenu.c/h` | Scan mode selection (4 modes) |
| `main/ui/screens/ui_ScanPage.c/h` | Real-time scan display |
| `main/ui/screens/ui_Setting.c/h` | Settings screen |
| `main/ui/screens/ui_SendData.c/h` | Send data screen |
| `main/ui/screens/ui_Memory.c/h` | Memory viewer |
| `main/ui/screens/ui_About.c/h` | About screen |
| `main/ui/images/*.c` | Embedded images |
| `main/ui/fonts/*.c` | Embedded fonts |

---

## 20. Key Data Flow Diagrams

### Key Event Flow
```
GPIO input (direct)
    │
    ▼
key_scan() [main.c: key_task]
    │
    ▼
key_get() → key_evt_t
    │
    ▼
brain_handle_key(evt) [updates state ONLY]
    │
    ▼
lvgl_task calls brain_process_ui_cmds()
    │
    ├── brain_transition_to_page() if page changed
    ├── brain_apply_focus_if_needed() if menu/scan/setting
    └── ui_scanpage_render() if SCAN/BATTERY event
```

### Scan Start Flow
```
User: MainMenu → Select Scan → OK
    │
    ▼
brain_handle_key(): current_page=PAGE_SCAN, scan_selected=0
    │
    ▼
lvgl_task: brain_transition_to_page(PAGE_SCAN)
    │
    ▼
User: Scan Menu → OK
    │
    ▼
brain_handle_key(): current_scan_mode = mode, scan_process_init()
    scan_process_start(mode), current_page = PAGE_SCAN_PAGE
    │
    ▼
User: TRIG (if auto_cal) → scan_process_calibrate_now()
User: TRIG → scan_process_capture_one_pulse() or capture_multi_pulse()
    │
    ▼
ui_scanpage_render() reads getters, updates LVGL widgets
```

### Settings Flow
```
User: MainMenu → Select Setting → OK
    │
    ▼
brain_handle_key(): current_page=PAGE_SETTING, SETTING_STATE_LIST
    │
    ▼
User: UP/DOWN → brain_setting_next/prev() → g_setting_index changes
User: OK → ui_Setting_focus_open() → SETTING_STATE_OPENING
    │
    ▼
After animation: SETTING_STATE_DETAIL
    │
    ▼
User: UP/DOWN → brain_setting_detail_step() → g_settings changes → NVS save
User: BACK → ui_Setting_focus_close() → SETTING_STATE_CLOSING
    │
    ▼
After animation: SETTING_STATE_LIST, ui_Setting_update_view()
```

---

## 21. Known Modifications

### Bluetooth Latency Patch
File: `sdkconfig` / ESP-IDF internal
Modification in ESP-IDF `bta_dm_pm.c` line 629:
```c
// Force Disable Sniff for Latency improvement
APPL_TRACE_WARNING("bta_dm_pm: SNIFF requested but BLOCKED. Keeping Active.");
bta_dm_pm_active(peer_addr);
return;
```
This prevents Bluetooth from entering sniff mode, keeping connection active for lower latency.

---

## 22. Context Card for AI

When helping with this project, remember:

1. **Brain is king** — only brain.c decides navigation and scan logic
2. **Scan process is a dumb executor** — no decision-making, just commands
3. **UI is a dumb renderer** — only reads getters, never decides
4. **All page transitions** go through brain_transition_to_page()
5. **All LVGL calls** must be under lvgl_lock() inside lvgl_task
6. **Splash blocks input** — splash_done must be true before any key navigation
7. **Scan has 2-level BACK** — first BACK stops scan, second BACK returns to menu
8. **Settings persist to NVS** — g_settings saved as blob
9. **Memory mode disables BT** — when bl_auto_off=true
10. **Pulse count ≠ calibration** — calibration never affects pulse_count