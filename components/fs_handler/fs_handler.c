#include "fs_handler.h"
#include "esp_log.h"
#include "esp_vfs.h"
#include "esp_littlefs.h"
#include <stdio.h>
#include <sys/unistd.h>
#include <sys/stat.h>

static const char *TAG = "FS_HANDLER";
static bool fs_mounted = false;

/* Partition name (must be equal in partitions.csv) */
#define LITTLEFS_PARTITION_NAME "storage"

/* =======================================================================
 * Initialize the LittleFS
 * ======================================================================= */
esp_err_t fs_handler_init(void)
{
    if (fs_mounted) {
        ESP_LOGI(TAG, "LittleFS já montado");
        return ESP_OK;
    }

    esp_vfs_littlefs_conf_t conf = {
        .base_path = "/littlefs",
        .partition_label = LITTLEFS_PARTITION_NAME,
        .format_if_mount_failed = true,   // format if fail
        .dont_mount = false,
    };

    esp_err_t ret = esp_vfs_littlefs_register(&conf);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Erro ao montar LittleFS (%s)", esp_err_to_name(ret));
        return ret;
    }

    size_t total = 0, used = 0;
    ret = esp_littlefs_info(LITTLEFS_PARTITION_NAME, &total, &used);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Erro ao obter info do FS (%s)", esp_err_to_name(ret));
        return ret;
    }

    fs_mounted = true;

    ESP_LOGI(TAG, "LittleFS is mount on /littlefs");
    ESP_LOGI(TAG, "Total: %u bytes, Used: %u bytes", total, used);

    return ESP_OK;
}


/* =======================================================================
 * Read a file
 * ======================================================================= */
esp_err_t fs_handler_read_file(const char *path, char **out_buf, size_t *out_len)
{
    if (!fs_mounted) return ESP_FAIL;

    char full_path[128];
    snprintf(full_path, sizeof(full_path), "/littlefs/%s", path);

    FILE *f = fopen(full_path, "rb");
    if (!f) {
        ESP_LOGW(TAG, "File not found: %s", full_path);
        return ESP_ERR_NOT_FOUND;
    }

    fseek(f, 0, SEEK_END);
    size_t len = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buf = malloc(len + 1);
    if (!buf) {
        fclose(f);
        return ESP_ERR_NO_MEM;
    }

    fread(buf, 1, len, f);
    fclose(f);

    buf[len] = '\0';  // Ensure valid string

    *out_buf = buf;
    *out_len = len;

    ESP_LOGI(TAG, "File '%s' read (%u bytes)", full_path, len);

    return ESP_OK;
}


/* =======================================================================
 * Write a file
 * ======================================================================= */
esp_err_t fs_handler_write_file(const char *path, const char *data, size_t len)
{
    if (!fs_mounted) return ESP_FAIL;

    char full_path[128];
    snprintf(full_path, sizeof(full_path), "/littlefs/%s", path);

    FILE *f = fopen(full_path, "wb");
    if (!f) {
        ESP_LOGE(TAG, "Fail to open '%s' to write", full_path);
        return ESP_FAIL;
    }

    fwrite(data, 1, len, f);
    fclose(f);

    ESP_LOGI(TAG, "File '%s' saved (%u bytes)", full_path, len);

    return ESP_OK;
}


/* =======================================================================
 * Delete a file
 * ======================================================================= */
esp_err_t fs_handler_delete_file(const char *path)
{
    if (!fs_mounted) return ESP_FAIL;

    char full_path[128];
    snprintf(full_path, sizeof(full_path), "/littlefs/%s", path);

    if (unlink(full_path) == 0) {
        ESP_LOGI(TAG, "File deleted: %s", full_path);
        return ESP_OK;
    }

    ESP_LOGW(TAG, "Error when delete file: %s", full_path);
    return ESP_FAIL;
}


/* =======================================================================
 * Verify if file exists
 * ======================================================================= */
bool fs_handler_exists(const char *path)
{
    if (!fs_mounted) return false;

    char full_path[128];
    snprintf(full_path, sizeof(full_path), "/littlefs/%s", path);

    struct stat st;
    return (stat(full_path, &st) == 0);
}


/* =======================================================================
 * List all files in root
 * ======================================================================= */
void fs_handler_list_files(void)
{
    if (!fs_mounted) return;

    DIR *dir = opendir("/littlefs");
    if (!dir) {
        ESP_LOGE(TAG, "Error when open folder");
        return;
    }

    struct dirent *entry;
    ESP_LOGI(TAG, "Files in /littlefs:");
    while ((entry = readdir(dir)) != NULL) {
        ESP_LOGI(TAG, " - %s", entry->d_name);
    }

    closedir(dir);
}
