#pragma once

#ifdef RGB_MATRIX_ENABLE
#    define RGB_MATRIX_KEYPRESSES
#endif

#define RAW_USAGE_PAGE 0xFF66  // added to seprarte calls from top and sides
#define RAW_USAGE_ID 0x66      // added to seprarte calls from top and sides

// retain brightness level between light layers
#define RGBLIGHT_LAYERS_RETAIN_VAL
