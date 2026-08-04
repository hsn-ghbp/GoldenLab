Microsoft Windows [Version 10.0.26100.8875]
(c) Microsoft Corporation. All rights reserved.

C:\Users\kimiya\Documents\ESP-IDF\Projects\MG_GUI>set IDF_PATH=c:\Users\kimiya\esp\v5.5.1\esp-idf

C:\Users\kimiya\Documents\ESP-IDF\Projects\MG_GUI>"C:\Users\kimiya\.espressif\python_env\idf5.5_py3.11_env\Scripts\python.exe" "c:\Users\kimiya\esp\v5.5.1\esp-idf\tools\idf_monitor.py" -p COM9 -b 115200 --toolchain-prefix xtensa-esp32-elf- --make ""C:\Users\kimiya\.espressif\python_env\idf5.5_py3.11_env\Scripts\python.exe" "c:\Users\kimiya\esp\v5.5.1\esp-idf\tools\idf.py"" --target esp32 "c:\Users\kimiya\Documents\ESP-IDF\Projects\MG_GUI\build\magi.elf"
--- Warning: GDB cannot open serial ports accessed as COMx
--- Using \\.\COM9 instead...
--- esp-idf-monitor 1.7.0 on \\.\COM9 115200
--- Quit: Ctrl+] | Menu: Ctrl+T | Help: Ctrl+T followed by Ctrl+H
ets Jul 29 2019 12:21:46

rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:2
load:0x3fff0030,len:6380
ho 0 tail 12 room 4
load:0x40078000,len:15916
load:0x40080400,len:3860
--- 0x40080400: _invalid_pc_placeholder at C:/Users/kimiya/esp/v5.5.1/esp-idf/components/xtensa/xtensa_vectors.S:2235
entry 0x40080638
I (29) boot: ESP-IDF v5.5.1-dirty 2nd stage bootloader
I (29) boot: compile time Jul 31 2026 00:28:36
I (29) boot: Multicore bootloader
I (31) boot: chip revision: v3.1
I (34) boot.esp32: SPI Speed      : 40MHz
I (38) boot.esp32: SPI Mode       : DIO
I (41) boot.esp32: SPI Flash Size : 4MB
I (45) boot: Enabling RNG early entropy source...
I (49) boot: Partition Table:
I (52) boot: ## Label            Usage          Type ST Offset   Length
I (58) boot:  0 nvs              WiFi data        01 02 00009000 00006000
I (65) boot:  1 otadata          OTA data         01 00 0000f000 00002000
I (71) boot:  2 phy_init         RF data          01 01 00011000 00001000
I (78) boot:  3 factory          factory app      00 00 00020000 00180000
I (84) boot:  4 storage          Unknown data     01 83 001a0000 00060000
I (91) boot: End of partition table
I (94) boot: Defaulting to factory image
I (98) esp_image: segment 0: paddr=00020020 vaddr=3f400020 size=7082ch (460844) map
I (263) esp_image: segment 1: paddr=00090854 vaddr=3ffbdb60 size=04cb8h ( 19640) load
I (270) esp_image: segment 2: paddr=00095514 vaddr=40080000 size=0ab04h ( 43780) load
I (287) esp_image: segment 3: paddr=000a0020 vaddr=400d0020 size=ae478h (713848) map
I (532) esp_image: segment 4: paddr=0014e4a0 vaddr=4008ab04 size=0f9c8h ( 63944) load
I (557) esp_image: segment 5: paddr=0015de70 vaddr=50000000 size=00020h (    32) load
I (570) boot: Loaded app from partition at offset 0x20000
I (570) boot: Disabling RNG early entropy source...
I (581) cpu_start: Multicore app
I (589) cpu_start: Pro cpu start user code
I (589) cpu_start: cpu freq: 160000000 Hz
I (589) app_init: Application information:
I (589) app_init: Project name:     magi
I (593) app_init: App version:      v0.9.4-56-gf175cbe-dirty
I (598) app_init: Compile time:     Aug  4 2026 16:17:40
I (603) app_init: ELF file SHA256:  d2a93af20...
I (608) app_init: ESP-IDF:          v5.5.1-dirty
I (612) efuse_init: Min chip rev:     v0.0
I (616) efuse_init: Max chip rev:     v3.99 
I (620) efuse_init: Chip rev:         v3.1
I (624) heap_init: Initializing. RAM available for dynamic allocation:
I (630) heap_init: At 3FFAFF10 len 000000F0 (0 KiB): DRAM
I (635) heap_init: At 3FFB6388 len 00001C78 (7 KiB): DRAM
I (640) heap_init: At 3FFB9A20 len 00004108 (16 KiB): DRAM
I (645) heap_init: At 3FFDB8C8 len 00004738 (17 KiB): DRAM
I (651) heap_init: At 3FFE0440 len 00003AE0 (14 KiB): D/IRAM
I (656) heap_init: At 3FFE4350 len 0001BCB0 (111 KiB): D/IRAM
I (661) heap_init: At 4009A4CC len 00005B34 (22 KiB): IRAM
I (668) spi_flash: detected chip: generic
I (670) spi_flash: flash io: dio
I (674) coexist: coex firmware version: b0bcc39
I (679) main_task: Started on CPU0
I (682) main_task: Calling app_main()
I (686) gc9a01: LCD panel create success, version: 2.0.4
I (1048) KEY: Direct GPIO keys init OK
I (1048) KEY: UP=13 DOWN=12 OK=14 BACK=27 TRIG=26
W (1058) BRAIN: nvs_open(read) failed, using defaults: ESP_ERR_NVS_NOT_FOUND
I (1065) battery_process: Battery process initialized
I (1065) battery_process: Battery adc=2380 voltage=3834mV percent=59 level=2
I (1066) BRAIN: Brain initialized
I (1070) MAIN: bluetooth_init completed
W (1124) BRAIN: STATE current=1 loaded=1 splash_done=0
I (1152) MAIN: UI init OK
PCF = 0xFF
I (1153) main_task: Returned from app_main()
I (10040) BRAIN: Key event: 3, current_page: 1
I (10040) battery_process: Battery adc=2380 voltage=3834mV percent=59 level=2
W (10060) BRAIN: STATE current=2 loaded=1 splash_done=1
W (10060) BRAIN: TRANSITION requested: 1 -> 2
I (10073) BRAIN: Prepared page 2
I (10074) BRAIN: Loaded page 2
W (10157) BRAIN: STATE current=2 loaded=2 splash_done=1
I (12511) BRAIN: Key event: 2, current_page: 2
I (12511) battery_process: Battery adc=2350 voltage=3786mV percent=54 level=2
I (12751) BRAIN: Key event: 2, current_page: 2
I (12752) battery_process: Battery adc=2229 voltage=3592mV percent=32 level=1
I (14042) BRAIN: Key event: 3, current_page: 2
I (14042) battery_process: Battery adc=2536 voltage=4086mV percent=87 level=4
W (14043) BRAIN: STATE current=6 loaded=2 splash_done=1
W (14047) BRAIN: TRANSITION requested: 2 -> 6
I (14107) BRAIN: Prepared page 6
I (14109) BRAIN: Loaded page 6
I (14110) BRAIN: Destroyed PAGE_MAIN_MENU
W (14205) BRAIN: STATE current=6 loaded=6 splash_done=1
I (17993) BRAIN: Key event: 4, current_page: 6
I (17993) battery_process: Battery adc=2547 voltage=4104mV percent=89 level=4
W (18016) BRAIN: STATE current=2 loaded=6 splash_done=1
W (18016) BRAIN: TRANSITION requested: 6 -> 2
I (18029) BRAIN: Prepared page 2
I (18030) BRAIN: Loaded page 2
I (18032) BRAIN: Destroyed PAGE_MEMORY
W (18115) BRAIN: STATE current=2 loaded=2 splash_done=1
I (20623) BRAIN: Key event: 1, current_page: 2
I (20623) battery_process: Battery adc=2115 voltage=3408mV percent=12 level=1
I (21983) BRAIN: Key event: 3, current_page: 2
I (21983) battery_process: Battery adc=2279 voltage=3672mV percent=41 level=2
I (22008) BTDM_INIT: BT controller compile version [722c907]
I (22009) BTDM_INIT: Bluetooth MAC: b4:bf:e9:11:89:2e
I (22012) phy_init: phy_version 4861,b71b5ad,Aug  5 2025,11:16:06
W (22013) phy_init: failed to load RF calibration data (0x1102), falling back to full calibration
I (22096) phy_init: Saving new calibration data due to checksum failure or outdated calibration data, mode(2)
I (22614) BT: Bluetooth Classic (SPP) initialized with Legacy Pairing enforced.
I (22615) BT: Device Name: GOLDEN LAB BRAIN
I (22615) BRAIN: Bluetooth enabled by scan-mode policy
W (22620) BRAIN: STATE current=5 loaded=2 splash_done=1
W (22624) BRAIN: TRANSITION requested: 2 -> 5
I (22675) BRAIN: Prepared page 5
I (22676) BRAIN: Loaded page 5
I (22677) BRAIN: Destroyed PAGE_MAIN_MENU
I (22681) BRAIN: PAGE_SEND entered: total_active=0, selected=1
W (22757) BRAIN: STATE current=5 loaded=5 splash_done=1
I (26284) BRAIN: Key event: 4, current_page: 5
I (26284) battery_process: Battery adc=2263 voltage=3646mV percent=38 level=2
I (26504) BT: Bluetooth disabled
I (26505) BRAIN: Bluetooth disabled by memory-mode policy
W (26505) BRAIN: STATE current=2 loaded=5 splash_done=1
W (26507) BRAIN: TRANSITION requested: 5 -> 2
I (26525) BRAIN: Prepared page 2
I (26526) BRAIN: Loaded page 2
I (26528) BRAIN: Destroyed PAGE_SEND
W (26611) BRAIN: STATE current=2 loaded=2 splash_done=1
I (27995) BRAIN: Key event: 4, current_page: 2
I (27995) battery_process: Battery adc=2343 voltage=3776mV percent=52 level=2
I (32775) BRAIN: Key event: 1, current_page: 2
I (32775) battery_process: Battery adc=2406 voltage=3876mV percent=64 level=3
I (34825) BRAIN: Key event: 3, current_page: 2
I (34825) battery_process: Battery adc=2318 voltage=3734mV percent=48 level=2
W (34847) BRAIN: STATE current=3 loaded=2 splash_done=1
W (34847) BRAIN: TRANSITION requested: 2 -> 3
I (34858) BRAIN: Prepared page 3
I (34859) BRAIN: Loaded page 3
I (34860) BRAIN: Destroyed PAGE_MAIN_MENU
W (34936) BRAIN: STATE current=3 loaded=3 splash_done=1
I (37296) BRAIN: Key event: 1, current_page: 3
I (37296) battery_process: Battery adc=2478 voltage=3992mV percent=76 level=3
I (39586) BRAIN: Key event: 3, current_page: 3
I (39586) SCAN_PROCESS: Scan process initialized
I (39587) SCAN_PROCESS: Scan process started. mode=3
I (39588) BRAIN: Brain: Scan mode=3, sub_state=RUNNING, phase=0
I (39594) battery_process: Battery adc=2321 voltage=3740mV percent=48 level=2
W (39609) BTDM_INIT: esp_bt_controller_rom_mem_release already released, mode 1
I (39609) BTDM_INIT: BT controller compile version [722c907]
I (39614) BTDM_INIT: Bluetooth MAC: b4:bf:e9:11:89:2e
I (40158) BT: Bluetooth Classic (SPP) initialized with Legacy Pairing enforced.
I (40159) BT: Device Name: GOLDEN LAB BRAIN
I (40159) BRAIN: Bluetooth enabled by scan-mode policy
W (40164) BRAIN: STATE current=4 loaded=3 splash_done=1
W (40168) BRAIN: TRANSITION requested: 3 -> 4
I (40245) BRAIN: Prepared page 4
I (40247) BRAIN: Loaded page 4
I (40248) BRAIN: Destroyed PAGE_SCAN
W (40350) BRAIN: STATE current=4 loaded=4 splash_done=1
I (42541) BRAIN: Key event: 5, current_page: 4
I (42541) SCAN_PROCESS: capture_one raw=2049 pulse=1
I (42541) BRAIN: Manual capture done. pulse=1
I (42543) battery_process: Battery adc=2593 voltage=4178mV percent=97 level=4
I (53729) BRAIN: Key event: 5, current_page: 4
I (53729) SCAN_PROCESS: capture_one raw=2030 pulse=2
I (53730) BRAIN: Manual capture done. pulse=2
I (53731) battery_process: Battery adc=2247 voltage=3620mV percent=35 level=1
I (54458) BRAIN: Key event: 5, current_page: 4
I (54458) SCAN_PROCESS: capture_one raw=2064 pulse=3
I (54459) BRAIN: Manual capture done. pulse=3
I (54460) battery_process: Battery adc=2265 voltage=3650mV percent=38 level=2
I (54917) BRAIN: Key event: 5, current_page: 4
I (54917) SCAN_PROCESS: capture_one raw=2239 pulse=4
I (54918) BRAIN: Manual capture done. pulse=4
I (54919) battery_process: Battery adc=2122 voltage=3420mV percent=13 level=1
I (55196) BRAIN: Key event: 5, current_page: 4
I (55196) SCAN_PROCESS: capture_one raw=2179 pulse=5
I (55197) BRAIN: Manual capture done. pulse=5
I (55198) battery_process: Battery adc=2070 voltage=3336mV percent=4 level=0
I (55445) BRAIN: Key event: 5, current_page: 4
I (55445) SCAN_PROCESS: capture_one raw=1860 pulse=6
I (55446) BRAIN: Manual capture done. pulse=6
I (55447) battery_process: Battery adc=2252 voltage=3628mV percent=36 level=2
I (55704) BRAIN: Key event: 5, current_page: 4
I (55704) SCAN_PROCESS: capture_one raw=2105 pulse=7
I (55705) BRAIN: Manual capture done. pulse=7
I (55706) battery_process: Battery adc=2369 voltage=3818mV percent=57 level=2
I (55923) BRAIN: Key event: 5, current_page: 4
I (55923) SCAN_PROCESS: capture_one raw=1999 pulse=8
I (55924) BRAIN: Manual capture done. pulse=8
I (55925) battery_process: Battery adc=2276 voltage=3668mV percent=40 level=2
I (56132) BRAIN: Key event: 5, current_page: 4
I (56132) SCAN_PROCESS: capture_one raw=2204 pulse=9
I (56133) BRAIN: Manual capture done. pulse=9
I (56134) battery_process: Battery adc=2502 voltage=4032mV percent=81 level=3
I (56331) BRAIN: Key event: 5, current_page: 4
I (56331) SCAN_PROCESS: capture_one raw=1858 pulse=10
I (56332) BRAIN: Manual capture done. pulse=10
I (56333) battery_process: Battery adc=2338 voltage=3768mV percent=52 level=2
I (56600) BRAIN: Key event: 5, current_page: 4
I (56600) SCAN_PROCESS: capture_one raw=2107 pulse=11
I (56601) BRAIN: Manual capture done. pulse=11
I (56602) battery_process: Battery adc=2349 voltage=3784mV percent=53 level=2
I (56809) BRAIN: Key event: 5, current_page: 4
I (56809) SCAN_PROCESS: capture_one raw=2188 pulse=12
I (56810) BRAIN: Manual capture done. pulse=12
I (56811) battery_process: Battery adc=2262 voltage=3644mV percent=38 level=2
I (57138) BRAIN: Key event: 5, current_page: 4
I (57138) SCAN_PROCESS: capture_one raw=2184 pulse=13
I (57139) BRAIN: Manual capture done. pulse=13
I (57140) battery_process: Battery adc=2094 voltage=3374mV percent=8 level=0
I (57537) BRAIN: Key event: 5, current_page: 4
I (57537) SCAN_PROCESS: capture_one raw=2207 pulse=14
I (57538) BRAIN: Manual capture done. pulse=14
I (57539) battery_process: Battery adc=2570 voltage=4142mV percent=93 level=4
I (58056) BRAIN: Key event: 5, current_page: 4
I (58056) SCAN_PROCESS: capture_one raw=1914 pulse=15
I (58057) BRAIN: Manual capture done. pulse=15
I (58058) battery_process: Battery adc=2104 voltage=3390mV percent=10 level=0
I (59745) BRAIN: Key event: 4, current_page: 4
I (59745) SCAN_PROCESS: Scan process stop requested
I (59746) BRAIN: Saving scan: total=15, calibration=0
I (59767) storage_lfs: Save scan index item: id=1, points=15, calibration=0
I (59789) BRAIN: Scan saved successfully, id=1
I (59790) BRAIN: Process stopped and saved by BACK
I (59790) battery_process: Battery adc=2168 voltage=3494mV percent=21 level=1
I (60514) BRAIN: Key event: 4, current_page: 4
I (60514) BRAIN: Leaving scan page by second BACK
I (60515) battery_process: Battery adc=2215 voltage=3568mV percent=29 level=1
I (60740) BT: Bluetooth disabled
I (60740) BRAIN: Bluetooth disabled by memory-mode policy
W (60740) BRAIN: STATE current=3 loaded=4 splash_done=1
W (60742) BRAIN: TRANSITION requested: 4 -> 3
I (60758) BRAIN: Prepared page 3
I (60759) BRAIN: Loaded page 3
I (60761) BRAIN: Destroyed PAGE_SCAN_PAGE
W (60818) BRAIN: STATE current=3 loaded=3 splash_done=1
I (71009) BRAIN: Key event: 4, current_page: 3
I (71009) battery_process: Battery adc=2117 voltage=3412mV percent=12 level=1
W (71026) BRAIN: STATE current=2 loaded=3 splash_done=1
W (71026) BRAIN: TRANSITION requested: 3 -> 2
I (71039) BRAIN: Prepared page 2
I (71040) BRAIN: Loaded page 2
I (71041) BRAIN: Destroyed PAGE_SCAN
W (71125) BRAIN: STATE current=2 loaded=2 splash_done=1
I (72460) BRAIN: Key event: 1, current_page: 2
I (72460) battery_process: Battery adc=2081 voltage=3352mV percent=5 level=0
I (72670) BRAIN: Key event: 1, current_page: 2
I (72670) battery_process: Battery adc=2128 voltage=3428mV percent=14 level=1
I (74261) BRAIN: Key event: 3, current_page: 2
I (74261) battery_process: Battery adc=2503 voltage=4034mV percent=81 level=3
W (74262) BRAIN: STATE current=7 loaded=2 splash_done=1
W (74266) BRAIN: TRANSITION requested: 2 -> 7
I (74397) BRAIN: Prepared page 7
I (74399) BRAIN: Loaded page 7
I (74400) BRAIN: Destroyed PAGE_MAIN_MENU
W (74510) BRAIN: STATE current=7 loaded=7 splash_done=1
I (78722) BRAIN: Key event: 3, current_page: 7
I (78723) battery_process: Battery adc=2096 voltage=3378mV percent=8 level=0
I (80063) BRAIN: Key event: 1, current_page: 7
I (80063) BRAIN: Setting index 0 updated to: ON
I (80067) BRAIN: Settings saved
I (80071) battery_process: Battery adc=2376 voltage=3828mV percent=58 level=2
I (81621) BRAIN: Key event: 4, current_page: 7
I (81622) battery_process: Battery adc=2278 voltage=3670mV percent=41 level=2
I (82503) BRAIN: Key event: 4, current_page: 7
I (82503) battery_process: Battery adc=2427 voltage=3910mV percent=67 level=3
W (82531) BRAIN: STATE current=2 loaded=7 splash_done=1
W (82531) BRAIN: TRANSITION requested: 7 -> 2
I (82544) BRAIN: Prepared page 2
I (82546) BRAIN: Loaded page 2
I (82550) BRAIN: Destroyed PAGE_SETTING
W (82633) BRAIN: STATE current=2 loaded=2 splash_done=1
I (84404) BRAIN: Key event: 2, current_page: 2
I (84404) battery_process: Battery adc=2355 voltage=3794mV percent=54 level=2
I (84944) BRAIN: Key event: 2, current_page: 2
I (84944) battery_process: Battery adc=2566 voltage=4134mV percent=92 level=4
I (86535) BRAIN: Key event: 3, current_page: 2
I (86535) battery_process: Battery adc=2243 voltage=3614mV percent=34 level=1
W (86539) BRAIN: STATE current=3 loaded=2 splash_done=1
W (86540) BRAIN: TRANSITION requested: 2 -> 3
I (86555) BRAIN: Prepared page 3
I (86557) BRAIN: Loaded page 3
I (86558) BRAIN: Destroyed PAGE_MAIN_MENU
W (86633) BRAIN: STATE current=3 loaded=3 splash_done=1
I (88366) BRAIN: Key event: 1, current_page: 3
I (88366) battery_process: Battery adc=2500 voltage=4028mV percent=80 level=3
I (90446) BRAIN: Key event: 3, current_page: 3
I (90446) SCAN_PROCESS: Scan process initialized
I (90447) SCAN_PROCESS: Scan process started. mode=3
I (90448) BRAIN: Brain: Scan mode=3, sub_state=RUNNING, phase=0
I (90454) battery_process: Battery adc=2258 voltage=3638mV percent=37 level=2
W (90470) BTDM_INIT: esp_bt_controller_rom_mem_release already released, mode 1
I (90470) BTDM_INIT: BT controller compile version [722c907]
I (90474) BTDM_INIT: Bluetooth MAC: b4:bf:e9:11:89:2e
I (91029) BT: Bluetooth Classic (SPP) initialized with Legacy Pairing enforced.
I (91030) BT: Device Name: GOLDEN LAB BRAIN
I (91030) BRAIN: Bluetooth enabled by scan-mode policy
W (91035) BRAIN: STATE current=4 loaded=3 splash_done=1
W (91040) BRAIN: TRANSITION requested: 3 -> 4
I (91116) BRAIN: Prepared page 4
I (91118) BRAIN: Loaded page 4
I (91119) BRAIN: Destroyed PAGE_SCAN
W (91221) BRAIN: STATE current=4 loaded=4 splash_done=1
I (110111) BRAIN: Key event: 5, current_page: 4
I (110111) SCAN_PROCESS: Calibration started. samples=16 mode=3
I (110111) SCAN_PROCESS: Calibration finished. actual=16
I (110114) BRAIN: Manual calibration done. phase=1
I (110119) battery_process: Battery adc=2475 voltage=3988mV percent=76 level=3
I (117326) BRAIN: Key event: 5, current_page: 4
I (117326) SCAN_PROCESS: capture_one raw=2202 pulse=1
I (117327) BRAIN: Manual capture done. pulse=1
I (117328) battery_process: Battery adc=2369 voltage=3818mV percent=57 level=2
I (118085) BRAIN: Key event: 5, current_page: 4
I (118085) SCAN_PROCESS: capture_one raw=1880 pulse=2
I (118086) BRAIN: Manual capture done. pulse=2
I (118087) battery_process: Battery adc=2558 voltage=4122mV percent=91 level=4
I (118524) BRAIN: Key event: 5, current_page: 4
I (118524) SCAN_PROCESS: capture_one raw=2155 pulse=3
I (118525) BRAIN: Manual capture done. pulse=3
I (118526) battery_process: Battery adc=2403 voltage=3872mV percent=63 level=3
I (118853) BRAIN: Key event: 5, current_page: 4
I (118853) SCAN_PROCESS: capture_one raw=2012 pulse=4
I (118854) BRAIN: Manual capture done. pulse=4
I (118855) battery_process: Battery adc=2302 voltage=3710mV percent=45 level=2
I (119162) BRAIN: Key event: 5, current_page: 4
I (119162) SCAN_PROCESS: capture_one raw=1938 pulse=5
I (119163) BRAIN: Manual capture done. pulse=5
I (119164) battery_process: Battery adc=2404 voltage=3874mV percent=63 level=3
I (119451) BRAIN: Key event: 5, current_page: 4
I (119451) SCAN_PROCESS: capture_one raw=2093 pulse=6
I (119452) BRAIN: Manual capture done. pulse=6
I (119453) battery_process: Battery adc=2270 voltage=3658mV percent=39 level=2
I (119750) BRAIN: Key event: 5, current_page: 4
I (119750) SCAN_PROCESS: capture_one raw=1853 pulse=7
I (119751) BRAIN: Manual capture done. pulse=7
I (119752) battery_process: Battery adc=2280 voltage=3674mV percent=41 level=2
I (120029) BRAIN: Key event: 5, current_page: 4
I (120029) SCAN_PROCESS: capture_one raw=2074 pulse=8
I (120030) BRAIN: Manual capture done. pulse=8
I (120031) battery_process: Battery adc=2533 voltage=4082mV percent=86 level=4
I (120338) BRAIN: Key event: 5, current_page: 4
I (120338) SCAN_PROCESS: capture_one raw=1894 pulse=9
I (120339) BRAIN: Manual capture done. pulse=9
I (120340) battery_process: Battery adc=2301 voltage=3708mV percent=45 level=2
I (120787) BRAIN: Key event: 5, current_page: 4
I (120787) SCAN_PROCESS: capture_one raw=2138 pulse=10
I (120788) BRAIN: Manual capture done. pulse=10
I (120790) battery_process: Battery adc=2151 voltage=3466mV percent=18 level=1
I (122156) BRAIN: Key event: 4, current_page: 4
I (122156) SCAN_PROCESS: Scan process stop requested
I (122157) BRAIN: Saving scan: total=26, calibration=16
I (122191) storage_lfs: Save scan index item: id=2, points=26, calibration=16
I (122227) BRAIN: Scan saved successfully, id=2
I (122227) BRAIN: Process stopped and saved by BACK
I (122228) battery_process: Battery adc=2559 voltage=4124mV percent=91 level=4
I (122462) BRAIN: Key event: 4, current_page: 4
I (122462) BRAIN: Leaving scan page by second BACK
I (122463) battery_process: Battery adc=2119 voltage=3414mV percent=12 level=1
I (122690) BT: Bluetooth disabled
I (122690) BRAIN: Bluetooth disabled by memory-mode policy
W (122690) BRAIN: STATE current=3 loaded=4 splash_done=1
W (122692) BRAIN: TRANSITION requested: 4 -> 3
I (122708) BRAIN: Prepared page 3
I (122709) BRAIN: Loaded page 3
I (122711) BRAIN: Destroyed PAGE_SCAN_PAGE
W (122768) BRAIN: STATE current=3 loaded=3 splash_done=1
I (123727) BRAIN: Key event: 4, current_page: 3
I (123727) battery_process: Battery adc=2283 voltage=3678mV percent=42 level=2
W (123736) BRAIN: STATE current=2 loaded=3 splash_done=1
W (123736) BRAIN: TRANSITION requested: 3 -> 2
I (123750) BRAIN: Prepared page 2
I (123751) BRAIN: Loaded page 2
I (123752) BRAIN: Destroyed PAGE_SCAN
W (123835) BRAIN: STATE current=2 loaded=2 splash_done=1
I (126108) BRAIN: Key event: 2, current_page: 2
I (126108) battery_process: Battery adc=2158 voltage=3478mV percent=19 level=1
I (127628) BRAIN: Key event: 3, current_page: 2
I (127628) battery_process: Battery adc=2403 voltage=3872mV percent=63 level=3
W (127652) BTDM_INIT: esp_bt_controller_rom_mem_release already released, mode 1
I (127652) BTDM_INIT: BT controller compile version [722c907]
I (127654) BTDM_INIT: Bluetooth MAC: b4:bf:e9:11:89:2e
I (128184) BT: Bluetooth Classic (SPP) initialized with Legacy Pairing enforced.
I (128185) BT: Device Name: GOLDEN LAB BRAIN
I (128185) BRAIN: Bluetooth enabled by scan-mode policy
W (128189) BRAIN: STATE current=5 loaded=2 splash_done=1
W (128194) BRAIN: TRANSITION requested: 2 -> 5
I (128245) BRAIN: Prepared page 5
I (128246) BRAIN: Loaded page 5
I (128247) BRAIN: Destroyed PAGE_MAIN_MENU
I (128259) BRAIN: PAGE_SEND entered: total_active=2, selected=1
I (128261) BRAIN: Send UI: total=15, calibration=0, displayed=15
W (128338) BRAIN: STATE current=5 loaded=5 splash_done=1
I (132419) BRAIN: Key event: 1, current_page: 5
I (132419) BRAIN: Send selection: 2/2
I (132420) battery_process: Battery adc=2471 voltage=3982mV percent=75 level=3
I (132448) BRAIN: Send UI: total=26, calibration=16, displayed=10
I (133983) BRAIN: Key event: 2, current_page: 5
I (133983) BRAIN: Send selection: 1/2
I (133984) battery_process: Battery adc=2510 voltage=4044mV percent=82 level=3
I (133989) BRAIN: Send UI: total=15, calibration=0, displayed=15
I (166603) BT: BT GAP auth success, device: Redmi 12C
I (173167) BRAIN: Key event: 3, current_page: 5
I (173168) BRAIN: KEY_OK pressed. Preparing to transmit scan 1/2...
I (173197) BRAIN: Starting Bluetooth stream for 15 points...
I (173197) BRAIN: Bluetooth stream started successfully.
I (173198) battery_process: Battery adc=2110 voltage=3400mV percent=11 level=1
I (173289) BRAIN: Marking scan 1 as sent in LittleFS...
I (184083) BRAIN: Key event: 1, current_page: 5
I (184083) BRAIN: Send selection: 2/2
I (184084) battery_process: Battery adc=2142 voltage=3452mV percent=16 level=1
I (184087) BRAIN: Send UI: total=26, calibration=16, displayed=10
I (185887) BRAIN: Key event: 3, current_page: 5
I (185888) BRAIN: KEY_OK pressed. Preparing to transmit scan 2/2...
I (185906) BRAIN: Starting Bluetooth stream for 26 points...
I (185907) BRAIN: Bluetooth stream started successfully.
I (185907) battery_process: Battery adc=2323 voltage=3744mV percent=49 level=2
I (186096) BRAIN: Marking scan 2 as sent in LittleFS...
I (197253) BRAIN: Key event: 4, current_page: 5
I (197253) battery_process: Battery adc=2437 voltage=3926mV percent=69 level=3
I (198397) BT: Bluetooth disabled
I (198398) BRAIN: Bluetooth disabled by memory-mode policy
W (198398) BRAIN: STATE current=2 loaded=5 splash_done=1
W (198400) BRAIN: TRANSITION requested: 5 -> 2
I (198418) BRAIN: Prepared page 2
I (198419) BRAIN: Loaded page 2
I (198421) BRAIN: Destroyed PAGE_SEND
W (198504) BRAIN: STATE current=2 loaded=2 splash_done=1
I (199394) BRAIN: Key event: 4, current_page: 2
I (199394) battery_process: Battery adc=2049 voltage=3302mV percent=0 level=0
I (202574) BRAIN: Key event: 3, current_page: 2
I (202574) battery_process: Battery adc=2526 voltage=4070mV percent=85 level=3
W (202585) BTDM_INIT: esp_bt_controller_rom_mem_release already released, mode 1
I (202585) BTDM_INIT: BT controller compile version [722c907]
I (202590) BTDM_INIT: Bluetooth MAC: b4:bf:e9:11:89:2e
I (203135) BT: Bluetooth Classic (SPP) initialized with Legacy Pairing enforced.
I (203136) BT: Device Name: GOLDEN LAB BRAIN
I (203136) BRAIN: Bluetooth enabled by scan-mode policy
W (203140) BRAIN: STATE current=5 loaded=2 splash_done=1
W (203145) BRAIN: TRANSITION requested: 2 -> 5
I (203196) BRAIN: Prepared page 5
I (203197) BRAIN: Loaded page 5
I (203198) BRAIN: Destroyed PAGE_MAIN_MENU
I (203210) BRAIN: PAGE_SEND entered: total_active=2, selected=1
I (203212) BRAIN: Send UI: total=15, calibration=0, displayed=15
W (203289) BRAIN: STATE current=5 loaded=5 splash_done=1
I (204954) BRAIN: Key event: 4, current_page: 5
I (204954) battery_process: Battery adc=2456 voltage=3958mV percent=73 level=3
I (205199) BT: Bluetooth disabled
I (205199) BRAIN: Bluetooth disabled by memory-mode policy
W (205200) BRAIN: STATE current=2 loaded=5 splash_done=1
W (205201) BRAIN: TRANSITION requested: 5 -> 2
I (205220) BRAIN: Prepared page 2
I (205221) BRAIN: Loaded page 2
I (205222) BRAIN: Destroyed PAGE_SEND
W (205306) BRAIN: STATE current=2 loaded=2 splash_done=1
I (207075) BRAIN: Key event: 1, current_page: 2
I (207075) battery_process: Battery adc=2531 voltage=4078mV percent=86 level=4
I (208485) BRAIN: Key event: 3, current_page: 2
I (208485) battery_process: Battery adc=2338 voltage=3768mV percent=52 level=2
W (208502) BRAIN: STATE current=3 loaded=2 splash_done=1
W (208502) BRAIN: TRANSITION requested: 2 -> 3
I (208513) BRAIN: Prepared page 3
I (208514) BRAIN: Loaded page 3
I (208515) BRAIN: Destroyed PAGE_MAIN_MENU
W (208591) BRAIN: STATE current=3 loaded=3 splash_done=1
I (211396) BRAIN: Key event: 3, current_page: 3
I (211396) SCAN_PROCESS: Scan process initialized
I (211396) SCAN_PROCESS: Scan process started. mode=0
I (211398) BRAIN: Brain: Scan mode=0, sub_state=RUNNING, phase=0
I (211404) battery_process: Battery adc=2397 voltage=3862mV percent=62 level=3
W (211412) BTDM_INIT: esp_bt_controller_rom_mem_release already released, mode 1
I (211418) BTDM_INIT: BT controller compile version [722c907]
I (211424) BTDM_INIT: Bluetooth MAC: b4:bf:e9:11:89:2e
I (211968) BT: Bluetooth Classic (SPP) initialized with Legacy Pairing enforced.
I (211969) BT: Device Name: GOLDEN LAB BRAIN
I (211969) BRAIN: Bluetooth enabled by scan-mode policy
W (211974) BRAIN: STATE current=4 loaded=3 splash_done=1
W (211979) BRAIN: TRANSITION requested: 3 -> 4
I (212055) BRAIN: Prepared page 4
I (212057) BRAIN: Loaded page 4
I (212058) BRAIN: Destroyed PAGE_SCAN
W (212161) BRAIN: STATE current=4 loaded=4 splash_done=1
I (215682) BRAIN: Key event: 4, current_page: 4
I (215682) SCAN_PROCESS: Scan process stop requested
I (215682) BRAIN: Scan stopped without saving, mode=0
I (215684) BRAIN: Process stopped and saved by BACK
I (215689) battery_process: Battery adc=2443 voltage=3936mV percent=70 level=3
I (216386) BRAIN: Key event: 4, current_page: 4
I (216386) BRAIN: Leaving scan page by second BACK
I (216387) battery_process: Battery adc=2151 voltage=3466mV percent=18 level=1
I (216619) BT: Bluetooth disabled
I (216619) BRAIN: Bluetooth disabled by memory-mode policy
W (216619) BRAIN: STATE current=3 loaded=4 splash_done=1
W (216621) BRAIN: TRANSITION requested: 4 -> 3
I (216637) BRAIN: Prepared page 3
I (216638) BRAIN: Loaded page 3
I (216640) BRAIN: Destroyed PAGE_SCAN_PAGE
W (216717) BRAIN: STATE current=3 loaded=3 splash_done=1
I (222371) BRAIN: Key event: 1, current_page: 3
I (222371) battery_process: Battery adc=2437 voltage=3926mV percent=69 level=3
I (224191) BRAIN: Key event: 3, current_page: 3
I (224191) SCAN_PROCESS: Scan process initialized
I (224192) SCAN_PROCESS: Scan process started. mode=3
I (224194) BRAIN: Brain: Scan mode=3, sub_state=RUNNING, phase=0
I (224199) battery_process: Battery adc=2459 voltage=3962mV percent=73 level=3
W (224211) BTDM_INIT: esp_bt_controller_rom_mem_release already released, mode 1
I (224214) BTDM_INIT: BT controller compile version [722c907]
I (224220) BTDM_INIT: Bluetooth MAC: b4:bf:e9:11:89:2e
I (224752) BT: Bluetooth Classic (SPP) initialized with Legacy Pairing enforced.
I (224753) BT: Device Name: GOLDEN LAB BRAIN
I (224753) BRAIN: Bluetooth enabled by scan-mode policy
W (224757) BRAIN: STATE current=4 loaded=3 splash_done=1
W (224762) BRAIN: TRANSITION requested: 3 -> 4
I (224839) BRAIN: Prepared page 4
I (224841) BRAIN: Loaded page 4
I (224841) BRAIN: Destroyed PAGE_SCAN
W (224943) BRAIN: STATE current=4 loaded=4 splash_done=1
I (226346) BRAIN: Key event: 4, current_page: 4
I (226346) SCAN_PROCESS: Scan process stop requested
W (226346) BRAIN: No scan data available to save
E (226348) BRAIN: Failed to save scan: ESP_ERR_INVALID_STATE
I (226353) BRAIN: Process stopped and saved by BACK
I (226358) battery_process: Battery adc=2132 voltage=3436mV percent=15 level=1
I (226945) BRAIN: Key event: 4, current_page: 4
I (226945) BRAIN: Leaving scan page by second BACK
I (226946) battery_process: Battery adc=2102 voltage=3386mV percent=9 level=0
I (227161) BT: Bluetooth disabled
I (227161) BRAIN: Bluetooth disabled by memory-mode policy
W (227161) BRAIN: STATE current=3 loaded=4 splash_done=1
W (227163) BRAIN: TRANSITION requested: 4 -> 3
I (227179) BRAIN: Prepared page 3
I (227180) BRAIN: Loaded page 3
I (227182) BRAIN: Destroyed PAGE_SCAN_PAGE
W (227239) BRAIN: STATE current=3 loaded=3 splash_done=1
I (231780) BRAIN: Key event: 1, current_page: 3
I (231780) battery_process: Battery adc=2054 voltage=3310mV percent=1 level=0
I (232971) BRAIN: Key event: 3, current_page: 3
I (232971) SCAN_PROCESS: Scan process initialized
I (232972) SCAN_PROCESS: Scan process started. mode=2
I (232974) BRAIN: Brain: Scan mode=2, sub_state=RUNNING, phase=0
W (232978) BTDM_INIT: esp_bt_controller_rom_mem_release already released, mode 1
I (232980) battery_process: Battery adc=2297 voltage=3702mV percent=44 level=2
I (232987) BTDM_INIT: BT controller compile version [722c907]
I (233000) BTDM_INIT: Bluetooth MAC: b4:bf:e9:11:89:2e
I (233544) BT: Bluetooth Classic (SPP) initialized with Legacy Pairing enforced.
I (233545) BT: Device Name: GOLDEN LAB BRAIN
I (233545) BRAIN: Bluetooth enabled by scan-mode policy
W (233550) BRAIN: STATE current=4 loaded=3 splash_done=1
W (233555) BRAIN: TRANSITION requested: 3 -> 4
I (233631) BRAIN: Prepared page 4
I (233633) BRAIN: Loaded page 4
I (233634) BRAIN: Destroyed PAGE_SCAN
W (233737) BRAIN: STATE current=4 loaded=4 splash_done=1
I (235813) BRAIN: Key event: 4, current_page: 4
I (235813) SCAN_PROCESS: Scan process stop requested
W (235813) BRAIN: No scan data available to save
E (235815) BRAIN: Failed to save scan: ESP_ERR_INVALID_STATE
I (235820) BRAIN: Process stopped and saved by BACK
I (235825) battery_process: Battery adc=2117 voltage=3412mV percent=12 level=1
I (237442) BRAIN: Key event: 4, current_page: 4
I (237442) BRAIN: Leaving scan page by second BACK
I (237443) battery_process: Battery adc=2062 voltage=3322mV percent=2 level=0
I (237686) BT: Bluetooth disabled
I (237686) BRAIN: Bluetooth disabled by memory-mode policy
W (237686) BRAIN: STATE current=3 loaded=4 splash_done=1
W (237688) BRAIN: TRANSITION requested: 4 -> 3
I (237704) BRAIN: Prepared page 3
I (237705) BRAIN: Loaded page 3
I (237707) BRAIN: Destroyed PAGE_SCAN_PAGE
W (237764) BRAIN: STATE current=3 loaded=3 splash_done=1
I (240357) BRAIN: Key event: 4, current_page: 3
I (240357) battery_process: Battery adc=2322 voltage=3742mV percent=49 level=2
W (240387) BRAIN: STATE current=2 loaded=3 splash_done=1
W (240387) BRAIN: TRANSITION requested: 3 -> 2
I (240400) BRAIN: Prepared page 2
I (240402) BRAIN: Loaded page 2
I (240402) BRAIN: Destroyed PAGE_SCAN
W (240486) BRAIN: STATE current=2 loaded=2 splash_done=1
I (241728) BRAIN: Key event: 1, current_page: 2
I (241728) battery_process: Battery adc=2280 voltage=3674mV percent=41 level=2
I (242318) BRAIN: Key event: 1, current_page: 2
I (242318) battery_process: Battery adc=2429 voltage=3914mV percent=68 level=3
I (243499) BRAIN: Key event: 3, current_page: 2
I (243499) battery_process: Battery adc=2517 voltage=4056mV percent=84 level=3
W (243510) BRAIN: STATE current=7 loaded=2 splash_done=1
W (243510) BRAIN: TRANSITION requested: 2 -> 7
I (243637) BRAIN: Prepared page 7
I (243639) BRAIN: Loaded page 7
I (243640) BRAIN: Destroyed PAGE_MAIN_MENU
W (243750) BRAIN: STATE current=7 loaded=7 splash_done=1
I (246490) BRAIN: Key event: 2, current_page: 7
I (246490) battery_process: Battery adc=2175 voltage=3504mV percent=22 level=1
I (247630) BRAIN: Key event: 2, current_page: 7
I (247630) battery_process: Battery adc=2471 voltage=3982mV percent=75 level=3
I (248141) BRAIN: Key event: 2, current_page: 7
I (248141) battery_process: Battery adc=2090 voltage=3368mV percent=7 level=0
I (248672) BRAIN: Key event: 2, current_page: 7
I (248672) battery_process: Battery adc=2398 voltage=3864mV percent=62 level=3
I (249123) BRAIN: Key event: 2, current_page: 7
I (249123) battery_process: Battery adc=2273 voltage=3662mV percent=40 level=2
I (250604) BRAIN: Key event: 2, current_page: 7
I (250604) battery_process: Battery adc=2300 voltage=3706mV percent=45 level=2
I (251735) BRAIN: Key event: 3, current_page: 7
I (251736) battery_process: Battery adc=2141 voltage=3450mV percent=16 level=1
I (255367) BRAIN: Key event: 1, current_page: 7
I (255367) BRAIN: Setting index 6 updated to: ON
I (255374) BRAIN: Settings saved
I (255377) battery_process: Battery adc=2225 voltage=3586mV percent=31 level=1
I (256728) BRAIN: Key event: 4, current_page: 7
I (256729) battery_process: Battery adc=2563 voltage=4130mV percent=92 level=4
I (257490) BRAIN: Key event: 4, current_page: 7
I (257490) battery_process: Battery adc=2362 voltage=3806mV percent=56 level=2
W (257514) BRAIN: STATE current=2 loaded=7 splash_done=1
W (257514) BRAIN: TRANSITION requested: 7 -> 2
I (257527) BRAIN: Prepared page 2
I (257529) BRAIN: Loaded page 2
Guru Meditation Error: Core  0 panic'ed (LoadProhibited). Exception was unhandled.

Core  0 register dump:
PC      : 0x4010d1a1  PS      : 0x00060330  A0      : 0x8010d303  A1      : 0x3ffe2490  
--- 0x4010d1a1: lv_obj_get_index at C:/Users/kimiya/Documents/ESP-IDF/Projects/MG_GUI/managed_components/lvgl__lvgl/src/core/lv_obj_tree.c:591
A2      : 0x00000000  A3      : 0x3ffca394  A4      : 0x00000000  A5      : 0x4010d3ac  
--- 0x4010d3ac: lv_obj_delete_async_cb at C:/Users/kimiya/Documents/ESP-IDF/Projects/MG_GUI/managed_components/lvgl__lvgl/src/core/lv_obj_tree.c:648
A6      : 0x00000000  A7      : 0x3ffca395  A8      : 0x8010d18c  A9      : 0x3ffe2470  
A10     : 0x003ffc85  A11     : 0x3ffca394  A12     : 0x00000000  A13     : 0x4010d3ac  
--- 0x4010d3ac: lv_obj_delete_async_cb at C:/Users/kimiya/Documents/ESP-IDF/Projects/MG_GUI/managed_components/lvgl__lvgl/src/core/lv_obj_tree.c:648
A14     : 0x3ffc85ac  A15     : 0x3ffe2470  SAR     : 0x00000006  EXCCAUSE: 0x0000001c  
EXCVADDR: 0x003ffc8d  LBEG    : 0x4000c2e0  LEND    : 0x4000c2f6  LCOUNT  : 0x00000000  
--- 0x4000c2e0: memcpy in ROM
--- 0x4000c2f6: memcpy in ROM


Backtrace: 0x4010d19e:0x3ffe2490 0x4010d300:0x3ffe24b0 0x4010d1fd:0x3ffe24d0 0x4010d380:0x3ffe24f0 0x400e21e6:0x3ffe2510 0x400dc8c3:0x3ffe2530 0x400dc986:0x3ffe2550 0x400ddb89:0x3ffe2580 0x400d8856:0x3ffe2600 0x40090b7d:0x3ffe2620
--- 0x4010d19e: lv_obj_get_index at C:/Users/kimiya/Documents/ESP-IDF/Projects/MG_GUI/managed_components/lvgl__lvgl/src/core/lv_obj_tree.c:592
--- 0x4010d300: obj_delete_core at C:/Users/kimiya/Documents/ESP-IDF/Projects/MG_GUI/managed_components/lvgl__lvgl/src/core/lv_obj_tree.c:743
--- 0x4010d1fd: obj_delete_core at C:/Users/kimiya/Documents/ESP-IDF/Projects/MG_GUI/managed_components/lvgl__lvgl/src/core/lv_obj_tree.c:688
--- 0x4010d380: lv_obj_delete at C:/Users/kimiya/Documents/ESP-IDF/Projects/MG_GUI/managed_components/lvgl__lvgl/src/core/lv_obj_tree.c:75
--- 0x400e21e6: ui_Setting_screen_destroy at C:/Users/kimiya/Documents/ESP-IDF/Projects/MG_GUI/main/ui/screens/ui_Setting.c:1369
--- 0x400dc8c3: brain_destroy_page at C:/Users/kimiya/Documents/ESP-IDF/Projects/MG_GUI/main/brain.c:985
--- 0x400dc986: brain_transition_to_page at C:/Users/kimiya/Documents/ESP-IDF/Projects/MG_GUI/main/brain.c:1107
--- 0x400ddb89: brain_process_ui_cmds at C:/Users/kimiya/Documents/ESP-IDF/Projects/MG_GUI/main/brain.c:1735
--- 0x400d8856: lvgl_task at C:/Users/kimiya/Documents/ESP-IDF/Projects/MG_GUI/main/main.c:109
--- 0x40090b7d: vPortTaskWrapper at C:/Users/kimiya/esp/v5.5.1/esp-idf/components/freertos/FreeRTOS-Kernel/portable/xtensa/port.c:139




ELF file SHA256: d2a93af20

Rebooting...
ets Jul 29 2019 12:21:46

rst:0xc (SW_CPU_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:2
load:0x3fff0030,len:6380
ho 0 tail 12 room 4
load:0x40078000,len:15916
load:0x40080400,len:3860
--- 0x40080400: _invalid_pc_placeholder at C:/Users/kimiya/esp/v5.5.1/esp-idf/components/xtensa/xtensa_vectors.S:2235
entry 0x40080638
I (29) boot: ESP-IDF v5.5.1-dirty 2nd stage bootloader
I (29) boot: compile time Jul 31 2026 00:28:36
I (29) boot: Multicore bootloader
I (31) boot: chip revision: v3.1
I (34) boot.esp32: SPI Speed      : 40MHz
I (38) boot.esp32: SPI Mode       : DIO
I (41) boot.esp32: SPI Flash Size : 4MB
I (45) boot: Enabling RNG early entropy source...
I (49) boot: Partition Table:
I (52) boot: ## Label            Usage          Type ST Offset   Length
I (58) boot:  0 nvs              WiFi data        01 02 00009000 00006000
I (65) boot:  1 otadata          OTA data         01 00 0000f000 00002000
I (71) boot:  2 phy_init         RF data          01 01 00011000 00001000
I (78) boot:  3 factory          factory app      00 00 00020000 00180000
I (84) boot:  4 storage          Unknown data     01 83 001a0000 00060000
I (91) boot: End of partition table
I (94) boot: Defaulting to factory image
I (98) esp_image: segment 0: paddr=00020020 vaddr=3f400020 size=7082ch (460844) map
I (263) esp_image: segment 1: paddr=00090854 vaddr=3ffbdb60 size=04cb8h ( 19640) load
I (270) esp_image: segment 2: paddr=00095514 vaddr=40080000 size=0ab04h ( 43780) load
I (287) esp_image: segment 3: paddr=000a0020 vaddr=400d0020 size=ae478h (713848) map
I (531) esp_image: segment 4: paddr=0014e4a0 vaddr=4008ab04 size=0f9c8h ( 63944) load
I (557) esp_image: segment 5: paddr=0015de70 vaddr=50000000 size=00020h (    32) load
I (570) boot: Loaded app from partition at offset 0x20000
I (570) boot: Disabling RNG early entropy source...
I (581) cpu_start: Multicore app
I (589) cpu_start: Pro cpu start user code
I (589) cpu_start: cpu freq: 160000000 Hz
I (589) app_init: Application information:
I (589) app_init: Project name:     magi
I (593) app_init: App version:      v0.9.4-56-gf175cbe-dirty
I (598) app_init: Compile time:     Aug  4 2026 16:17:40
I (603) app_init: ELF file SHA256:  d2a93af20...
I (608) app_init: ESP-IDF:          v5.5.1-dirty
I (612) efuse_init: Min chip rev:     v0.0
I (616) efuse_init: Max chip rev:     v3.99 
I (620) efuse_init: Chip rev:         v3.1
I (624) heap_init: Initializing. RAM available for dynamic allocation:
I (630) heap_init: At 3FFAFF10 len 000000F0 (0 KiB): DRAM
I (635) heap_init: At 3FFB6388 len 00001C78 (7 KiB): DRAM
I (640) heap_init: At 3FFB9A20 len 00004108 (16 KiB): DRAM
I (645) heap_init: At 3FFDB8C8 len 00004738 (17 KiB): DRAM
I (650) heap_init: At 3FFE0440 len 00003AE0 (14 KiB): D/IRAM
I (656) heap_init: At 3FFE4350 len 0001BCB0 (111 KiB): D/IRAM
I (661) heap_init: At 4009A4CC len 00005B34 (22 KiB): IRAM
I (668) spi_flash: detected chip: generic
I (670) spi_flash: flash io: dio
I (674) coexist: coex firmware version: b0bcc39
I (678) main_task: Started on CPU0
I (681) main_task: Calling app_main()
I (685) gc9a01: LCD panel create success, version: 2.0.4
I (1047) KEY: Direct GPIO keys init OK
I (1047) KEY: UP=13 DOWN=12 OK=14 BACK=27 TRIG=26
I (1064) BRAIN: Settings loaded
I (1078) battery_process: Battery process initialized
I (1078) battery_process: Battery adc=2380 voltage=3834mV percent=59 level=2
I (1078) BRAIN: Brain initialized
I (1082) MAIN: bluetooth_init completed
W (1136) BRAIN: STATE current=1 loaded=1 splash_done=0
I (1164) MAIN: UI init OK
PCF = 0xFF
I (1165) main_task: Returned from app_main()
I (597002) BRAIN: Key event: 3, current_page: 1
I (597002) battery_process: Battery adc=2380 voltage=3834mV percent=59 level=2
W (597026) BRAIN: STATE current=2 loaded=1 splash_done=1
W (597026) BRAIN: TRANSITION requested: 1 -> 2
I (597039) BRAIN: Prepared page 2
I (597040) BRAIN: Loaded page 2
W (597123) BRAIN: STATE current=2 loaded=2 splash_done=1
I (598823) BRAIN: Key event: 1, current_page: 2
I (598823) battery_process: Battery adc=2350 voltage=3786mV percent=54 level=2
I (599043) BRAIN: Key event: 1, current_page: 2
I (599044) battery_process: Battery adc=2229 voltage=3592mV percent=32 level=1
I (600314) BRAIN: Key event: 3, current_page: 2
I (600314) battery_process: Battery adc=2536 voltage=4086mV percent=87 level=4
W (600321) BRAIN: STATE current=7 loaded=2 splash_done=1
W (600321) BRAIN: TRANSITION requested: 2 -> 7
I (600451) BRAIN: Prepared page 7
I (600453) BRAIN: Loaded page 7
I (600453) BRAIN: Destroyed PAGE_MAIN_MENU
W (600564) BRAIN: STATE current=7 loaded=7 splash_done=1
I (602275) BRAIN: Key event: 2, current_page: 7
I (602275) battery_process: Battery adc=2547 voltage=4104mV percent=89 level=4
I (602705) BRAIN: Key event: 2, current_page: 7
I (602705) battery_process: Battery adc=2115 voltage=3408mV percent=12 level=1
I (603896) BRAIN: Key event: 2, current_page: 7
I (603896) battery_process: Battery adc=2279 voltage=3672mV percent=41 level=2
I (604517) BRAIN: Key event: 2, current_page: 7
I (604517) battery_process: Battery adc=2263 voltage=3646mV percent=38 level=2
I (604948) BRAIN: Key event: 2, current_page: 7
I (604948) battery_process: Battery adc=2343 voltage=3776mV percent=52 level=2
I (605389) BRAIN: Key event: 2, current_page: 7
I (605389) battery_process: Battery adc=2406 voltage=3876mV percent=64 level=3
I (607590) BRAIN: Key event: 3, current_page: 7
I (607591) battery_process: Battery adc=2318 voltage=3734mV percent=48 level=2
I (609242) BRAIN: Key event: 2, current_page: 7
I (609242) BRAIN: Setting index 6 updated to: OFF
I (609249) BRAIN: Settings saved
I (609253) battery_process: Battery adc=2478 voltage=3992mV percent=76 level=3
I (610794) BRAIN: Key event: 4, current_page: 7
I (610795) battery_process: Battery adc=2250 voltage=3626mV percent=36 level=2
I (611506) BRAIN: Key event: 4, current_page: 7
I (611506) battery_process: Battery adc=2352 voltage=3790mV percent=54 level=2
W (611520) BRAIN: STATE current=2 loaded=7 splash_done=1
W (611520) BRAIN: TRANSITION requested: 7 -> 2
I (611533) BRAIN: Prepared page 2
I (611534) BRAIN: Loaded page 2
I (611539) BRAIN: Destroyed PAGE_SETTING
W (611622) BRAIN: STATE current=2 loaded=2 splash_done=1
I (618317) BRAIN: Key event: 3, current_page: 2
I (618317) battery_process: Battery adc=2155 voltage=3472mV percent=19 level=1
W (618343) BRAIN: STATE current=7 loaded=2 splash_done=1
W (618343) BRAIN: TRANSITION requested: 2 -> 7
I (618471) BRAIN: Prepared page 7
I (618472) BRAIN: Loaded page 7
I (618473) BRAIN: Destroyed PAGE_MAIN_MENU
W (618584) BRAIN: STATE current=7 loaded=7 splash_done=1
I (620297) BRAIN: Key event: 2, current_page: 7
I (620297) battery_process: Battery adc=2440 voltage=3932mV percent=70 level=3
I (621377) BRAIN: Key event: 1, current_page: 7
I (621377) battery_process: Battery adc=2297 voltage=3702mV percent=44 level=2
I (622818) BRAIN: Key event: 3, current_page: 7
I (622819) battery_process: Battery adc=2293 voltage=3694mV percent=43 level=2
I (624100) BRAIN: Key event: 1, current_page: 7
I (624100) BRAIN: Setting index 6 updated to: ON
I (624106) BRAIN: Settings saved
I (624109) battery_process: Battery adc=2136 voltage=3442mV percent=15 level=1
I (625270) BRAIN: Key event: 4, current_page: 7
I (625271) battery_process: Battery adc=2299 voltage=3704mV percent=44 level=2
I (625462) BRAIN: Key event: 4, current_page: 7
I (625462) battery_process: Battery adc=2084 voltage=3358mV percent=6 level=0
I (627443) BRAIN: Key event: 4, current_page: 7
I (627443) battery_process: Battery adc=2534 voltage=4084mV percent=87 level=4
W (627460) BRAIN: STATE current=2 loaded=7 splash_done=1
W (627460) BRAIN: TRANSITION requested: 7 -> 2
I (627473) BRAIN: Prepared page 2
I (627474) BRAIN: Loaded page 2
I (627479) BRAIN: Destroyed PAGE_SETTING
W (627562) BRAIN: STATE current=2 loaded=2 splash_done=1
I (628684) BRAIN: Key event: 4, current_page: 2
I (628684) battery_process: Battery adc=2259 voltage=3640mV percent=37 level=2
I (629354) BRAIN: Key event: 4, current_page: 2
I (629354) battery_process: Battery adc=2595 voltage=4182mV percent=98 level=4
I (629584) BRAIN: Key event: 4, current_page: 2
I (629584) battery_process: Battery adc=2387 voltage=3846mV percent=60 level=2
I (629784) BRAIN: Key event: 4, current_page: 2
I (629784) battery_process: Battery adc=2430 voltage=3916mV percent=68 level=3
I (630074) BRAIN: Key event: 4, current_page: 2
I (630074) battery_process: Battery adc=2436 voltage=3926mV percent=69 level=3
I (632204) BRAIN: Key event: 5, current_page: 2
I (632204) battery_process: Battery adc=2599 voltage=4188mV percent=98 level=4
I (633134) BRAIN: Key event: 4, current_page: 2
I (633134) battery_process: Battery adc=2451 voltage=3950mV percent=72 level=3
I (633384) BRAIN: Key event: 4, current_page: 2
I (633384) battery_process: Battery adc=2376 voltage=3828mV percent=58 level=2
I (633414) BRAIN: Key event: 5, current_page: 2
I (633414) battery_process: Battery adc=2518 voltage=4058mV percent=84 level=3
I (633814) BRAIN: Key event: 5, current_page: 2
I (633814) battery_process: Battery adc=2314 voltage=3728mV percent=47 level=2
I (634154) BRAIN: Key event: 4, current_page: 2
I (634154) battery_process: Battery adc=2397 voltage=3862mV percent=62 level=3
I (634794) BRAIN: Key event: 5, current_page: 2
I (634794) battery_process: Battery adc=2351 voltage=3788mV percent=54 level=2
I (635084) BRAIN: Key event: 4, current_page: 2
I (635084) battery_process: Battery adc=2602 voltage=4192mV percent=99 level=4
I (635314) BRAIN: Key event: 5, current_page: 2
I (635314) battery_process: Battery adc=2247 voltage=3620mV percent=35 level=1
I (635624) BRAIN: Key event: 4, current_page: 2
I (635624) battery_process: Battery adc=2151 voltage=3466mV percent=18 level=1
I (635634) BRAIN: Key event: 5, current_page: 2
I (635634) battery_process: Battery adc=2477 voltage=3992mV percent=76 level=3
I (637405) BRAIN: Key event: 3, current_page: 2
I (637405) battery_process: Battery adc=2389 voltage=3850mV percent=61 level=3
W (637433) BRAIN: STATE current=7 loaded=2 splash_done=1
W (637433) BRAIN: TRANSITION requested: 2 -> 7
I (637561) BRAIN: Prepared page 7
I (637562) BRAIN: Loaded page 7
I (637563) BRAIN: Destroyed PAGE_MAIN_MENU
I (637595) BRAIN: Key event: 3, current_page: 7
I (637596) battery_process: Battery adc=2451 voltage=3950mV percent=72 level=3
W (637666) BRAIN: STATE current=7 loaded=7 splash_done=1
I (637807) BRAIN: Key event: 3, current_page: 7
I (637807) battery_process: Battery adc=2130 voltage=3432mV percent=14 level=1
I (640348) BRAIN: Key event: 4, current_page: 7
I (640349) battery_process: Battery adc=2308 voltage=3718mV percent=46 level=2
I (640590) BRAIN: Key event: 4, current_page: 7
I (640590) battery_process: Battery adc=2197 voltage=3540mV percent=26 level=1
I (640721) BRAIN: Key event: 5, current_page: 7
I (640721) battery_process: Battery adc=2163 voltage=3486mV percent=20 level=1
I (641102) BRAIN: Key event: 5, current_page: 7
I (641102) battery_process: Battery adc=2243 voltage=3614mV percent=34 level=1
I (641482) BRAIN: Key event: 5, current_page: 7
I (641482) battery_process: Battery adc=2318 voltage=3734mV percent=48 level=2
I (641542) BRAIN: Key event: 4, current_page: 7
I (641542) battery_process: Battery adc=2536 voltage=4086mV percent=87 level=4
W (641562) BRAIN: STATE current=2 loaded=7 splash_done=1
W (641562) BRAIN: TRANSITION requested: 7 -> 2
I (641575) BRAIN: Prepared page 2
I (641576) BRAIN: Loaded page 2
I (641581) BRAIN: Destroyed PAGE_SETTING
W (641664) BRAIN: STATE current=2 loaded=2 splash_done=1