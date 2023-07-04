#pragma once
#include <Arduino.h>
#include <esp_camera.h>
#include <esp_http_server.h>
#include <esp_wifi.h>

#include "soc/rtc_cntl_reg.h"
#include "soc/soc.h"

#define _STREAM_BOUND "==stream=="
static const char* _STREAM_HEADER = "multipart/x-mixed-replace;boundary=" _STREAM_BOUND;
static const char* _CNT_TYPE_LEN = "X-Framerate: 60\r\nContent-Type: image/jpeg\r\nContent-Length: ";
static const char* _RNRN = "\r\n\r\n";
static const char* _STREAM_END = "\r\n--" _STREAM_BOUND "\r\n";
httpd_handle_t _stream_httpd = NULL;
bool _stream_state = false;

static esp_err_t _stream_handler(httpd_req_t* req);
void cam_stream_begin(uint16_t port = 82);
void cam_stream_end();

void cam_stream_begin(uint16_t port) {
    esp_wifi_set_ps(WIFI_PS_NONE);              // no power save
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);  // disable brownout

    httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
    cfg.server_port = port;
    cfg.send_wait_timeout = 1;
    cfg.max_uri_handlers = 16;
    httpd_uri_t index = {"/", HTTP_GET, _stream_handler, NULL};
    if (httpd_start(&_stream_httpd, &cfg) == ESP_OK) {
        httpd_register_uri_handler(_stream_httpd, &index);
        _stream_state = 1;
    }
}

void cam_stream_end() {
    if (_stream_httpd) httpd_stop(_stream_httpd);
    _stream_httpd = NULL;
    _stream_state = 0;
}

static esp_err_t _stream_handler(httpd_req_t* req) {
    camera_fb_t* fb = NULL;
    char buf[72];
    esp_err_t res = httpd_resp_set_type(req, _STREAM_HEADER);

    while (1) {
        if (res != ESP_OK || !_stream_state) break;

        fb = esp_camera_fb_get();
        if (!fb) continue;

        strcpy(buf, _CNT_TYPE_LEN);
        utoa(fb->len, buf + strlen(buf), 10);
        strcpy(buf + strlen(buf), _RNRN);

        res = httpd_resp_send_chunk(req, (const char*)buf, strlen(buf));

        if (res == ESP_OK) {
            res = httpd_resp_send_chunk(req, (const char*)fb->buf, fb->len);
        }
        if (res == ESP_OK) {
            res = httpd_resp_send_chunk(req, _STREAM_END, strlen(_STREAM_END));
        }
        esp_camera_fb_return(fb);
        delay(20);
    }
    return res;
}