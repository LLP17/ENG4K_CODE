#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "esp_camera.h"
#include <ESP_I2S.h>

const int buff_size = 128;
int available_bytes, read_bytes;
uint8_t buffer[buff_size];
I2SClass SPKR;
I2SClass MIC;

// Added buffer for mic recording
static int16_t mic_buffer[1024];

/* Audio and Microphone Stream Configurations */
typedef struct {
  int sampleRate;
  int channels;
  int bitDepth;
} audioStreamConfig_t;

const int DEFAULT_MIC_STREAM_SAMPLE_RATE = 16000;
const int DEFAULT_AUDIO_STREAM_SAMPLE_RATE = 16000;
const int MIC_BUFFER_SIZE = 1024;
const int AUDIO_BUFFER_SIZE = 1024;
const int DEFAULT_MIC_STREAM_CHANNELS = 2;
const int DEFAULT_AUDIO_STREAM_CHANNELS = 2;
const int DEFAULT_MIC_STREAM_BIT_DEPTH = 16;
const int DEFAULT_AUDIO_STREAM_BIT_DEPTH = 16;

audioStreamConfig_t micStreamConfig, audioStreamConfig;

#define CAMERA_MODEL_ESP32S3_EYE
#include "camera_pins.h"
#include "app_httpd.h"

/* WiFi Access Point Configuration */
const char *ssid_AP = "ECHO-AP";
const char *password_AP = "ourbread";
IPAddress local_IP(192, 168, 1, 100);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

/* Motor Configurations */
int motor1Pin1 = 21;
int motor1Pin2 = 22;
int enable1Pin = 23;
int motor2Pin1 = 5;
int motor2Pin2 = 19;
int enable2Pin = 18;

const int freq = 30000;
const int pwm_resolution = 8;
const int channel1 = 0;
const int channel2 = 1;

// Stub for motor control
void controlMotor(const char *dir, double x, double y) {
  Serial.printf("Motor direction: %s | x: %.2f | y: %.2f\n", dir, x, y);
}

// Audio play function (proper casting for I2S)
void playSound(int16_t *data, size_t len) {
  SPKR.write((const uint8_t *)data, len * sizeof(int16_t));
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    DynamicJsonDocument doc(256);
    DeserializationError error = deserializeJson(doc, data, len);
    if (error) {
      Serial.print(F("deserializeJson() failed with code "));
      Serial.println(error.c_str());
      return;
    }

    const char *direction = doc["direction"];
    double x = doc["x"];
    double y = doc["y"];
    controlMotor(direction, x, y);
  }
}

void handleAudioStream(const char *data, const char *len, const char *sampleRate, const char *channels, const char *bitDepth) {
  int dataLen = atoi(len);
  int sampleRateInt = atoi(sampleRate);
  int channelsInt = atoi(channels);
  int bitDepthInt = atoi(bitDepth);
  Serial.print("handleAudioStream: ");
  Serial.print(dataLen);
  Serial.print(", ");
  Serial.print(sampleRateInt);
  Serial.print(", ");
  Serial.print(channelsInt);
  Serial.print(", ");
  Serial.println(bitDepthInt);
  int16_t *audioData = (int16_t *)data;
  playSound(audioData, dataLen);
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
             AwsEventType type, void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo *)arg;

  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;

    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;

    case WS_EVT_DATA:
      if (info->final && info->index == 0 && info->len == len) {
        if (info->opcode == WS_TEXT) {
          handleWebSocketMessage(arg, data, len);
        } else if (info->opcode == WS_BINARY) {
          Serial.printf("🔊 Audio chunk received: %d bytes\n", len);
          playSound((int16_t *)data, len / sizeof(int16_t));
        }
      }
      break;

    default:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void initMic() {
  MIC.setPins(5, 25, 26, 35, 0); // SCK, WS, SDOUT, SDIN, MCLK
  MIC.begin(I2S_MODE_STD, 16000, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO);
}

void initSpeaker() {
  SPKR.setPinsPdmTx(27, 32, -1); // CLK, DATA, unused
  SPKR.begin(I2S_MODE_PDM, 16000, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO);
}

void readMic(int16_t *data, size_t len) {
  available_bytes = MIC.available();
  if (available_bytes < MIC_BUFFER_SIZE) {
    MIC.read(mic_buffer, available_bytes);
  } else {
    MIC.read(mic_buffer, MIC_BUFFER_SIZE);
  }
}

void closeMic() {
  MIC.end();
}

void closeSpeaker() {
  SPKR.end();
}

void configureMicStream(int sampleRate, int channels, int bitDepth) {
  if (sampleRate == -1) {
    micStreamConfig.sampleRate = DEFAULT_MIC_STREAM_SAMPLE_RATE;
  }
  micStreamConfig.sampleRate = 16000;
  micStreamConfig.channels = 2;
  micStreamConfig.bitDepth = 16;
}

void configureAudioStream(int sampleRate, int channels, int bitDepth) {
  audioStreamConfig.sampleRate = 16000;
  audioStreamConfig.channels = 2;
  audioStreamConfig.bitDepth = 16;
}

void initWifi() {
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  if (WiFi.softAPConfig(local_IP, gateway, subnet)) {
    Serial.println("AP Config Ready");
  } else {
    Serial.println("AP Config Failed!");
  }

  if (WiFi.softAP(ssid_AP, password_AP)) {
    Serial.println("AP Started");
    Serial.println("IP Address: " + WiFi.softAPIP().toString());
  } else {
    Serial.println("AP Failed to Start!");
  }
}

void setup() {
  Serial.begin(115200);
  if (!SPIFFS.begin(true)) {
    Serial.println("Failed to mount SPIFFS");
    return;
  }

  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);
  ledcAttach(enable1Pin, freq, pwm_resolution);
  ledcAttach(enable2Pin, freq, pwm_resolution);
  ledcWrite(channel1, 0);
  ledcWrite(channel2, 0);

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
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_SVGA;
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 2;

  if (psramFound()) {
    config.jpeg_quality = 10;
    config.fb_count = 2;
    config.grab_mode = CAMERA_GRAB_LATEST;
  } else {
    config.fb_count = 1;
    config.fb_location = CAMERA_FB_IN_DRAM;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t *s = esp_camera_sensor_get();
  s->set_vflip(s, 1);
  s->set_brightness(s, 1);
  s->set_saturation(s, 0);

  initWifi();
  initWebSocket();
  initSpeaker();
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  server.begin();
  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.softAPIP());
  Serial.println("' to connect");
}

void loop() {
  ws.cleanupClients();
}