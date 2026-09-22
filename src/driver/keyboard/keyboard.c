#include "keyboard.h"
#include "ps2.h"
#include "utils.h"

#include "keyboard_layout.h"

#include "stdio.h"

static const keyboard_layout_t *active_layout;

void init_keyboard(void) {
    reset_keyboard();
    set_scanning(false);
    select_scancode_set(0x02);
    set_scanning(true);

    ps2_flush_output();

    keyboard_set_layout(&keyboard_layout_us);
}

void keyboard_set_layout(const keyboard_layout_t *layout) { active_layout = layout; }

char keyboard_translate(keycode_t key, const keyboard_state_t *state) {
    return keyboard_layout_translate(active_layout, key, state);
}

void reset_keyboard(void) {
    ps2_wait_input_clear();
    ps2_write(PS2_DC_RESET);

    ps2_wait_output_full();

    if (ps2_read() != K_ACK) {
        printf("\033[31mERROR:\033[0m Keyboard did not acknowledge reset.\n");
        return;
    }

    ps2_wait_output_full();

    if (ps2_read() != k_BAT) {
        printf("\033[31mERROR:\033[0m Keyboard BAT failed.\n");
        return;
    }
}

void set_scanning(bool state) {
    ps2_flush_output();
    if (!ps2_wait_input_clear()) {
        printf("\033[31mERROR:\033[0m Keyboard input timeout while toggling scanning.\n");
        return;
    }

    if (state) {
        ps2_write(PS2_DC_ENABLE_SCANNING);
    } else {
        ps2_write(PS2_DC_DISABLE_SCANNING);
    }

    if (!ps2_wait_output_full()) {
        printf("\033[31mERROR:\033[0m Keyboard scan ack timeout.\n");
        return;
    }

    if (ps2_read() != K_ACK) {
        printf("\033[31mERROR:\033[0m Keyboard did not acknowledge scanning command.\n");
    }
}

void set_scaning(bool state) { set_scanning(state); }

void select_scancode_set(uint8_t set) {
    if (set > 3) {
        printf("\033[31mERROR:\033[0m Invalid scancode set %u.\n", set);
        return;
    }

    ps2_flush_output();
    if (!ps2_wait_input_clear()) {
        printf("\033[31mERROR:\033[0m Keyboard input timeout before scancode set select.\n");
        return;
    }

    ps2_write(PS2_DC_SELECT_SET);

    if (!ps2_wait_output_full()) {
        printf("\033[31mERROR:\033[0m Keyboard ack timeout while selecting scancode set.\n");
        return;
    }

    if (ps2_read() != K_ACK) {
        printf("\033[31mERROR:\033[0m keyboard.h:53 select_scancode_set .\n");
        return;
    }

    ps2_flush_output();
    if (!ps2_wait_input_clear()) {
        printf("\033[31mERROR:\033[0m Keyboard input timeout before scancode value.\n");
        return;
    }

    ps2_write(set);

    if (!ps2_wait_output_full()) {
        printf("\033[31mERROR:\033[0m Keyboard ack timeout after scancode value.\n");
        return;
    }

    if (ps2_read() != K_ACK) {
        printf("\033[31mERROR:\033[0m keyboard.h:65 select_scancode_set .\n");
    }
}
