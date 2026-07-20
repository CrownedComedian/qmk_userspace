#include QMK_KEYBOARD_H

#include "raw_hid.h"
#include "quantum.h"

#include <math.h>


#define HID_MSG_LEN 32
#define _USE_MATH_DEFINES
#define BRANCHLESS_MIN(a, b) (a * (a < b)) + (b * (b <= a))
#define BRANCHLESS_MAX(a, b) (a * (a > b)) + (b * (b >= a))
#define DEG_TO_RAD(d) (d * M_PI / 180)
#define ALT_COS(multiplier, offset) cos( DEG_TO_RAD(M_PI * multiplier * (frame + offset) / 90) )
#define ALT_SIN(multiplier, offset) sin( DEG_TO_RAD(M_PI * multiplier * (frame + offset) / 90) )

#define READ_MSG_LINK (char*) "Link"
#define READ_MSG_SERV_BOOT_1 (char*) "Starting"
#define READ_MSG_SERV_BOOT_2 (char*) "Environment"
#define READ_MSG_SERV_BOOT_3 (char*) "Loading"
#define READ_MSG_SERV_BOOT_4 (char*) "Preparing level"
#define READ_MSG_SERV_BOOT_DONE (char*) "Done"
#define READ_MSG_RELINK (char*) "Relink"
#define READ_MSG_SERV_SHUTDOWN (char*) "Server Closed!"

// All raw HID messages must be 32 bytes long
// see: https://docs.qmk.fm/features/rawhid#api
#define MSG_LINK_TEST (uint8_t *)           "Link............................"
#define MSG_FRONT_LINK (uint8_t *)          "Front bdn9 connected!..........."
#define MSG_LEFT_LINK (uint8_t *)           "Left bdn9 connected!............"
#define MSG_RIGHT_LINK (uint8_t *)          "Right bdn9 connected!..........."
#define MSG_TOP_LINK (uint8_t *)            "Top bdn9 connected!............."
#define MSG_RGB_MOD (uint8_t *)             "RGB LED mode +.................."
#define MSG_RGB_RMOD (uint8_t *)            "RGB LED mode -.................."
#define MSG_RGB_HUI (uint8_t *)             "RGB LED hue +..................."
#define MSG_RGB_HUD (uint8_t *)             "RGB LED hue -..................."
#define MSG_RGB_SAI (uint8_t *)             "RGB LED saturation +............"
#define MSG_RGB_SAD (uint8_t *)             "RGB LED saturation -............"
#define MSG_RGB_VAI (uint8_t *)             "RGB LED value +................."
#define MSG_RGB_VAD (uint8_t *)             "RGB LED value -................."
#define MSG_START_STOP (uint8_t *)          "Start/Stop......................"
#define MSG_MINECRAFT_KEY_1 (uint8_t *)     "execute command 01.............."
#define MSG_MINECRAFT_KEY_2 (uint8_t *)     "execute command 02.............."
#define MSG_MINECRAFT_KEY_3 (uint8_t *)     "execute command 03.............."
#define MSG_MINECRAFT_KEY_4 (uint8_t *)     "execute command 04.............."
#define MSG_MINECRAFT_KEY_5 (uint8_t *)     "execute command 05.............."
#define MSG_MINECRAFT_KEY_6 (uint8_t *)     "execute command 06.............."
#define MSG_MINECRAFT_KEY_7 (uint8_t *)     "execute command 07.............."
#define MSG_MINECRAFT_KEY_8 (uint8_t *)     "execute command 08.............."
#define MSG_MINECRAFT_KEY_9 (uint8_t *)     "execute command 09.............."
#define MSG_MINECRAFT_KEY_10 (uint8_t *)    "execute command 10.............."
#define MSG_MINECRAFT_KEY_11 (uint8_t *)    "execute command 11.............."
#define MSG_MINECRAFT_KEY_12 (uint8_t *)    "execute command 12.............."
#define MSG_MINECRAFT_KEY_13 (uint8_t *)    "execute command 13.............."
#define MSG_MINECRAFT_KEY_14 (uint8_t *)    "execute command 14.............."
#define MSG_MINECRAFT_KEY_15 (uint8_t *)    "execute command 15.............."
#define MSG_MINECRAFT_KEY_16 (uint8_t *)    "execute command 16.............."
#define MSG_MINECRAFT_KEY_17 (uint8_t *)    "execute command 17.............."
#define MSG_MINECRAFT_KEY_18 (uint8_t *)    "execute command 18.............."
#define MSG_MINECRAFT_KEY_19 (uint8_t *)    "execute command 19.............."
#define MSG_MINECRAFT_KEY_20 (uint8_t *)    "execute command 20.............."
#define MSG_MINECRAFT_KEY_21 (uint8_t *)    "execute command 21.............."

/* Variables used for the cmd block animation */
unsigned int frame           = 0;
uint8_t      transition_s    = 0;
uint8_t      transition_vin  = 255;
uint8_t      transition_vout = 255;
bool         transition_off  = false;

bool keys_down[9] = {
    false, false, false,
    false, false, false,
    false, false, false,
};

uint8_t status_leds[9] = {
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
};


uint8_t brightness(uint8_t amplitude, double cos_frequency, double sin_frequency, uint8_t offset, uint8_t value) {
    return (amplitude * rgb_matrix_get_val() / 255) *
           (ALT_COS(cos_frequency, offset) + ALT_SIN(sin_frequency, offset)) +
           (value * rgb_matrix_get_val() / 255);
}


void update_led(int index, uint8_t h, uint8_t s, uint8_t v) {
    HSV hsv = {h, BRANCHLESS_MIN(s, transition_s), BRANCHLESS_MIN(BRANCHLESS_MAX(v, transition_vin), transition_vout)};
    RGB rgb = hsv_to_rgb(hsv);

    rgb_matrix_set_color(index, rgb.r, rgb.g, rgb.b);

    // Turn any key that is pressed white
    if(keys_down[index]) {
        hsv.s = 0;
        hsv.v = rgb_matrix_get_val();
        rgb   = hsv_to_rgb(hsv);
        rgb_matrix_set_color(index, rgb.r, rgb.g, rgb.b);
    }
}


void update_status_leds(void) {
    for(unsigned int i = 0; i < 9; i++) {
        uint8_t val = status_leds[i] * rgb_matrix_get_val() / 255;
        rgb_matrix_set_color(i, val, val, val);
    }

    rgb_matrix_set_color(9, 0, 0, 0);
    rgb_matrix_set_color(10, 0, 0, 0);
}


void display_serv_boot_1_for_side_kb(void) {
    status_leds[3] = 0xff;
    status_leds[5] = 0xff;
    status_leds[7] = 0xff;
}


void display_serv_boot_2_for_side_kb(void) {
    display_serv_boot_1_for_side_kb();

    status_leds[0] = 0xff;
    status_leds[2] = 0xff;
    status_leds[4] = 0xff;
}


void display_serv_boot_3_for_side_kb(void) {
    display_serv_boot_2_for_side_kb();

    status_leds[1] = 0xff;
}


// Called when the keyboard is first plugged in
void keyboard_post_init_user(void) { rgb_matrix_mode_noeeprom(1); }


void reset_to_matrix_1(void) {
    while(rgb_matrix_get_mode() != 1) {
        rgb_matrix_step_reverse();
    }
}


void reset_to_hue_0(void) {
    while(rgb_matrix_get_hue() != 0) {
        rgb_matrix_decrease_hue();
    }
}
