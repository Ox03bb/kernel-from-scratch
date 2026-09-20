#include "keyboard.h"
#include "ps2.h"
#include "utils.h"

#include "stdio.h"

void init_keyboard(void) {
    reset_keyboard();
    set_scaning(false);
    select_scancode_set(0x02);
    set_scaning(true);
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

void set_scaning(bool state) {
    ps2_flush_output();
    ps2_wait_input_clear();

    if (state) {
        ps2_write(PS2_DC_ENABLE_SCANNING);
    } else {
        ps2_write(PS2_DC_DISABLE_SCANNING);
    }

    ps2_wait_output_full();

    if (ps2_read() != K_ACK) {
        printf("\033[31mERROR:\033[0m Keyboard did not acknowledge scanning command.\n");
        return;
    }
}

void select_scancode_set(uint8_t set) {
    ps2_flush_output();
    ps2_wait_input_clear();

    if (set <= 3) {
        ps2_write(PS2_DC_SELECT_SET);
    }

    ps2_wait_output_full();

    if (ps2_read() != K_ACK) {
        printf("\033[31mERROR:\033[0m keyboard.h:53 select_scancode_set .\n");
        return;
    }

    ps2_flush_output();
    ps2_wait_input_clear();

    ps2_write(set);

    ps2_wait_output_full();

    if (ps2_read() != K_ACK) {
        printf("\033[31mERROR:\033[0m keyboard.h:65 select_scancode_set .\n");
        return;
    }
}

void keyboard_irq_handler(void) {
    printf("KEYBOARD IRQ!\n");

    uint8_t data = ps2_read();

    printf("SCAN: %d\n", data);
}