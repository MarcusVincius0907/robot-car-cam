#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoWebsockets.h>
#include "esp_camera.h"


#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

using namespace websockets;

WebsocketsClient client;

const char* ssid = "MLEITE"; //Enter SSID
const char* password = "Coelho@0907"; //Enter Password
const char* websockets_server_host = "192.168.15.5"; //Enter server adress
const uint16_t websockets_server_port = 5000; // Enter server port


void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("WiFi connected");

  // Initialize camera
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;


  //eh possivel melhorar a qualidade do video por aqui
  config.frame_size = FRAMESIZE_QVGA; 
  config.jpeg_quality = 40;
  config.fb_count = 2;

  //if (psramFound()) {
    //config.frame_size = FRAMESIZE_UXGA; // 1600x1200
    //config.jpeg_quality = 10;
    //config.fb_count = 2;
  //} else {
    
  //}

  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Camera initialization failed");
    return;
  }

  while(!client.connect(websockets_server_host, websockets_server_port, "/")) { Serial.println("Web socket Not Connected!"); delay(500);  }
    Serial.println("Websocket Connected!");

}

void loop() {
    if (!client.available()) {
        Serial.println("WebSocket connection lost. Reconnecting...");
        while (!client.connect(websockets_server_host, websockets_server_port, "/")) {
            Serial.println("Reconnecting to WebSocket...");
            delay(500);
        }
        Serial.println("WebSocket reconnected");
    }

    camera_fb_t* fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("Camera capture failed");
        return;
    }

    if (fb->format == PIXFORMAT_JPEG) {
        //uint8_t testData[1024] = {0};
        //client.sendBinary((const char*)testData, sizeof(testData));
        client.sendBinary((const char*)fb->buf, fb->len);
        Serial.println("Frame sent");
    } else {
        Serial.println("Unsupported frame format");
    }

    esp_camera_fb_return(fb);
    //delay(33); // Control frame rate
}

