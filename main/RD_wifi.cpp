#ifdef __cplusplus
extern "C"
{
#endif
#include "RD_wifi.hpp"
#include <stdio.h>
#include "string.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include <esp32c3/rom/ets_sys.h>
#include "nvs_flash.h"
//  #include "nvs_flash.h"
static const char *TAG = "OTA:";
char* ssid_hao = "Hao_ESP32";
#define EXAMPLE_ESP_WIFI_SSID      "Hao_ESP32"
#define EXAMPLE_ESP_WIFI_PASS      "RDSL@2804"
#define EXAMPLE_ESP_WIFI_CHANNEL   1
#define EXAMPLE_MAX_AP_CON         4

char* SSID_INIT          =       "Hao_ESP32";
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG,"station" MACSTR "join, AID=%d",MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
       ESP_LOGI(TAG, "station " MACSTR " leave, AID=%d",
                MAC2STR(event->mac), event->aid);
    }
}

// init wifi 
void wifi_init_softap(char * ssid_setup, char * pass_setup)
{
        esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
       ret = nvs_flash_init();
   }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_netif_init());                      //init TCP/IP
   ESP_LOGI(TAG, "Rang Dong wifi ap init");
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,    // handler even
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {};
	for(int i=0; i< strlen(ssid_setup); i++)
	{
		wifi_config.ap.ssid[i] = *(ssid_setup + i);
	}
	for(int i=0; i< strlen(pass_setup); i++)
	{
		wifi_config.ap.password[i] = *(pass_setup +i);
	}
    
    wifi_config.ap.ssid_len = strlen((const char*)ssid_setup);

    wifi_config.ap.channel  = EXAMPLE_ESP_WIFI_CHANNEL;
    wifi_config.ap.authmode = WIFI_AUTH_WPA2_PSK;
    wifi_config.ap.max_connection = EXAMPLE_MAX_AP_CON;
    wifi_config.ap.ssid_hidden = 0;
    if (strlen(ssid_setup) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;                               // hightLight
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
             ssid_setup, pass_setup, EXAMPLE_ESP_WIFI_CHANNEL);
}
void rd_wifi_init(void)
{
    ESP_LOGI(TAG, "Rang Dong wifi init");
    
    ESP_LOGI(TAG, "ESP_WIFI_MODE_AP");
    wifi_init_softap("ssid_hao123", EXAMPLE_ESP_WIFI_PASS);
}


Wifi::Wifi(){

}

void Wifi::begin(){
    
}
#ifdef __cplusplus
}
#endif
