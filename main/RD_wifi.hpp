#pragma once
#include <string>
#include "esp_err.h"
//#include <sys/param.h>
 #include <esp_http_server.h>
// #ifdef __cplusplus // chỉ thêm vào hàm thư viện c muốn dùng ở c++
// extern "C" {
// #endif
typedef const char *esp_event_base_t; // forward declaration -  
// typedef esp_err_t;
void rd_wifi_init(void);

#define MAX_RETRY 10
class Wifi {
public:
    Wifi();
    void begin();
    void initAP(const std::string& ssid_AP, const std::string& pass_AP);
    void init_sta(const std::string& ssid_sta, const std::string& pass_sta);
    void init_ap_and_sta(const std::string& ssid_AP, const std::string& pass_AP,
                         const std::string& ssid_sta, const std::string& pass_sta);
private:
    static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);  
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;  
    static int retry_count;
};


class HttpServer {

public:
    HttpServer();
    void begin();
    void stop();
private:
    static esp_err_t hello_get_handler(httpd_req_t *req);
    static esp_err_t button_post_handler(httpd_req_t *req);
    static esp_err_t submit_post_handler(httpd_req_t *req);
    httpd_handle_t  server;
    
};