// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "esp_http_server.h"
#include "esp_timer.h"
#include "esp_camera.h"
#include "img_converters.h"
#include "fb_gfx.h"
#include "driver/ledc.h"
#include "driver/gpio.h"

#include "sdkconfig.h"


#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_ARDUHAL_ESP_LOG)
#include "esp32-hal-log.h"
#define TAG ""
#else
#include "esp_log.h"
static const char *TAG = "camera_httpd";
#endif

#define OUTPUT 1
#define HIGH 1 
#define LOW 0 
#define MOTOR1PIN1   21
#define MOTOR1PIN2   22 
#define ENABLE1PIN   23
#define MOTOR2PIN1   5
#define MOTOR2PIN2   19
#define ENABLE2PIN   18 
#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<MOTOR1PIN1) | (1ULL<<MOTOR1PIN2) | (1ULL << ENABLE1PIN)|(1ULL<<MOTOR2PIN1)| (1ULL<<MOTOR1PIN2) | (1ULL<<ENABLE2PIN))
// Motor Pins
int motor1Pin1 = 21; 
int motor1Pin2 = 22; 
int enable1Pin = 23;
int motor2Pin1 = 5; 
int motor2Pin2 = 19; 
int enable2Pin = 18;

int RightSpeed = 0; 
int LeftSpeed  = 0; 

typedef struct
{
    httpd_req_t *req;
    size_t len;
} jpg_chunking_t;

#define PART_BOUNDARY "123456789000000000000987654321"
static const char *_STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char *_STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char *_STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\nX-Timestamp: %d.%06d\r\n\r\n";

httpd_handle_t stream_httpd = NULL;
httpd_handle_t camera_httpd = NULL;

typedef struct
{
    size_t size;  // number of values used for filtering
    size_t index; // current value index
    size_t count; // value count
    int sum;
    int *values; // array to be filled with values
} ra_filter_t;

static ra_filter_t ra_filter;
int map(int x, int in_min, int in_max, int out_min, int out_max);
int constrain(int value, int min_val, int max_val);
void setMotorSpeed(int leftSpeed, int rightSpeed);
static ra_filter_t *ra_filter_init(ra_filter_t *filter, size_t sample_size)
{
    memset(filter, 0, sizeof(ra_filter_t));

    filter->values = (int *)malloc(sample_size * sizeof(int));
    if (!filter->values)
    {
        return NULL;
    }
    memset(filter->values, 0, sample_size * sizeof(int));

    filter->size = sample_size;
    return filter;
}

static int ra_filter_run(ra_filter_t *filter, int value)
{
    if (!filter->values)
    {
        return value;
    }
    filter->sum -= filter->values[filter->index];
    filter->values[filter->index] = value;
    filter->sum += filter->values[filter->index];
    filter->index++;
    filter->index = filter->index % filter->size;
    if (filter->count < filter->size)
    {
        filter->count++;
    }
    return filter->sum / filter->count;
}

static esp_err_t stream_handler(httpd_req_t *req)
{
    camera_fb_t *fb = NULL;
    struct timeval _timestamp;
    esp_err_t res = ESP_OK;
    size_t _jpg_buf_len = 0;
    uint8_t *_jpg_buf = NULL;
    char *part_buf[128];

    static int64_t last_frame = 0;
    if (!last_frame)
    {
        last_frame = esp_timer_get_time();
    }

    res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    if (res != ESP_OK)
    {
        return res;
    }

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "X-Framerate", "60");

    while (true)
    {
        fb = esp_camera_fb_get();
        if (!fb)
        {
            ESP_LOGE(TAG, "Camera capture failed");
            res = ESP_FAIL;
        }
        else
        {
            _timestamp.tv_sec = fb->timestamp.tv_sec;
            _timestamp.tv_usec = fb->timestamp.tv_usec;
            if (fb->format != PIXFORMAT_JPEG)
            {
                bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
                esp_camera_fb_return(fb);
                fb = NULL;
                if (!jpeg_converted)
                {
                    ESP_LOGE(TAG, "JPEG compression failed");
                    res = ESP_FAIL;
                }
            }
            else
            {
                _jpg_buf_len = fb->len;
                _jpg_buf = fb->buf;
            }
        }
        if (res == ESP_OK)
        {
            res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
        }
        if (res == ESP_OK)
        {
            size_t hlen = snprintf((char *)part_buf, 128, _STREAM_PART, _jpg_buf_len, _timestamp.tv_sec, _timestamp.tv_usec);
            res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
        }
        if (res == ESP_OK)
        {
            res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
        }
        if (fb)
        {
            esp_camera_fb_return(fb);
            fb = NULL;
            _jpg_buf = NULL;
        }
        else if (_jpg_buf)
        {
            free(_jpg_buf);
            _jpg_buf = NULL;
        }
        if (res != ESP_OK)
        {
            ESP_LOGI(TAG, "res != ESP_OK : %d , break!", res);
            break;
        }
        int64_t fr_end = esp_timer_get_time();

        int64_t frame_time = fr_end - last_frame;
        last_frame = fr_end;
        frame_time /= 1000;
        uint32_t avg_frame_time = ra_filter_run(&ra_filter, frame_time);
        ESP_LOGI(TAG, "MJPG: %uB %ums (%.1ffps), AVG: %ums (%.1ffps)",
                 (uint32_t)(_jpg_buf_len),
                 (uint32_t)frame_time, 1000.0 / (uint32_t)frame_time,
                 avg_frame_time, 1000.0 / avg_frame_time);
    }
    ESP_LOGI(TAG, "Stream exit!");
    last_frame = 0;
    return res;
}

static esp_err_t parse_get(httpd_req_t *req, char **obuf)
{
    char *buf = NULL;
    size_t buf_len = 0;

    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1)
    {
        buf = (char *)malloc(buf_len);
        if (!buf)
        {
            httpd_resp_send_500(req);
            return ESP_FAIL;
        }
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK)
        {
            *obuf = buf;
            return ESP_OK;
        }
        free(buf);
    }
    httpd_resp_send_404(req);
    return ESP_FAIL;
}

const char index_web[] =
"\
<html>\
  <head>\
    <title>Video Streaming Demonstration</title>\
  </head>\
  <body>\
    <h1>Video Streaming Demonstration</h1>\
    <img src=\"/stream\"\ style=\"transform:rotate(180deg)\"/>\
  </body>\
</html>\
";

static esp_err_t index_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    // httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
    sensor_t *s = esp_camera_sensor_get();
    if (s != NULL)
    {
        // return httpd_resp_sendstr(req, (const char *)index_web);
        return httpd_resp_send(req, (const char *)index_web, sizeof(index_web));
        // return httpd_resp_send(req, (const char *)index_ov2640_html_gz, index_ov2640_html_gz_len);
    }
    else
    {
        ESP_LOGE(TAG, "Camera sensor not found");
        return httpd_resp_send_500(req);
    }
}
static esp_err_t joystick_handler(httpd_req_t *req){ 
  char query[100];
  char x_string[10] = {0}; 
  char y_string[10] = {0}; 
  esp_err_t x_param_available = ESP_OK;  
  esp_err_t y_param_available = ESP_OK; 

  if(httpd_req_get_url_query_str(req,query,sizeof(query)) == ESP_OK){ 
      x_param_available = httpd_query_key_value(query,"x",x_string, sizeof(x_string));
      y_param_available = httpd_query_key_value(query, "y",y_string,sizeof(y_string));
      if( x_param_available == ESP_OK && y_param_available == ESP_OK){ 
            // Calculate motor speeds and directions
          int leftSpeed = constrain(map(atoi(y_string) + atoi(x_string), -100, 100, -255, 255), -255, 255);
          int rightSpeed = constrain(map(atoi(y_string) - atoi(x_string), -100, 100, -255, 255), -255, 255);
         setMotorSpeed(leftSpeed, rightSpeed); 
      }
  }
  /* Send response */
    const char *resp = "Query received";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}
int constrain(int value, int min_val, int max_val) {
    if (value < min_val) return min_val;
    if (value > max_val) return max_val;
    return value;
}
int map(int x, int in_min, int in_max, int out_min, int out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
void analogWrite(int pin, int value) {
    int channel = pin % 16;  // Choose a channel (ESP32 supports up to 16 channels)
    
    // Configure the LEDC channel
    ledc_channel_config_t ledc_channel = {
        .gpio_num   = pin,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel    = (ledc_channel_t)channel,
        .intr_type  = LEDC_INTR_DISABLE,
        .timer_sel  = LEDC_TIMER_0,
        .duty       = value,  // Duty cycle (0 - 255 for 8-bit resolution)
        .hpoint     = 0
    };
    
    ledc_channel_config(&ledc_channel);
}
// Function to set GPIO output
void digitalWrite(int pin, int value) {
    gpio_set_level((gpio_num_t)pin, value);
}
void pinMode(int pin, int mode) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << pin),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
}

void setMotorSpeed(int leftSpeed, int rightSpeed){ 
  // Left motor
  if (leftSpeed > 0) {
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, HIGH);
  } else if (leftSpeed < 0) {
    digitalWrite(motor1Pin1, HIGH);
    digitalWrite(motor1Pin2, LOW);
    leftSpeed = -leftSpeed;
  } else {
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, LOW);
  }

  // Right motor
  if (rightSpeed > 0) {
    digitalWrite(motor2Pin1, HIGH);
    digitalWrite(motor2Pin2, LOW);
  } else if (rightSpeed < 0) {
    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, HIGH);
    rightSpeed = -rightSpeed;
  } else {
    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, LOW);
  }

  // Set PWM values
  analogWrite(enable1Pin, leftSpeed);
  analogWrite(enable2Pin, rightSpeed);
}
void startCameraServer()
{
  

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 16;

    httpd_uri_t index_uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = index_handler,
        .user_ctx = NULL};
    
    httpd_uri_t joystick_uri = { 
      .uri = "/joystick",
      .method = HTTP_GET, 
      .handler = joystick_handler, 
      .user_ctx = NULL
    }; 

    httpd_uri_t stream_uri = {
        .uri = "/stream",
        .method = HTTP_GET,
        .handler = stream_handler,
        .user_ctx = NULL};
    ra_filter_init(&ra_filter, 20);
   

    ESP_LOGI(TAG, "Starting web server on port: '%d'", config.server_port);
    if (httpd_start(&camera_httpd, &config) == ESP_OK)
    {
        httpd_register_uri_handler(camera_httpd, &index_uri);
        httpd_register_uri_handler(camera_httpd, &stream_uri);
    }
}
