#include "keyboard_layout.h"

const keyboard_layout_t keyboard_layout_us = {
    .name = "US QWERTY",

    .keys = {
        [KEY_A] = {
            .normal = 'a',
            .shift = 'A'
        },

        [KEY_B] = {
            .normal = 'b',
            .shift = 'B'
        },

        [KEY_C] = {
            .normal = 'c',
            .shift = 'C'
        },

        [KEY_D] = {
            .normal = 'd',
            .shift = 'D'
        },

        [KEY_E] = {
            .normal = 'e',
            .shift = 'E'
        },

        [KEY_F] = {
            .normal = 'f',
            .shift = 'F'
        },

        [KEY_G] = {
            .normal = 'g',
            .shift = 'G'
        },

        [KEY_H] = {
            .normal = 'h',
            .shift = 'H'
        },

        [KEY_I] = {
            .normal = 'i',
            .shift = 'I'
        },

        [KEY_J] = {
            .normal = 'j',
            .shift = 'J'
        },

        [KEY_K] = {
            .normal = 'k',
            .shift = 'K'
        },

        [KEY_L] = {
            .normal = 'l',
            .shift = 'L'
        },

        [KEY_M] = {
            .normal = 'm',
            .shift = 'M'
        },

        [KEY_N] = {
            .normal = 'n',
            .shift = 'N'
        },

        [KEY_O] = {
            .normal = 'o',
            .shift = 'O'
        },

        [KEY_P] = {
            .normal = 'p',
            .shift = 'P'
        },

        [KEY_Q] = {
            .normal = 'q',
            .shift = 'Q'
        },

        [KEY_R] = {
            .normal = 'r',
            .shift = 'R'
        },

        [KEY_S] = {
            .normal = 's',
            .shift = 'S'
        },

        [KEY_T] = {
            .normal = 't',
            .shift = 'T'
        },

        [KEY_U] = {
            .normal = 'u',
            .shift = 'U'
        },

        [KEY_V] = {
            .normal = 'v',
            .shift = 'V'
        },

        [KEY_W] = {
            .normal = 'w',
            .shift = 'W'
        },

        [KEY_X] = {
            .normal = 'x',
            .shift = 'X'
        },

        [KEY_Y] = {
            .normal = 'y',
            .shift = 'Y'
        },

        [KEY_Z] = {
            .normal = 'z',
            .shift = 'Z'
        },

        [KEY_1] = {
            .normal = '1',
            .shift = '!'
        },

        [KEY_2] = {
            .normal = '2',
            .shift = '@'
        },

        [KEY_3] = {
            .normal = '3',
            .shift = '#'
        },

        [KEY_4] = {
            .normal = '4',
            .shift = '$'
        },

        [KEY_5] = {
            .normal = '5',
            .shift = '%'
        },

        [KEY_6] = {
            .normal = '6',
            .shift = '^'
        },

        [KEY_7] = {
            .normal = '7',
            .shift = '&'
        },

        [KEY_8] = {
            .normal = '8',
            .shift = '*'
        },

        [KEY_9] = {
            .normal = '9',
            .shift = '('
        },

        [KEY_0] = {
            .normal = '0',
            .shift = ')'
        },

        [KEY_SPACE] = {
            .normal = ' ',
            .shift = ' '
        },

        [KEY_MINUS] = {
            .normal = '-',
            .shift = '_'
        },

        [KEY_EQUAL] = {
            .normal = '=',
            .shift = '+'
        },

        [KEY_COMMA] = {
            .normal = ',',
            .shift = '<'
        },

        [KEY_DOT] = {
            .normal = '.',
            .shift = '>'
        },

        [KEY_SLASH] = {
            .normal = '/',
            .shift = '?'
        },

        [KEY_SEMICOLON] = {
            .normal = ';',
            .shift = ':'
        },

        [KEY_APOSTROPHE] = {
            .normal = '\'',
            .shift = '"'
        },

        [KEY_GRAVE] = {
            .normal = '`',
            .shift = '~'
        },

        [KEY_LEFT_BRACKET] = {
            .normal = '[',
            .shift = '{'
        },

        [KEY_RIGHT_BRACKET] = {
            .normal = ']',
            .shift = '}'
        },

        [KEY_BACKSLASH] = {
            .normal = '\\',
            .shift = '|'
        },
    }
};

char keyboard_layout_translate(
    const keyboard_layout_t *layout,
    keycode_t key,
    const keyboard_state_t *state
){
    if (key >= KEY_COUNT) {
        return '\0';
    }

    keyboard_layout_entry_t entry = layout->keys[key];

    if (entry.normal == '\0') {
        return '\0';
    }

    bool shifted =
        state->left_shift ||
        state->right_shift;

    if (shifted) {
        return entry.shift;
    }


    return entry.normal;
}