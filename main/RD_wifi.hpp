#pragma once
#include <string>
// #ifdef __cplusplus // chỉ thêm vào hàm thư viện c muốn dùng ở c++
// extern "C" {
// #endif
typedef const char *esp_event_base_t; // forward declaration -  
void rd_wifi_init(void);

class Wifi {
public:
    Wifi();
    void begin();
    void initAP(const std::string& ssid_AP, const std::string& pass_AP);
private:
    static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);    
};
