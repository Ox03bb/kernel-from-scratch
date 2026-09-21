#ifndef keyboard_keys_H
#define keyboard_keys_H

#include "ps2.h"
#include "types.h"

typedef enum {
    SCAN_STATE_NORMAL,
    SCAN_STATE_BREAK,
    SCAN_STATE_EXTENDED,
    SCAN_STATE_EXTENDED_BREAK
} scancode_state_t;

typedef enum {
    KEY_PRESSED,
    KEY_RELEASED
} key_action_t;

typedef struct {
    keycode_t key;
    key_action_t action;
} key_event_t;


typedef enum {
    KEY_NONE = 0,

    /* Function keys */
    KEY_ESC,

    KEY_F1,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_F9,
    KEY_F10,
    KEY_F11,
    KEY_F12,

    /* Number row */
    KEY_GRAVE, /* ` ~ */
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,
    KEY_0,
    KEY_MINUS, /* - _ */
    KEY_EQUAL, /* = + */
    KEY_BACKSPACE,

    /* First row */
    KEY_TAB,
    KEY_Q,
    KEY_W,
    KEY_E,
    KEY_R,
    KEY_T,
    KEY_Y,
    KEY_U,
    KEY_I,
    KEY_O,
    KEY_P,
    KEY_LEFT_BRACKET,  /* [ { */
    KEY_RIGHT_BRACKET, /* ] } */
    KEY_BACKSLASH,     /* \ | */

    /* Second row */
    KEY_CAPS_LOCK,
    KEY_A,
    KEY_S,
    KEY_D,
    KEY_F,
    KEY_G,
    KEY_H,
    KEY_J,
    KEY_K,
    KEY_L,
    KEY_SEMICOLON,  /* ; : */
    KEY_APOSTROPHE, /* ' " */
    KEY_ENTER,

    /* Third row */
    KEY_LEFT_SHIFT,
    KEY_Z,
    KEY_X,
    KEY_C,
    KEY_V,
    KEY_B,
    KEY_N,
    KEY_M,
    KEY_COMMA, /* , < */
    KEY_DOT,   /* . > */
    KEY_SLASH, /* / ? */
    KEY_RIGHT_SHIFT,

    /* Bottom row */
    KEY_LEFT_CTRL,
    KEY_LEFT_GUI,
    KEY_LEFT_ALT,
    KEY_SPACE,
    KEY_RIGHT_ALT,
    KEY_RIGHT_GUI,
    KEY_MENU,
    KEY_RIGHT_CTRL,

    /* Navigation */
    KEY_INSERT,
    KEY_DELETE,
    KEY_HOME,
    KEY_END,
    KEY_PAGE_UP,
    KEY_PAGE_DOWN,

    /* Arrow keys */
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,

    /* Numeric keypad */
    KEY_NUM_LOCK,

    KEY_KP_DIVIDE,
    KEY_KP_MULTIPLY,
    KEY_KP_MINUS,
    KEY_KP_PLUS,
    KEY_KP_ENTER,

    KEY_KP_0,
    KEY_KP_1,
    KEY_KP_2,
    KEY_KP_3,
    KEY_KP_4,
    KEY_KP_5,
    KEY_KP_6,
    KEY_KP_7,
    KEY_KP_8,
    KEY_KP_9,

    KEY_KP_DOT,

    /* Lock keys */
    KEY_SCROLL_LOCK,

    /* Special / system keys */
    KEY_PRINT_SCREEN,
    KEY_PAUSE,

    /* Additional common keys */
    KEY_POWER,
    KEY_SLEEP,
    KEY_WAKE,

    KEY_VOLUME_UP,
    KEY_VOLUME_DOWN,
    KEY_MUTE,

    KEY_MEDIA_PLAY_PAUSE,
    KEY_MEDIA_STOP,
    KEY_MEDIA_PREVIOUS,
    KEY_MEDIA_NEXT,

    KEY_CALCULATOR,

    KEY_COUNT
} keycode_t;

typedef struct {
    keycode_t normal[256];
    keycode_t extended[256];
} keyboard_scancode_table_t;

static const keyboard_scancode_table_t scancode_set2 = {.normal =
                                                            {

                                                                [0x76] = KEY_ESC,

                                                                [0x05] = KEY_F1,
                                                                [0x06] = KEY_F2,
                                                                [0x04] = KEY_F3,
                                                                [0x0C] = KEY_F4,
                                                                [0x03] = KEY_F5,
                                                                [0x0B] = KEY_F6,
                                                                [0x83] = KEY_F7,
                                                                [0x0A] = KEY_F8,
                                                                [0x01] = KEY_F9,
                                                                [0x09] = KEY_F10,
                                                                [0x78] = KEY_F11,
                                                                [0x07] = KEY_F12,

                                                                [0x0E] = KEY_GRAVE,

                                                                [0x16] = KEY_1,
                                                                [0x1E] = KEY_2,
                                                                [0x26] = KEY_3,
                                                                [0x25] = KEY_4,
                                                                [0x2E] = KEY_5,
                                                                [0x36] = KEY_6,
                                                                [0x3D] = KEY_7,
                                                                [0x3E] = KEY_8,
                                                                [0x46] = KEY_9,
                                                                [0x45] = KEY_0,

                                                                [0x4E] = KEY_MINUS,
                                                                [0x55] = KEY_EQUAL,

                                                                [0x66] = KEY_BACKSPACE,

                                                                [0x0D] = KEY_TAB,

                                                                [0x15] = KEY_Q,
                                                                [0x1D] = KEY_W,
                                                                [0x24] = KEY_E,
                                                                [0x2D] = KEY_R,
                                                                [0x2C] = KEY_T,
                                                                [0x35] = KEY_Y,
                                                                [0x3C] = KEY_U,
                                                                [0x43] = KEY_I,
                                                                [0x44] = KEY_O,
                                                                [0x4D] = KEY_P,

                                                                [0x54] = KEY_LEFT_BRACKET,
                                                                [0x5B] = KEY_RIGHT_BRACKET,
                                                                [0x5D] = KEY_BACKSLASH,

                                                                [0x58] = KEY_CAPS_LOCK,

                                                                [0x1C] = KEY_A,
                                                                [0x1B] = KEY_S,
                                                                [0x23] = KEY_D,
                                                                [0x2B] = KEY_F,
                                                                [0x34] = KEY_G,
                                                                [0x33] = KEY_H,
                                                                [0x3B] = KEY_J,
                                                                [0x42] = KEY_K,
                                                                [0x4B] = KEY_L,

                                                                [0x4C] = KEY_SEMICOLON,
                                                                [0x52] = KEY_APOSTROPHE,

                                                                [0x5A] = KEY_ENTER,

                                                                [0x12] = KEY_LEFT_SHIFT,

                                                                [0x1A] = KEY_Z,
                                                                [0x22] = KEY_X,
                                                                [0x21] = KEY_C,
                                                                [0x2A] = KEY_V,
                                                                [0x32] = KEY_B,
                                                                [0x31] = KEY_N,
                                                                [0x3A] = KEY_M,

                                                                [0x41] = KEY_COMMA,
                                                                [0x49] = KEY_DOT,
                                                                [0x4A] = KEY_SLASH,

                                                                [0x59] = KEY_RIGHT_SHIFT,

                                                                [0x14] = KEY_LEFT_CTRL,
                                                                [0x11] = KEY_LEFT_ALT,

                                                                [0x29] = KEY_SPACE,

                                                                [0x77] = KEY_NUM_LOCK,

                                                                [0x7C] = KEY_KP_MULTIPLY,
                                                                [0x7B] = KEY_KP_MINUS,
                                                                [0x79] = KEY_KP_PLUS,

                                                                [0x70] = KEY_KP_0,
                                                                [0x69] = KEY_KP_1,
                                                                [0x72] = KEY_KP_2,
                                                                [0x7A] = KEY_KP_3,
                                                                [0x6B] = KEY_KP_4,
                                                                [0x73] = KEY_KP_5,
                                                                [0x74] = KEY_KP_6,
                                                                [0x6C] = KEY_KP_7,
                                                                [0x75] = KEY_KP_8,
                                                                [0x7D] = KEY_KP_9,
                                                                [0x71] = KEY_KP_DOT,

                                                                [0x7E] = KEY_SCROLL_LOCK,
                                                            },

                                                        .extended = {

                                                            /* Modifiers */
                                                            [0x14] = KEY_RIGHT_CTRL,
                                                            [0x11] = KEY_RIGHT_ALT,

                                                            /* Navigation */
                                                            [0x70] = KEY_INSERT,
                                                            [0x71] = KEY_DELETE,
                                                            [0x6C] = KEY_HOME,
                                                            [0x69] = KEY_END,
                                                            [0x7D] = KEY_PAGE_UP,
                                                            [0x7A] = KEY_PAGE_DOWN,

                                                            /* Arrows */
                                                            [0x75] = KEY_UP,
                                                            [0x72] = KEY_DOWN,
                                                            [0x6B] = KEY_LEFT,
                                                            [0x74] = KEY_RIGHT,

                                                            /* Keypad */
                                                            [0x4A] = KEY_KP_DIVIDE,
                                                            [0x5A] = KEY_KP_ENTER,

                                                            /* GUI keys */
                                                            [0x1F] = KEY_LEFT_GUI,
                                                            [0x27] = KEY_RIGHT_GUI,
                                                            [0x2F] = KEY_MENU,

                                                            /* Print Screen */
                                                            [0x12] = KEY_PRINT_SCREEN,
                                                        }};

#endif