#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <string>
//  namespace std;
class Wifi {
public:
    Wifi(){

    }
    void begin();
    void initAP(const std::string ssid);
private:
    static void envent_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);    
}
void rd_wifi_init(void);

#ifdef __cplusplus
}
#endif