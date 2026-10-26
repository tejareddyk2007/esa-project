#ifndef _SOS_CONTROLLER_H_
#define _SOS_CONTROLLER_H_

#include <string>

class SosController {
public:
    static SosController& GetInstance() {
        static SosController instance;
        return instance;
    }

    void Configure(const std::string& bot_token, const std::string& chat_id);
    void TriggerAlert(const std::string& reason);
    bool CheckVoiceEmergency(const std::string& text);

private:
    SosController() = default;
    void SendTelegramMessage(const std::string& message);

    std::string bot_token_ = "8860203910:AAE0PgoKdzmcHJ5xHJj32VKbE-IM3YYSdFE";
    std::string chat_id_   = "6125494215";
};

#endif // _SOS_CONTROLLER_H_
