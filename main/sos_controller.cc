#include "sos_controller.h"
#include <esp_log.h>
#include <esp_http_client.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <algorithm>
#include <vector>

#include <esp_crt_bundle.h>

#define TAG "SosController"

void SosController::Configure(const std::string& bot_token, const std::string& chat_id) {
    bot_token_ = bot_token;
    chat_id_ = chat_id;
}

void SosController::SendTelegramMessage(const std::string& message) {
    if (bot_token_.empty() || chat_id_.empty() || bot_token_ == "YOUR_TELEGRAM_BOT_TOKEN") {
        ESP_LOGW(TAG, "Telegram Bot Token or Chat ID not configured (message: %s)", message.c_str());
        return;
    }

    char url[512];
    snprintf(url, sizeof(url),
             "https://api.telegram.org/bot%s/sendMessage?chat_id=%s&text=%s",
             bot_token_.c_str(), chat_id_.c_str(), message.c_str());

    esp_http_client_config_t config = {};
    config.url = url;
    config.method = HTTP_METHOD_GET;
    config.timeout_ms = 8000;
    config.crt_bundle_attach = esp_crt_bundle_attach;

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (client == nullptr) {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        return;
    }

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Telegram alert sent! Status code: %d",
                 esp_http_client_get_status_code(client));
    } else {
        ESP_LOGE(TAG, "Failed to send Telegram alert: %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
}

void SosController::TriggerAlert(const std::string& reason) {
    ESP_LOGW(TAG, "Triggering SOS alert: %s", reason.c_str());
    std::string alert_text = "EMERGENCY_ALERT_FOR_ELDERLY:%20" + reason;
    std::replace(alert_text.begin(), alert_text.end(), ' ', '+');

    xTaskCreate([](void* arg) {
        std::string* msg = static_cast<std::string*>(arg);
        SosController::GetInstance().SendTelegramMessage(*msg);
        delete msg;
        vTaskDelete(NULL);
    }, "sos_tg_task", 4096, new std::string(alert_text), 5, NULL);

    ESP_LOGW(TAG, "SOS ALERT SENT!");
}

bool SosController::CheckVoiceEmergency(const std::string& text) {
    static const std::vector<std::string> emergency_keywords = {
        "help", "emergency", "fell down", "fall down", "heart attack",
        "save me", "pain", "doctor", "ambulance", "bachao", "madad"
    };

    std::string lower_text = text;
    std::transform(lower_text.begin(), lower_text.end(), lower_text.begin(), ::tolower);

    for (const auto& keyword : emergency_keywords) {
        if (lower_text.find(keyword) != std::string::npos) {
            TriggerAlert("Voice_Emergency_Keyword_Detected:_" + keyword);
            return true;
        }
    }
    return false;
}
