#include "nvs_manager.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"

static const char *TAG = "NVS_MANAGER";
static bool is_initialized = false;

esp_err_t nvs_manager_init(void) {
    esp_err_t ret = ESP_OK;
    if (!is_initialized) {
        ret = nvs_flash_init();
        if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
            ESP_ERROR_CHECK(nvs_flash_erase());
            ret = nvs_flash_init();
        }
        is_initialized = true;
    }
    return ret;
}

esp_err_t nvs_manager_save_str(const char *key, const char *value) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open("nvs", NVS_READWRITE, &handle);
    if (err != ESP_OK) return err;

    err = nvs_set_str(handle, key, value);
    if (err == ESP_OK) err = nvs_commit(handle);
    nvs_close(handle);
    ESP_LOGI(TAG, "Saved key='%s', value='%s'", key, value);
    return err;
}

esp_err_t nvs_manager_read_str(const char *key, char *out_value, size_t len) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open("nvs", NVS_READONLY, &handle);
    if (err != ESP_OK) return err;

    err = nvs_get_str(handle, key, out_value, &len);
    nvs_close(handle);
    return err;
}

esp_err_t nvs_manager_save_double(const char *key, double value) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open("nvs", NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE("NVS", "Error opening NVS for write: %s", esp_err_to_name(err));
        return err;
    }

    err = nvs_set_blob(handle, key, &value, sizeof(double));
    if (err == ESP_OK) {
        err = nvs_commit(handle);
    }

    nvs_close(handle);

    if (err == ESP_OK)
        ESP_LOGI("NVS", "Saved double key='%s' value=%lf", key, value);
    else
        ESP_LOGE("NVS", "Failed to save key='%s': %s", key, esp_err_to_name(err));

    return err;
}

esp_err_t nvs_manager_read_double(const char *key, double *out_value) {
    if (!out_value) {
        return ESP_ERR_INVALID_ARG;
    }

    nvs_handle_t handle;
    esp_err_t err = nvs_open("nvs", NVS_READONLY, &handle);
    if (err != ESP_OK) {
        return err;
    }

    size_t size = sizeof(double);
    err = nvs_get_blob(handle, key, out_value, &size);
    nvs_close(handle);

    if (err == ESP_OK) {
        ESP_LOGI("NVS", "Loaded double key='%s' value=%lf", key, *out_value);
    }

    return err;
}

esp_err_t nvs_manager_erase_all(void) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open("nvs", NVS_READWRITE, &handle);
    if (err == ESP_OK) {
        err = nvs_erase_all(handle);
        nvs_commit(handle);
        nvs_close(handle);
    }
    return err;
}
