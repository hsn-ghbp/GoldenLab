#include "storage_littlefs.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "esp_check.h"
#include "esp_littlefs.h"
#include "esp_log.h"

#define STORAGE_SCAN_FILE_NAME_MAX 64

static const char *TAG = "storage_lfs";

static bool s_littlefs_ready = false;

static esp_err_t storage_write_index_file(const scan_index_header_t *header,
                                          const scan_index_item_t *items);
static esp_err_t storage_read_index_file(scan_index_header_t *header,
                                         scan_index_item_t **out_items);
static esp_err_t storage_build_scan_path(uint32_t scan_id,
                                         char *path,
                                         size_t path_len);
static esp_err_t storage_write_scan_file(uint32_t scan_id,
                                         const storage_scan_record_t *record);
static bool storage_file_exists(const char *path);

esp_err_t storage_littlefs_init(void)
{
    if (s_littlefs_ready) {
        return ESP_OK;
    }

    const esp_vfs_littlefs_conf_t conf = {
        .base_path = STORAGE_LFS_BASE_PATH,
        .partition_label = STORAGE_LFS_PARTITION_LABEL,
        .format_if_mount_failed = true,
        .dont_mount = false,
    };

    esp_err_t err = esp_vfs_littlefs_register(&conf);
    ESP_RETURN_ON_ERROR(err, TAG, "LittleFS mount failed");

    s_littlefs_ready = true;

    scan_index_header_t header = {0};
    scan_index_item_t *items = NULL;

    err = storage_read_index_file(&header, &items);
    if (err == ESP_ERR_NOT_FOUND) {
        header.magic = STORAGE_SCAN_INDEX_MAGIC;
        header.version = STORAGE_SCAN_VERSION;
        header.reserved = 0;
        header.count = 0;
        header.next_id = 1;

        err = storage_write_index_file(&header, NULL);
        ESP_RETURN_ON_ERROR(err, TAG, "failed to create index file");
    } else {
        ESP_RETURN_ON_ERROR(err, TAG, "failed to validate index file");
    }

    free(items);
    return ESP_OK;
}

esp_err_t storage_littlefs_deinit(void)
{
    if (!s_littlefs_ready) {
        return ESP_OK;
    }

    esp_err_t err = esp_vfs_littlefs_unregister(STORAGE_LFS_PARTITION_LABEL);
    if (err == ESP_OK) {
        s_littlefs_ready = false;
    }

    return err;
}

bool storage_littlefs_is_ready(void)
{
    return s_littlefs_ready;
}

esp_err_t storage_littlefs_save_scan(const storage_scan_record_t *record,
                                     uint32_t *out_scan_id)
{
    scan_index_header_t header = {0};
    scan_index_item_t *items = NULL;
    scan_index_item_t *new_items = NULL;
    scan_index_item_t new_item = {0};
    uint32_t new_id = 0;
    esp_err_t err;

    ESP_RETURN_ON_FALSE(s_littlefs_ready, ESP_ERR_INVALID_STATE, TAG, "LittleFS not ready");
    ESP_RETURN_ON_FALSE(record != NULL, ESP_ERR_INVALID_ARG, TAG, "record is NULL");
    ESP_RETURN_ON_FALSE(record->samples != NULL, ESP_ERR_INVALID_ARG, TAG, "samples is NULL");
    ESP_RETURN_ON_FALSE(record->sample_count > 0, ESP_ERR_INVALID_ARG, TAG, "sample_count is zero");
    ESP_RETURN_ON_FALSE(record->sample_count <= UINT32_MAX, ESP_ERR_INVALID_ARG, TAG, "sample_count too large");

    err = storage_read_index_file(&header, &items);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to read index file");

    new_id = header.next_id;

    err = storage_write_scan_file(new_id, record);
    if (err != ESP_OK) {
        free(items);
        return err;
    }

    new_items = calloc(header.count + 1U, sizeof(scan_index_item_t));
    if (new_items == NULL) {
        free(items);
        return ESP_ERR_NO_MEM;
    }

    if ((items != NULL) && (header.count > 0U)) {
        memcpy(new_items, items, header.count * sizeof(scan_index_item_t));
    }

    new_item.id = new_id;
    new_item.mode = record->mode;
    new_item.point_count = (uint32_t)record->sample_count;
    new_item.timestamp_sec = record->timestamp_sec;

    new_items[header.count] = new_item;
    header.count += 1U;
    header.next_id += 1U;

    err = storage_write_index_file(&header, new_items);

    free(items);
    free(new_items);

    ESP_RETURN_ON_ERROR(err, TAG, "failed to update index file");

    if (out_scan_id != NULL) {
        *out_scan_id = new_id;
    }

    return ESP_OK;
}

esp_err_t storage_littlefs_load_index(scan_index_item_t *items,
                                      size_t max_items,
                                      size_t *out_count)
{
    scan_index_header_t header = {0};
    scan_index_item_t *loaded_items = NULL;
    size_t copy_count = 0;
    esp_err_t err;

    ESP_RETURN_ON_FALSE(s_littlefs_ready, ESP_ERR_INVALID_STATE, TAG, "LittleFS not ready");
    ESP_RETURN_ON_FALSE(out_count != NULL, ESP_ERR_INVALID_ARG, TAG, "out_count is NULL");

    err = storage_read_index_file(&header, &loaded_items);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to read index file");

    copy_count = header.count;
    if (copy_count > max_items) {
        copy_count = max_items;
    }

    if ((items != NULL) && (copy_count > 0U)) {
        memcpy(items, loaded_items, copy_count * sizeof(scan_index_item_t));
    }

    *out_count = copy_count;
    free(loaded_items);
    return ESP_OK;
}

esp_err_t storage_littlefs_load_scan_header(uint32_t scan_id,
                                            scan_record_header_t *out_header)
{
    char path[STORAGE_SCAN_FILE_NAME_MAX];
    FILE *fp = NULL;
    size_t nread;

    ESP_RETURN_ON_FALSE(s_littlefs_ready, ESP_ERR_INVALID_STATE, TAG, "LittleFS not ready");
    ESP_RETURN_ON_FALSE(out_header != NULL, ESP_ERR_INVALID_ARG, TAG, "out_header is NULL");

    ESP_RETURN_ON_ERROR(storage_build_scan_path(scan_id, path, sizeof(path)),
                        TAG, "invalid scan path");

    fp = fopen(path, "rb");
    if (fp == NULL) {
        return ESP_ERR_NOT_FOUND;
    }

    nread = fread(out_header, 1, sizeof(*out_header), fp);
    fclose(fp);

    if (nread != sizeof(*out_header)) {
        return ESP_ERR_INVALID_SIZE;
    }

    if ((out_header->magic != STORAGE_SCAN_RECORD_MAGIC) ||
        (out_header->version != STORAGE_SCAN_VERSION)) {
        return ESP_ERR_INVALID_RESPONSE;
    }

    return ESP_OK;
}

esp_err_t storage_littlefs_load_scan_samples(uint32_t scan_id,
                                             int16_t *out_samples,
                                             size_t max_samples,
                                             size_t *out_count)
{
    char path[STORAGE_SCAN_FILE_NAME_MAX];
    FILE *fp = NULL;
    scan_record_header_t header = {0};
    size_t sample_bytes;
    size_t sample_count;
    size_t nread;

    ESP_RETURN_ON_FALSE(s_littlefs_ready, ESP_ERR_INVALID_STATE, TAG, "LittleFS not ready");
    ESP_RETURN_ON_FALSE(out_count != NULL, ESP_ERR_INVALID_ARG, TAG, "out_count is NULL");

    ESP_RETURN_ON_ERROR(storage_build_scan_path(scan_id, path, sizeof(path)),
                        TAG, "invalid scan path");

    fp = fopen(path, "rb");
    if (fp == NULL) {
        return ESP_ERR_NOT_FOUND;
    }

    nread = fread(&header, 1, sizeof(header), fp);
    if (nread != sizeof(header)) {
        fclose(fp);
        return ESP_ERR_INVALID_SIZE;
    }

    if ((header.magic != STORAGE_SCAN_RECORD_MAGIC) ||
        (header.version != STORAGE_SCAN_VERSION)) {
        fclose(fp);
        return ESP_ERR_INVALID_RESPONSE;
    }

    sample_count = header.point_count;
    if (sample_count > max_samples) {
        fclose(fp);
        return ESP_ERR_INVALID_SIZE;
    }

    sample_bytes = sample_count * sizeof(int16_t);
    if ((sample_count > 0U) && (out_samples != NULL)) {
        nread = fread(out_samples, 1, sample_bytes, fp);
        if (nread != sample_bytes) {
            fclose(fp);
            return ESP_ERR_INVALID_SIZE;
        }
    }

    *out_count = sample_count;
    fclose(fp);
    return ESP_OK;
}

esp_err_t storage_littlefs_delete_scan(uint32_t scan_id)
{
    scan_index_header_t header = {0};
    scan_index_item_t *items = NULL;
    scan_index_item_t *new_items = NULL;
    char path[STORAGE_SCAN_FILE_NAME_MAX];
    size_t i;
    size_t keep_count = 0;
    bool found = false;
    esp_err_t err;

    ESP_RETURN_ON_FALSE(s_littlefs_ready, ESP_ERR_INVALID_STATE, TAG, "LittleFS not ready");

    err = storage_read_index_file(&header, &items);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to read index file");

    for (i = 0; i < header.count; ++i) {
        if (items[i].id == scan_id) {
            found = true;
        } else {
            keep_count++;
        }
    }

    if (!found) {
        free(items);
        return ESP_ERR_NOT_FOUND;
    }

    if (keep_count > 0U) {
        new_items = calloc(keep_count, sizeof(scan_index_item_t));
        if (new_items == NULL) {
            free(items);
            return ESP_ERR_NO_MEM;
        }

        keep_count = 0;
        for (i = 0; i < header.count; ++i) {
            if (items[i].id != scan_id) {
                new_items[keep_count++] = items[i];
            }
        }
    }

    header.count = (uint32_t)keep_count;

    err = storage_write_index_file(&header, new_items);
    if (err != ESP_OK) {
        free(items);
        free(new_items);
        return err;
    }

    err = storage_build_scan_path(scan_id, path, sizeof(path));
    if (err != ESP_OK) {
        free(items);
        free(new_items);
        return err;
    }

    if (remove(path) != 0) {
        free(items);
        free(new_items);
        return ESP_FAIL;
    }

    free(items);
    free(new_items);
    return ESP_OK;
}

static esp_err_t storage_write_index_file(const scan_index_header_t *header,
                                          const scan_index_item_t *items)
{
    FILE *fp = NULL;
    size_t nwritten;
    size_t item_bytes = 0;

    ESP_RETURN_ON_FALSE(header != NULL, ESP_ERR_INVALID_ARG, TAG, "header is NULL");

    fp = fopen(STORAGE_SCAN_TEMP_FILE, "wb");
    if (fp == NULL) {
        return ESP_FAIL;
    }

    nwritten = fwrite(header, 1, sizeof(*header), fp);
    if (nwritten != sizeof(*header)) {
        fclose(fp);
        remove(STORAGE_SCAN_TEMP_FILE);
        return ESP_ERR_INVALID_SIZE;
    }

    item_bytes = header->count * sizeof(scan_index_item_t);
    if ((item_bytes > 0U) && (items != NULL)) {
        nwritten = fwrite(items, 1, item_bytes, fp);
        if (nwritten != item_bytes) {
            fclose(fp);
            remove(STORAGE_SCAN_TEMP_FILE);
            return ESP_ERR_INVALID_SIZE;
        }
    }

    fclose(fp);

    if (remove(STORAGE_SCAN_INDEX_FILE) != 0) {
        if (storage_file_exists(STORAGE_SCAN_INDEX_FILE)) {
            remove(STORAGE_SCAN_TEMP_FILE);
            return ESP_FAIL;
        }
    }

    if (rename(STORAGE_SCAN_TEMP_FILE, STORAGE_SCAN_INDEX_FILE) != 0) {
        remove(STORAGE_SCAN_TEMP_FILE);
        return ESP_FAIL;
    }

    return ESP_OK;
}

static esp_err_t storage_read_index_file(scan_index_header_t *header,
                                         scan_index_item_t **out_items)
{
    FILE *fp = NULL;
    scan_index_item_t *items = NULL;
    size_t item_bytes = 0;
    size_t nread;

    ESP_RETURN_ON_FALSE(header != NULL, ESP_ERR_INVALID_ARG, TAG, "header is NULL");
    ESP_RETURN_ON_FALSE(out_items != NULL, ESP_ERR_INVALID_ARG, TAG, "out_items is NULL");

    *out_items = NULL;

    fp = fopen(STORAGE_SCAN_INDEX_FILE, "rb");
    if (fp == NULL) {
        return ESP_ERR_NOT_FOUND;
    }

    nread = fread(header, 1, sizeof(*header), fp);
    if (nread != sizeof(*header)) {
        fclose(fp);
        return ESP_ERR_INVALID_SIZE;
    }

    if ((header->magic != STORAGE_SCAN_INDEX_MAGIC) ||
        (header->version != STORAGE_SCAN_VERSION)) {
        fclose(fp);
        return ESP_ERR_INVALID_RESPONSE;
    }

    item_bytes = header->count * sizeof(scan_index_item_t);
    if (item_bytes > 0U) {
        items = calloc(header->count, sizeof(scan_index_item_t));
        if (items == NULL) {
            fclose(fp);
            return ESP_ERR_NO_MEM;
        }

        nread = fread(items, 1, item_bytes, fp);
        if (nread != item_bytes) {
            free(items);
            fclose(fp);
            return ESP_ERR_INVALID_SIZE;
        }
    }

    fclose(fp);
    *out_items = items;
    return ESP_OK;
}

static esp_err_t storage_build_scan_path(uint32_t scan_id,
                                         char *path,
                                         size_t path_len)
{
    int written;

    ESP_RETURN_ON_FALSE(path != NULL, ESP_ERR_INVALID_ARG, TAG, "path is NULL");

    written = snprintf(path,
                       path_len,
                       STORAGE_LFS_BASE_PATH "/%s%04lu%s",
                       STORAGE_SCAN_FILE_PREFIX,
                       (unsigned long)scan_id,
                       STORAGE_SCAN_FILE_EXT);

    if ((written < 0) || ((size_t)written >= path_len)) {
        return ESP_ERR_INVALID_SIZE;
    }

    return ESP_OK;
}

static esp_err_t storage_write_scan_file(uint32_t scan_id,
                                         const storage_scan_record_t *record)
{
    char path[STORAGE_SCAN_FILE_NAME_MAX];
    char temp_path[STORAGE_SCAN_FILE_NAME_MAX];
    FILE *fp = NULL;
    scan_record_header_t header;
    size_t header_written;
    size_t sample_bytes;
    size_t sample_written;
    int written;

    ESP_RETURN_ON_FALSE(record != NULL, ESP_ERR_INVALID_ARG, TAG, "record is NULL");

    ESP_RETURN_ON_ERROR(storage_build_scan_path(scan_id, path, sizeof(path)),
                        TAG, "invalid scan path");

    written = snprintf(temp_path, sizeof(temp_path), "%s.tmp", path);
    if ((written < 0) || ((size_t)written >= sizeof(temp_path))) {
        return ESP_ERR_INVALID_SIZE;
    }

    header.magic = STORAGE_SCAN_RECORD_MAGIC;
    header.version = STORAGE_SCAN_VERSION;
    header.reserved = 0;
    header.id = scan_id;
    header.mode = record->mode;
    header.point_count = (uint32_t)record->sample_count;
    header.timestamp_sec = record->timestamp_sec;

    fp = fopen(temp_path, "wb");
    if (fp == NULL) {
        return ESP_FAIL;
    }

    header_written = fwrite(&header, 1, sizeof(header), fp);
    if (header_written != sizeof(header)) {
        fclose(fp);
        remove(temp_path);
        return ESP_ERR_INVALID_SIZE;
    }

    sample_bytes = record->sample_count * sizeof(int16_t);
    sample_written = fwrite(record->samples, 1, sample_bytes, fp);
    if (sample_written != sample_bytes) {
        fclose(fp);
        remove(temp_path);
        return ESP_ERR_INVALID_SIZE;
    }

    fclose(fp);

    if (remove(path) != 0) {
        if (storage_file_exists(path)) {
            remove(temp_path);
            return ESP_FAIL;
        }
    }

    if (rename(temp_path, path) != 0) {
        remove(temp_path);
        return ESP_FAIL;
    }

    return ESP_OK;
}

static bool storage_file_exists(const char *path)
{
    struct stat st;

    if (path == NULL) {
        return false;
    }

    return stat(path, &st) == 0;
}
