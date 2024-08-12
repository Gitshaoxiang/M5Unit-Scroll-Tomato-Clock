#include <M5Unified.h>
#include <M5GFX.h>
#include "./lv_anim/lv_anim.h"
#include "Unit_Encoder.h"

#include "num.h"
#include "idle_bg.h"
#include "focus_bg.h"
#include "done_bg.h"
#include "vector"

#define CANVAS_HEIGHT 100
#define CANVAS_WIDTH  320

#define NUM_WIDTH  46
#define NUM_HEIGHT 100

void canvasInit();
void focus_run();
void focus_done();
void updateEncoderDisplay(int min, int sec);
void updateProgressBar(int percent);

M5Canvas bg_canvas(&M5.Display);
Unit_Encoder sensor;

signed short int last_encoder_value = 9999;

int count_sec = 0;
int count_min = 0;
int total_sec = 0;

void setup() {
    canvasInit();
    sensor.begin(&Wire, ENCODER_ADDR, 32, 33);
}

void loop() {
    signed short int encoder_value = sensor.getEncoderValue();
    if (last_encoder_value != encoder_value) {
        total_sec = encoder_value;
        if (last_encoder_value > encoder_value) {
            sensor.setLEDColor(0xff0000, 0);
        } else {
            sensor.setLEDColor(0x0000ff, 0);
        }
        last_encoder_value = encoder_value;
        if (encoder_value <= 0) {
            sensor.setEncoderValue(0);
            updateEncoderDisplay(0, 0);
        } else {
            count_sec = encoder_value % 60;
            count_min = encoder_value / 60;
            updateEncoderDisplay(count_min, count_sec);
        }
        sensor.setLEDColor(0x000000, 0);
    }
    if (!sensor.getButtonStatus()) {
        int count = 3;
        while (count--) {
            sensor.setLEDColor(0x00ff00, 0);
            delay(50);
            sensor.setLEDColor(0x000000, 0);
            delay(50);
        }
        focus_run();
        focus_done();
        delay(100);
    }
}

void canvasInit() {
    M5.begin();
    M5.Display.begin();
    bg_canvas.createSprite(320, 240);
    bg_canvas.pushImage(0, 0, 320, 240, image_data_idle_bg);
    bg_canvas.pushSprite(0, 0);
}

LVGL::Anim_Path pageAni;

void transition(uint16_t color, const uint16_t* bg) {
    pageAni.setAnim(LVGL::ease_out, 0, 640, 300);
    pageAni.resetTime(M5.millis());

    while (!pageAni.isFinished(M5.millis())) {
        int ani = pageAni.getValue(M5.millis());
        bg_canvas.fillCircle(0, 0, ani, color);
        bg_canvas.pushSprite(0, 0);
    }

    pageAni.setAnim(LVGL::ease_out, 640, 0, 300);
    pageAni.resetTime(M5.millis());

    while (!pageAni.isFinished(M5.millis())) {
        int ani = pageAni.getValue(M5.millis());
        bg_canvas.pushImage(0, 0, 320, 240, bg);
        bg_canvas.fillCircle(0, 0, ani, color);
        bg_canvas.pushSprite(0, 0);
    }
    bg_canvas.pushImage(0, 0, 320, 240, bg);
    bg_canvas.pushSprite(0, 0);
}

void focus_run() {
    transition(0xeac0, image_data_focus_bg);
    time_t start_time = 0;
    // bg_canvas.pushImage(0, 0, 320, 240, image_data_focus_bg);

    int total = total_sec;

    do {
        if (millis() - start_time >= 1000) {
            start_time = millis();
            total_sec--;
            count_sec = total_sec % 60;
            count_min = total_sec / 60;
            updateProgressBar(100 - (int)((float)total_sec / (float)total * 100));
            updateEncoderDisplay(count_min, count_sec);
        }
        if (!sensor.getButtonStatus()) {
            break;
        }
    } while (total_sec);
}

void focus_done() {
    transition(0x58da, image_data_done_bg);
    // bg_canvas.pushImage(0, 0, 320, 240, image_data_done_bg);
    bg_canvas.pushSprite(0, 0);
    while (1) {
        if (!sensor.getButtonStatus()) {
            break;
        }
        delay(10);
    }
    transition(0x045c, image_data_idle_bg);
    // bg_canvas.pushImage(0, 0, 320, 240, image_data_idle_bg);
    bg_canvas.pushSprite(0, 0);
    updateEncoderDisplay(0, 0);

    count_sec          = 0;
    count_min          = 0;
    total_sec          = 0;
    last_encoder_value = 0;
    sensor.setEncoderValue(0);
}

void updateProgressBar(int percent) {
    int w = map(percent, 0, 100, 0, 256);
    Serial.printf("w: %d\n", w);
    bg_canvas.drawRoundRect(27, 179, 266, 28, 15, TFT_WHITE);
    bg_canvas.fillSmoothRoundRect(32, 184, w, 18, 15, TFT_WHITE);
    bg_canvas.pushSprite(0, 0);
}

LVGL::Anim_Path numAnimK;
LVGL::Anim_Path numAnimH;
LVGL::Anim_Path numAnimT;
LVGL::Anim_Path numAnimO;

int last_min = 0;
int last_sec = 0;

void updateEncoderDisplay(int min, int sec) {
    bg_canvas.fillSmoothRoundRect(10, 61, 300, 105, 20, BLACK);
    int min_h = abs(last_min / 10);
    int min_l = abs(last_min % 10);
    int sec_h = abs(last_sec / 10);
    int sec_l = abs(last_sec % 10);

    int new_min_h = abs(min / 10);
    int new_min_l = abs(min % 10);
    int new_sec_h = abs(sec / 10);
    int new_sec_l = abs(sec % 10);

    last_min = min;
    last_sec = sec;

    numAnimK.setAnim(LVGL::ease_out, min_h * NUM_HEIGHT, new_min_h * NUM_HEIGHT, 50);
    numAnimH.setAnim(LVGL::ease_out, min_l * NUM_HEIGHT, new_min_l * NUM_HEIGHT, 50);
    numAnimT.setAnim(LVGL::ease_out, sec_h * NUM_HEIGHT, new_sec_h * NUM_HEIGHT, 50);
    numAnimO.setAnim(LVGL::ease_out, sec_l * NUM_HEIGHT, new_sec_l * NUM_HEIGHT, 50);

    numAnimK.resetTime(M5.millis());
    numAnimH.resetTime(M5.millis());
    numAnimT.resetTime(M5.millis());
    numAnimO.resetTime(M5.millis());

    while (!numAnimK.isFinished(M5.millis()) || !numAnimH.isFinished(M5.millis()) ||
           !numAnimT.isFinished(M5.millis()) || !numAnimO.isFinished(M5.millis())) {
        int aniMh = numAnimK.getValue(M5.millis());
        int aniMl = numAnimH.getValue(M5.millis());
        int aniSh = numAnimT.getValue(M5.millis());
        int aniSl = numAnimO.getValue(M5.millis());

        bg_canvas.pushImage(27, 66, NUM_WIDTH, NUM_HEIGHT, &image_data_num[aniMh * NUM_WIDTH]);
        bg_canvas.pushImage(82, 66, NUM_WIDTH, NUM_HEIGHT, &image_data_num[aniMl * NUM_WIDTH]);
        bg_canvas.pushImage(137, 66, NUM_WIDTH, NUM_HEIGHT, &image_data_num[10 * NUM_HEIGHT * NUM_WIDTH]);
        bg_canvas.pushImage(192, 66, NUM_WIDTH, NUM_HEIGHT, &image_data_num[aniSh * NUM_WIDTH]);
        bg_canvas.pushImage(247, 66, NUM_WIDTH, NUM_HEIGHT, &image_data_num[aniSl * NUM_WIDTH]);
        bg_canvas.pushSprite(0, 0);
    }
    bg_canvas.pushImage(27, 66, NUM_WIDTH, NUM_HEIGHT, &image_data_num[new_min_h * NUM_HEIGHT * NUM_WIDTH]);
    bg_canvas.pushImage(82, 66, NUM_WIDTH, NUM_HEIGHT, &image_data_num[new_min_l * NUM_HEIGHT * NUM_WIDTH]);
    bg_canvas.pushImage(137, 66, NUM_WIDTH, NUM_HEIGHT, &image_data_num[10 * NUM_HEIGHT * NUM_WIDTH]);
    bg_canvas.pushImage(192, 66, NUM_WIDTH, NUM_HEIGHT, &image_data_num[new_sec_h * NUM_HEIGHT * NUM_WIDTH]);
    bg_canvas.pushImage(247, 66, NUM_WIDTH, NUM_HEIGHT, &image_data_num[new_sec_l * NUM_HEIGHT * NUM_WIDTH]);
    bg_canvas.pushSprite(0, 0);
}