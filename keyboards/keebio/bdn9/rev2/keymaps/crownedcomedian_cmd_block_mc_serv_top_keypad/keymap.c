#include "../shared.c"

// Override LED positions for TOP keypad (above front keypad)
#ifdef RGB_MATRIX_ENABLE
led_config_t g_led_config = {
    {
        // Key Matrix to LED Index (same as default)
        { 0, 1, 2 },
        { 3, 4, 5 },
        { 6, 7, 8 }
    }, {
        // LED Index to Physical Position (TOP keypad is upside-down)
        { 140, 26 }, { 112, 26 }, { 84, 26 },
        { 140, 13 }, { 112, 13 }, { 84, 13 },
        { 140, 0 }, { 112, 0 }, { 84, 0 },
                { 126, 0 }, { 98, 0 },       // Underglow LEDs (not used)
    }, {
        // LED Index to Flag (same as default)
        4, 4, 4,
        4, 4, 4,
        4, 4, 4,
          2, 2
    }
};
#endif

enum my_keycodes {
    MINECRAFT_KEY_8 = SAFE_RANGE,
    MINECRAFT_KEY_9,
    MINECRAFT_KEY_10,
    MINECRAFT_KEY_11,
    MINECRAFT_KEY_12,
    MINECRAFT_KEY_13,
    MINECRAFT_KEY_14
};


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
        MINECRAFT_KEY_8,    MINECRAFT_KEY_9,   MINECRAFT_KEY_10,
        MINECRAFT_KEY_11,   MINECRAFT_KEY_12,  MINECRAFT_KEY_13,
        KC_C,               MINECRAFT_KEY_14,  KC_D
    )
};


// Brightness values are shuffled from other keymaps
#define VAL0 brightness(24, 9, 19, 120, 180)
#define VAL1 brightness(50, 0, 13, 160, 135)
#define VAL2 brightness(25, 2, 19, 100, 190)
#define VAL3 brightness(30, 20, 16, 40, 194)
#define VAL4 brightness(27, 8, 7, 80, 200)
#define VAL5 brightness(25, 2, 19, 100, 190)
//#define VAL6 brightness(60, 20, 0, 20, 192)
#define VAL7 brightness(45, 16, 0, 140, 160)
//#define VAL8 brightness(50, 0, 13, 160, 135)


// Hue and Saturation values are shuffled between keymaps
void update_leds(void) {
    update_led(0, 3, 230, VAL0);
    update_led(1, 64, 245, VAL1);
    update_led(2, 21, 254, VAL2);
    update_led(3, 2, 253, VAL3);
    update_led(4, 2, 235, VAL4);
    update_led(5, 8, 230, VAL5);
    // update_led(6, 2, 253, VAL6);
    rgb_matrix_set_color(6, 0, 0, 0);
    update_led(7, 0, 253, VAL7);
    // update_led(8, 3, 230, VAL8);
    rgb_matrix_set_color(8, 0, 0, 0);
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
                transition_s   = 0;
                transition_off = false;

                for (unsigned int i = 0; i < 9; i++) {
                    status_leds[i] = 0x00;
                }
            }
        }
    }
}


// Called when a user presses/releases a key
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch(keycode) {
        case MINECRAFT_KEY_8:
            keys_down[0] = record->event.pressed;

            if(record->event.pressed) {
                raw_hid_send(MSG_MINECRAFT_KEY_8, HID_MSG_LEN);
            }

            return true;
        case MINECRAFT_KEY_9:
            keys_down[1] = record->event.pressed;

            if(record->event.pressed) {
                raw_hid_send(MSG_MINECRAFT_KEY_9, HID_MSG_LEN);
            }

            return true;
        case MINECRAFT_KEY_10:
            keys_down[2] = record->event.pressed;

            if(record->event.pressed) {
                raw_hid_send(MSG_MINECRAFT_KEY_10, HID_MSG_LEN);
            }

            return true;
        case MINECRAFT_KEY_11:
            keys_down[3] = record->event.pressed;

            if(record->event.pressed) {
                raw_hid_send(MSG_MINECRAFT_KEY_11, HID_MSG_LEN);
            }

            return true;
        case MINECRAFT_KEY_12:
            keys_down[4] = record->event.pressed;

            if(record->event.pressed) {
                raw_hid_send(MSG_MINECRAFT_KEY_12, HID_MSG_LEN);
            }

            return true;
        case MINECRAFT_KEY_13:
            keys_down[5] = record->event.pressed;

            if(record->event.pressed) {
                raw_hid_send(MSG_MINECRAFT_KEY_13, HID_MSG_LEN);
            }

            return true;
        case MINECRAFT_KEY_14:
            keys_down[7] = record->event.pressed;

            if(record->event.pressed) {
                raw_hid_send(MSG_MINECRAFT_KEY_14, HID_MSG_LEN);
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
        raw_hid_send(MSG_TOP_LINK, HID_MSG_LEN);
    }

    if(strstr((char *)data, READ_MSG_SERV_BOOT_1) != NULL) {
        display_serv_boot_1_for_side_kb();
    }

    if(strstr((char *)data, READ_MSG_SERV_BOOT_2) != NULL) {
        display_serv_boot_2_for_side_kb();
    }

    if(strstr((char *)data, READ_MSG_SERV_BOOT_3) != NULL) {
        display_serv_boot_3_for_side_kb();
    }

    if(strstr((char *)data, READ_MSG_SERV_BOOT_DONE) != NULL) {
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

    if(strstr((char *)data, "RGB LED mode +") != NULL) {
        rgb_matrix_step();
    }

    if(strstr((char *)data, "RGB LED mode -") != NULL) {
        rgb_matrix_step_reverse();
    }

    if(strstr((char *)data, "RGB LED hue +") != NULL) {
        rgb_matrix_increase_hue();
    }

    if(strstr((char *)data, "RGB LED hue -") != NULL) {
        rgb_matrix_decrease_hue();
    }

    if(strstr((char *)data, "RGB LED saturation +") != NULL) {
        rgb_matrix_increase_sat();
    }

    if(strstr((char *)data, "RGB LED saturation -") != NULL) {
        rgb_matrix_decrease_sat();
    }

    if(strstr((char *)data, "RGB LED value +") != NULL) {
        rgb_matrix_increase_val();
    }

    if(strstr((char *)data, "RGB LED value -") != NULL) {
        rgb_matrix_decrease_val();
    }

    // Shutdown
    if(strstr((char *)data, READ_MSG_SERV_SHUTDOWN) != NULL) {
        // Fade out rgbs
        transition_off = true;
    }
}


// Called when the rgb matrix animation mode changes
bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    // Force unused corner LEDs to be off for all RGB matrix modes
    rgb_matrix_set_color(6, 0, 0, 0);
    rgb_matrix_set_color(8, 0, 0, 0);

    // Force underglow LEDs to be off for all RGB matrix modes
    rgb_matrix_set_color(9, 0, 0, 0);
    rgb_matrix_set_color(10, 0, 0, 0);

    return false;
}
