#pragma once
#include <string>
#include "esp_err.h"
//#include <sys/param.h>
 #include <esp_http_server.h>
 #include <mqtt_client.h>
// #ifdef __cplusplus // chỉ thêm khi muons build ở c
// extern "C" {
// #endif
typedef const char *esp_event_base_t; // forward declaration -  
// typedef esp_err_t;
void rd_wifi_init(void);

class Wifi {
public:
    Wifi();
    void begin();
    void initAP(const std::string& ssid_AP, const std::string& pass_AP);
    void initSTA(const std::string& ssid_STA, const std::string& pass_STA);
private:
    static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);    
    static void ip_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);    
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

class Mqtt
{
private:
    /* data */
    static void mqtt_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
    esp_mqtt_client_handle_t client;
    std::string broker_uri;
public:
    Mqtt(/* args */);
    void begin();
    void connect();
    void disconnect();
    void publish();
    void subscribe();

    ~Mqtt();
};

Mqtt::Mqtt(/* args */)
{
}

Mqtt::~Mqtt()
{
}



