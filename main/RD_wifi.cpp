// #ifdef __cplusplus
// extern "C"
// {
// #endif
#include "RD_wifi.hpp"
#include <stdio.h>
//#include "string.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include <esp32c3/rom/ets_sys.h>
#include "nvs_flash.h"
#include <cstring>
//  #include "nvs_flash.h"
static const char *TAG = "OTA:";
// Định nghĩa biến static bên ngoài lớp
int Wifi::retry_count = 0;

//char* ssid_hao = "Hao_ESP32";
#if(0)
#   define EXAMPLE_ESP_WIFI_SSID      "Hao_ESP32"
#   define EXAMPLE_ESP_WIFI_PASS      "RDSL@2804"
#else
#   define EXAMPLE_ESP_WIFI_SSID      "P202"
#   define EXAMPLE_ESP_WIFI_PASS      "xincamon"
#endif
#define EXAMPLE_ESP_WIFI_CHANNEL   1
#define EXAMPLE_MAX_AP_CON         4

static int count_press =0;



/*
 * Serve OTA update portal (index.html)
 */
extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_index_html_end");

uint8_t Net_key[10] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};


Wifi::Wifi(){

}

void Wifi::begin(){
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
       ret = nvs_flash_init();
   }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_netif_init());                      //init TCP/IP
    ESP_LOGI(TAG, "Rang Dong wifi begin");
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    std::string key_str;
    for(int i=0;i<10; i++)
    {
       key_str = key_str + "-0x";
       key_str = key_str + std::to_string(Net_key[i]);
    }
        ESP_LOGI(TAG, "key_str %s", key_str.c_str());

}
 
void Wifi::initAP(const std::string& ssid_AP, const std::string& pass_AP){
    esp_netif_create_default_wifi_ap();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        (esp_event_handler_t)&Wifi::event_handler,    // handler even
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {};
    
    wifi_config.ap.ssid_len = ssid_AP.length();
    strncpy((char *) wifi_config.ap.ssid, ssid_AP.c_str(), sizeof(wifi_config.ap.ssid));
    strncpy((char *) wifi_config.ap.password, pass_AP.c_str(), sizeof(wifi_config.ap.password));

    wifi_config.ap.channel  = EXAMPLE_ESP_WIFI_CHANNEL;
    wifi_config.ap.authmode = WIFI_AUTH_WPA2_PSK;
    wifi_config.ap.max_connection = EXAMPLE_MAX_AP_CON;
    wifi_config.ap.ssid_hidden = 0;
    if (pass_AP.empty()) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;                               // hightLight
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
             ssid_AP.c_str(), pass_AP.c_str(), EXAMPLE_ESP_WIFI_CHANNEL);
}
void Wifi::init_sta(const std::string &ssid_sta, const std::string &pass_sta){
    esp_netif_t* netif_sta = esp_netif_create_default_wifi_sta();
    esp_netif_set_hostname(netif_sta, "Hao_device");
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        (esp_event_handler_t)&Wifi::event_handler,    // handler even
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip)); //&instance_got_ip 
    wifi_config_t wifi_config = {};
    
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    strncpy((char *) wifi_config.sta.ssid, ssid_sta.c_str(), ssid_sta.size());
    strncpy((char *) wifi_config.sta.password, pass_sta.c_str(), pass_sta.size()); 
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start()); 
    ESP_LOGI(TAG, "wifi_init_sta finished. SSID:%s password:%s ",
            ssid_sta.c_str(), pass_sta.c_str());     
                                                  
}

void Wifi:: init_ap_and_sta(const std::string& ssid_ap, const std::string& pass_ap,
                     const std::string& ssid_sta, const std::string& pass_sta){
    // Create default Wi-Fi interfaces
    esp_netif_t* netif_sta = esp_netif_create_default_wifi_sta();
    esp_netif_t* netif_ap  = esp_netif_create_default_wifi_ap();
    esp_netif_set_hostname(netif_sta, "Hao_device");  
    
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));  

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &Wifi::event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &Wifi::event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    // Configure STA
    wifi_config_t wifi_config_sta = {};
    strncpy((char*)wifi_config_sta.sta.ssid, ssid_sta.c_str(), sizeof(wifi_config_sta.sta.ssid));
    strncpy((char*)wifi_config_sta.sta.password, pass_sta.c_str(), sizeof(wifi_config_sta.sta.password));
    wifi_config_sta.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

    // Configure AP
    wifi_config_t wifi_config_ap = {};
    strncpy((char*)wifi_config_ap.ap.ssid, ssid_ap.c_str(), sizeof(wifi_config_ap.ap.ssid));
    strncpy((char*)wifi_config_ap.ap.password, pass_ap.c_str(), sizeof(wifi_config_ap.ap.password));
    wifi_config_ap.ap.ssid_len = ssid_ap.length();
    wifi_config_ap.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
    wifi_config_ap.ap.max_connection = 4;  
    wifi_config_ap.ap.beacon_interval = 100;

    // Set Wi-Fi mode to APSTA
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));


    // Set Wi-Fi configurations
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config_sta));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config_ap));


    // Start Wi-Fi
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI("wifi_apsta", "wifi_init_apsta finished.");

    }
// không cần khai báo ại từ khóa static
void Wifi::event_handler(void* arg , esp_event_base_t event_base, int32_t event_id, void* event_data){
    #if(0)
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG,"station" MACSTR "join, AID=%d",MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
       ESP_LOGI(TAG, "station " MACSTR " leave, AID=%d",
                MAC2STR(event->mac), event->aid);
    }
    #endif
    if(WIFI_EVENT == event_base){
        switch (event_id)
        {
        case WIFI_EVENT_AP_STACONNECTED:{
            wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
            ESP_LOGI(TAG,"station" MACSTR "join, AID=%d",MAC2STR(event->mac), event->aid);
            /* code */
        }
            break;
        case WIFI_EVENT_AP_STADISCONNECTED:{
            wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
            ESP_LOGI(TAG, "station " MACSTR " leave, AID=%d",
            MAC2STR(event->mac), event->aid);
        }
            break;
        case WIFI_EVENT_STA_START:
            esp_wifi_connect();
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            if (retry_count < MAX_RETRY) {
                esp_wifi_connect();
                retry_count++;
                ESP_LOGI(TAG, "Retrying to connect to the AP");
            } else {
                //xEventGroupSetBits(wifi_event_group, WIFI_FAIL_BIT);
                ESP_LOGI(TAG, "Retry max -  wifi disconect");
            }
            ESP_LOGI(TAG,"connect to the AP fail");
        default:
            ESP_LOGI(TAG,"wifi event default");
            break;
        }
    }
    if(IP_EVENT == event_base){
        switch (event_id)
        {
        case (IP_EVENT_STA_GOT_IP):{
            ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
            ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
            retry_count = 0;
        }
        break;
        default:
            ESP_LOGI(TAG, "ip event default");
            break;
        }
    }
}


static const char *TAG_HTTP  = "http_sever";

HttpServer::HttpServer(){

}

esp_err_t HttpServer::hello_get_handler(httpd_req_t *req){

       /* const char *resp_str = 
        "<!DOCTYPE html>"
        "<html>"
        "<head>"
        "<title>Hello Page</title>"
        "</head>"
        "<body>"
        "<h1>Hello</h1>"
        "<p> count: %d</p>"
        "<p> Key: %s</p>"
        "<button onclick=\"fetch('/button', {method: 'POST'})\">Press me</button>"
        "</body>"
        "</html>";
        */
    char html_buffer[1024];

    // std::string key_str;
    //     for(int i=0;i<10; i++)
    // {
    //    key_str = key_str + "-0x";
    //    key_str = key_str + std::to_string(Net_key[i]);
    // }


    char buff[50];
    for(int i=0; i<10; i++)
    {
        sprintf(&buff[i*2], "%02x", Net_key[i]);
    }
    std::string limited_buffer(buff, 20);
    //int len = snprintf(html_buffer, sizeof(html_buffer), resp_str, count_press, limited_buffer.c_str());

    //snprintf(html_buffer, key_str.length(), key_str.c_str());
    //httpd_resp_send(req, html_buffer, HTTPD_RESP_USE_STRLEN);

    int len = snprintf(html_buffer, sizeof(html_buffer), (const char *)  index_html_start, limited_buffer.c_str());
    httpd_resp_send(req, html_buffer, HTTPD_RESP_USE_STRLEN);
   // httpd_resp_send(req, (const char *) index_html_start, index_html_end - index_html_start);
    return ESP_OK;
}

esp_err_t HttpServer::button_post_handler(httpd_req_t *req) {
    ESP_LOGI(TAG_HTTP, "Button pressed!");
    count_press++;
    httpd_resp_send(req, "Button pressed!", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t HttpServer::submit_post_handler(httpd_req_t *req) {
     char input_string[100] = {0}; // Chuỗi nhận từ client
    char buf[100];
    int ret, remaining = req->content_len;
        ESP_LOGI(TAG_HTTP, "/sbmit %d", remaining);

    while (remaining > 0) {
        if ((ret = httpd_req_recv(req, buf, MIN(remaining, sizeof(buf) -1 )))  <= 0) {
              ESP_LOGI(TAG, "HTTPD err ID %d", ret);
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                 ESP_LOGI(TAG, "HTTPD_SOCK_ERR_TIMEOUT");
                continue; // Retry receiving if timeout
            }
            return ESP_FAIL;
        }
        remaining -= ret;
        buf[ret] = '\0'; 
               /* Log data received */
        // Parse the received data (assuming form-urlencoded format)
        char *input = strstr(buf, "inputText=");
        if (input) {
            input += strlen("inputText=");
            strcpy(input_string, input);
            // Decode URL-encoded characters if necessary (e.g., '+' to ' ')
            // for (char *p = input_string; *p; p++) {
            //     if (*p == '+') {
            //         *p = ' ';
            //     }
            // }
            ESP_LOGI(TAG, "Received input: %s", input_string);
        }
    }

   httpd_resp_send(req, "Submitted", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// Function to handle Wi-Fi setup form submission
esp_err_t wifi_setup_post_handler(httpd_req_t *req) {
    char buf[100];
    int ret, remaining = req->content_len;

    // Initialize buffers for SSID and password
    char ssid[32] = {0};
    char password[64] = {0};

    // Receive the form data
    while (remaining > 0) {
        if ((ret = httpd_req_recv(req, buf, MIN(remaining, sizeof(buf)))) <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                continue;
            }
            return ESP_FAIL;
        }
        remaining -= ret;
        buf[ret] = '\0';

        // Parse SSID and password from form data
       // char *ssid_pos = strstr(buf, "ssid=");
        char *password_pos = strstr(buf, "password=");
        char *ssid_pos = strtok(buf, "&");
        if (ssid_pos) {
            sscanf(ssid_pos, "ssid=%31s", ssid);
        }
        if (password_pos) {
            sscanf(password_pos, "password=%63s", password);
        }
    }

    ESP_LOGI(TAG, "SSID: %s", ssid);
    ESP_LOGI(TAG, "Password: %s", password);

    // Store SSID and password in NVS
    // nvs_handle_t nvs_handle;
    // esp_err_t err = nvs_open("wifi_config", NVS_READWRITE, &nvs_handle);
    // if (err != ESP_OK) {
    //     ESP_LOGE(TAG, "Error opening NVS handle!");
    //     return ESP_FAIL;
    // }
    // nvs_set_str(nvs_handle, "ssid", ssid);
    // nvs_set_str(nvs_handle, "password", password);
    // nvs_commit(nvs_handle);
    // nvs_close(nvs_handle);

    // Respond with a success message
    const char resp[] = "Wi-Fi credentials received successfully!";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);

    // Restart the ESP32 to apply the new Wi-Fi settings
   // esp_restart();

    return ESP_OK;
}
void HttpServer::begin() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

config.max_uri_handlers =1000;
config.max_resp_headers = 1000;
    httpd_uri_t hello = {
        .uri       = "/",
        .method    = HTTP_GET,
        .handler   = hello_get_handler,
        .user_ctx  = NULL
    };

    httpd_uri_t button = {
        .uri       = "/button",
        .method    = HTTP_POST,
        .handler   = button_post_handler,
        .user_ctx  = NULL
    };

    httpd_uri_t submit = {
        .uri       = "/submit",
        .method    = HTTP_POST,
        .handler   = submit_post_handler,
        .user_ctx  = NULL
    };

    httpd_uri_t setupo_wifi_sta = {
        .uri       = "/setup",
        .method    = HTTP_POST,
        .handler   = wifi_setup_post_handler,
        .user_ctx  = NULL
    };

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &hello);
        httpd_register_uri_handler(server, &button);
        httpd_register_uri_handler(server, &submit);
        httpd_register_uri_handler(server, &setupo_wifi_sta);
        ESP_LOGI(TAG_HTTP, "HTTP server started");
    } else {
        ESP_LOGI(TAG_HTTP, "Failed to start HTTP server");
    }
}

void HttpServer::stop() {
    if (server) {
        httpd_stop(server);
        server = nullptr;
        ESP_LOGI(TAG_HTTP, "HTTP server stopped");
    }
}


MqttClient::MqttClient(const char* broker_uri, const char* token){
    /*
        uri là bao gồm hostname + giao thức + port
        token như một mã định danh client mà sever dùng để phân việc các thiết bị
    */
   mqtt_cfg = {};
    mqtt_cfg.broker.address.uri = broker_uri; 
    mqtt_cfg.broker.address.port = 1883;
    mqtt_cfg.credentials.username = token;
     ESP_LOGI("Mqtt", "set init");
     client = {};
    client = esp_mqtt_client_init(&mqtt_cfg);
}

void MqttClient::start() {
    esp_mqtt_client_register_event(client, (esp_mqtt_event_id_t) ESP_EVENT_ANY_ID,(esp_event_handler_t) &MqttClient::mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}

esp_err_t MqttClient::mqtt_event_handler_cb(esp_mqtt_event_handle_t event) {
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;

    switch (event->event_id) {
    case MQTT_EVENT_CONNECTED: {
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        // Gửi một message test đến ThingsBoard để kiểm tra kết nối
        msg_id = esp_mqtt_client_publish(client, "v1/devices/me/telemetry", "{\"temp\":25}", 0, 1, 0);
        ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
    }
        break;
    // Các xử lý sự kiện khác
    default:
        break;
    }
    return ESP_OK;
}

void MqttClient::mqtt_event_handler(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data) {
    MqttClient* client = reinterpret_cast<MqttClient*>(handler_args);
    client->mqtt_event_handler_cb((esp_mqtt_event_handle_t)event_data);
}

void MqttClient::publish(const char* topic, const char* payload) {
    esp_mqtt_client_publish(client, topic, payload, 0, 1, 0);
}
// #ifdef __cplusplus
// }
// #endif
