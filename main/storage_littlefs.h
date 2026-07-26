#ifndef STORAGE_LITTLEFS_H
#define STORAGE_LITTLEFS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define STORAGE_LFS_BASE_PATH          "/littlefs"
#define STORAGE_LFS_PARTITION_LABEL    "storage"

#define STORAGE_SCAN_FILE_PREFIX       "scan_"
#define STORAGE_SCAN_FILE_EXT          ".bin"
#define STORAGE_SCAN_INDEX_FILE        STORAGE_LFS_BASE_PATH "/scan_index.dat"
#define STORAGE_SCAN_TEMP_FILE         STORAGE_LFS_BASE_PATH "/scan_index.tmp"

#define STORAGE_SCAN_INDEX_MAGIC       0x58444E49UL /* "INDX" */
#define STORAGE_SCAN_RECORD_MAGIC      0x4E414353UL /* "SCAN" */
#define STORAGE_SCAN_VERSION           1U

typedef struct {
    uint32_t id;
    uint32_t mode;
    uint32_t point_count;
    uint32_t timestamp_sec;
} scan_index_item_t;

typedef struct {
    uint32_t magic;
    uint16_t version;
    uint16_t reserved;
    uint32_t id;
    uint32_t mode;
    uint32_t point_count;
    uint32_t timestamp_sec;
} scan_record_header_t;

typedef struct {
    uint32_t magic;
    uint16_t version;
    uint16_t reserved;
    uint32_t count;
    uint32_t next_id;
} scan_index_header_t;

typedef struct {
    uint32_t mode;
    uint32_t timestamp_sec;
    const int16_t *samples;
    size_t sample_count;
} storage_scan_record_t;

esp_err_t storage_littlefs_init(void);
esp_err_t storage_littlefs_deinit(void);
bool storage_littlefs_is_ready(void);

esp_err_t storage_littlefs_save_scan(const storage_scan_record_t *record,
                                     uint32_t *out_scan_id);

esp_err_t storage_littlefs_load_index(scan_index_item_t *items,
                                      size_t max_items,
                                      size_t *out_count);

esp_err_t storage_littlefs_load_scan_header(uint32_t scan_id,
                                            scan_record_header_t *out_header);

esp_err_t storage_littlefs_load_scan_samples(uint32_t scan_id,
                                             int16_t *out_samples,
                                             size_t max_samples,
                                             size_t *out_count);

esp_err_t storage_littlefs_delete_scan(uint32_t scan_id);

#ifdef __cplusplus
}
#endif

#endif /* STORAGE_LITTLEFS_H */
