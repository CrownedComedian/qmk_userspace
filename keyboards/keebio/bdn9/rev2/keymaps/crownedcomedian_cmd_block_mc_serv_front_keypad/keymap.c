#include "../shared.c"

// Override LED positions for FRONT keypad (center of virtual matrix)
#ifdef RGB_MATRIX_ENABLE
led_config_t g_led_config = {
    {
        // Key Matrix to LED Index (same as default)
        { 0, 1, 2 },
        { 3, 4, 5 },
        { 6, 7, 8 }
    }, {
        // LED Index to Physical Position (FRONT keypad is upside-down)
        { 140, 64 }, { 112, 64 }, { 84, 64 },
        { 140, 51 }, { 112, 51 }, { 84, 51 },
        { 140, 38 }, { 112, 38 }, { 84, 38 },
                { 126, 38 }, { 98, 38 },       // Underglow LEDs (not used)
    }, {
        // LED Index to Flag (same as default)
        4, 4, 4,
        4, 4, 4,
        4, 4, 4,
          2, 2
    }
};
#endif

// Brightness values are shuffled from other keymaps
#define VAL0 brightness(24, 9, 19, 120, 180)
#define VAL1 brightness(30, 20, 16, 40, 194)
#define VAL2 brightness(25, 2, 19, 100, 190)
#define VAL3 brightness(30, 1, 30, 60, 190)
#define VAL4 brightness(45, 16, 0, 140, 160)
#define VAL5 brightness(50, 8, 0, 100, 150)
#define VAL6 brightness(60, 20, 0, 20, 192)
#define VAL7 brightness(27, 8, 7, 80, 200)
#define VAL8 brightness(50, 0, 13, 160, 135)

enum my_keycodes { MINECRAFT_KEY_0 = SAFE_RANGE };

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
        QK_RGB_MATRIX_MODE_NEXT,        QK_RGB_MATRIX_MODE_PREVIOUS,    QK_RGB_MATRIX_VALUE_DOWN,
        QK_RGB_MATRIX_SATURATION_UP,    MINECRAFT_KEY_0,                QK_RGB_MATRIX_VALUE_UP,
        QK_RGB_MATRIX_SATURATION_DOWN,  QK_RGB_MATRIX_HUE_UP,           QK_RGB_MATRIX_HUE_DOWN
    )
};

bool transition_kneed = true;
bool transition_on    = false;


// Hue and Saturation values are shuffled between keymaps
void update_leds(void) {
    update_led(0, 3, 230, VAL0);
    update_led(1, 0, 253, VAL1);
    update_led(2, 2, 245, VAL2);
    update_led(3, 2, 245, VAL3);
    update_led(4, 1, 200, VAL4);
    update_led(5, 21, 254, VAL5);
    update_led(6, 64, 245, VAL6);
    update_led(7, 2, 253, VAL7);
    update_led(8, 8, 230, VAL8);
    rgb_matrix_set_color(9, 0, 0, 0);
    rgb_matrix_set_color(10, 0, 0, 0);
}


// Called over and over
void matrix_scan_user(void) {
    if(rgb_matrix_get_mode() == 1) {
        frame++;

        if(transition_s) {
            update_leds();

            if(transition_s < 255) {
                transition_s++;
                transition_vin--;
            }
        } else {
            update_status_leds();
            transition_vin  = rgb_matrix_get_val();
            transition_vout = rgb_matrix_get_val();
        }

        if(transition_off) {
            transition_vout--;

            if(!transition_vout) {
                transition_s     = 0;
                transition_off   = false;
                transition_on    = false;
                transition_kneed = true;
                frame            = 0;

                for (unsigned int i = 0; i < 9; i++) {
                    status_leds[i] = 0x00;
                }
            }
        } else if(!transition_s && transition_kneed) {
            uint8_t val = brightness(127, 1, 0, 0, 127);

            if((!val && transition_on)) {
                transition_kneed = false;
            } else {
                rgb_matrix_set_color(4, val, val, val);
            }
        }
    }
}


// Called when a user presses/releases a key
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case MINECRAFT_KEY_0:
            keys_down[4] = record->event.pressed;

            if(record->event.pressed) {
                raw_hid_send(MSG_START_STOP, HID_MSG_LEN);
            }

            return true;
        case QK_RGB_MATRIX_MODE_NEXT:
            keys_down[0] = record->event.pressed;

            if(record->event.pressed) {
                raw_hid_send(MSG_RGB_MOD, HID_MSG_LEN);
            }

            return true;
        case QK_RGB_MATRIX_MODE_PREVIOUS:
            keys_down[1] = record->event.pressed;

            if(record->event.pressed) {
                raw_hid_send(MSG_RGB_RMOD, HID_MSG_LEN);
            }

            return true;
        case QK_RGB_MATRIX_HUE_UP:
            keys_down[7] = record->event.pressed;

            if(record->event.pressed) {
                raw_hid_send(MSG_RGB_HUI, HID_MSG_LEN);
            }

            return true;
        case QK_RGB_MATRIX_HUE_DOWN:
            keys_down[8] = record->event.pressed;

            if(record->event.pressed) {
                raw_hid_send(MSG_RGB_HUD, HID_MSG_LEN);
            }

            return true;
        case QK_RGB_MATRIX_SATURATION_UP:
            keys_down[3] = record->event.pressed;

            if(record->event.pressed) {
                raw_hid_send(MSG_RGB_SAI, HID_MSG_LEN);
            }

            return true;
        case QK_RGB_MATRIX_SATURATION_DOWN:
            keys_down[6] = record->event.pressed;

            if(record->event.pressed) {
                raw_hid_send(MSG_RGB_SAD, HID_MSG_LEN);
            }

            return true;
        case QK_RGB_MATRIX_VALUE_UP:
            keys_down[5] = record->event.pressed;

            if(record->event.pressed) {
                raw_hid_send(MSG_RGB_VAI, HID_MSG_LEN);
            }

            return true;
        case QK_RGB_MATRIX_VALUE_DOWN:
            keys_down[2] = record->event.pressed;

            if(record->event.pressed) {
                raw_hid_send(MSG_RGB_VAD, HID_MSG_LEN);
            }

            return true;
        default:
            return true;  // Process all other keycodes normally
    }
}


// Called whenever we recieve a message
void raw_hid_receive(uint8_t *data, uint8_t length) {
    if(strstr((char *)data, READ_MSG_LINK) != NULL) {
        reset_to_matrix_1();
        reset_to_hue_0();
        raw_hid_send(MSG_FRONT_LINK, HID_MSG_LEN);
    }

    if(strstr((char *)data, READ_MSG_SERV_BOOT_1) != NULL) {
        transition_on = true;
    }

    if(strstr((char *)data, READ_MSG_SERV_BOOT_3) != NULL) {
        status_leds[0] = 0xff;
        status_leds[2] = 0xff;
        status_leds[6] = 0xff;
        status_leds[8] = 0xff;
    }

    if(strstr((char *)data, READ_MSG_SERV_BOOT_4) != NULL) {
        status_leds[1] = 0xff;
        status_leds[3] = 0xff;
        status_leds[5] = 0xff;
        status_leds[7] = 0xff;
    }

    if(strstr((char *)data, READ_MSG_SERV_BOOT_DONE) != NULL) {
        status_leds[4] = 0xff;

        // Begin rgb transition to colors
        transition_s++;
        transition_vin  = 255;
        transition_vout = 255;
    }

    if(strstr((char *)data, READ_MSG_RELINK) != NULL) {
        frame           = 0;
        transition_s    = 1;
        transition_vin  = 255;
        transition_vout = 255;
        transition_off  = false;
    }

    // Shutdown
    if(strstr((char *)data, READ_MSG_SERV_SHUTDOWN) != NULL) {
        // Fade out rgbs
        transition_off = true;
    }
}


// Called when the rgb matrix animation mode changes
bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    // Force underglow LEDs to be off for all RGB matrix modes
    rgb_matrix_set_color(9, 0, 0, 0);
    rgb_matrix_set_color(10, 0, 0, 0);

    return false;
}
