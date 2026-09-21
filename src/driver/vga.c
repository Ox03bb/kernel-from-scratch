#include "vga.h"
#include "utils.h"

static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;
static uint16_t cursor_position = 0;
static uint8_t current_color = LIGHT_GRAY;
static volatile uint16_t *vga_buf;

void vga_init(void) {
    cursor_x = 0;
    cursor_y = 0;
    cursor_position = 0;
    current_color = LIGHT_GRAY;

    vga_buf = (volatile uint16_t *)VGA_ADDRESS;

    vga_clear();

    vga_set_cursor(0, 0);
    vga_cursor_enable();
}


// helper functions 

void vga_cursor_carriage_return(void)
{
    uint16_t column = cursor_position % VGA_WIDTH;

    cursor_position -= column;

    vga_set_cursor_position(cursor_position);
}


void vga_cursor_newline(void)
{
    uint16_t row = cursor_position / VGA_WIDTH;

    row++;

    if (row >= VGA_HEIGHT) {
        vga_scroll_up();
        row = VGA_HEIGHT - 1;
    }

    cursor_position = row * VGA_WIDTH;

    vga_set_cursor_position(cursor_position);
}


void vga_cursor_tab(void)
{
    uint16_t column = cursor_position % VGA_WIDTH;

    uint16_t spaces =
        VGA_TAB_WIDTH - (column % VGA_TAB_WIDTH);

    for (uint16_t i = 0; i < spaces; i++) {

        if (cursor_position >= VGA_WIDTH * VGA_HEIGHT) {
            vga_scroll_up();

            cursor_position =
                (VGA_HEIGHT - 1) * VGA_WIDTH;
        }

        vga_buf[cursor_position] =
            ((uint16_t)current_color << 8) | ' ';

        cursor_position++;
    }

    vga_set_cursor_position(cursor_position);
}


void vga_cursor_backspace(void)
{
    if (cursor_position == 0)
        return;

    cursor_position--;

    vga_buf[cursor_position] =
        ((uint16_t)current_color << 8) | ' ';

    vga_set_cursor_position(cursor_position);
}

// Print

void vga_print_char(char c)
{
    switch (c) {

    case '\n':
        vga_cursor_newline();
        break;

    case '\r':
        vga_cursor_carriage_return();
        break;

    case '\t':
        vga_cursor_tab();
        break;

    case '\b':
        vga_cursor_backspace();
        break;

    default:
        vga_buf[cursor_position] =
            ((uint16_t)current_color << 8) | (uint8_t)c;

        vga_set_cursor_position(cursor_position + 1);
        break;
    }
}
void vga_print(const char *str) {
    if (str == 0) {
        return;
    }

    for (uint32_t i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') {
            uint8_t y = vga_get_cursor_y() + 1;
            if (y >= VGA_HEIGHT) {
                vga_scroll_up();
            } else {
                vga_set_cursor(0, y);
            }

        } else {
            vga_print_char(str[i]);
        }
    }
}

void vga_print_hex(uint32_t value) {
    const char hex[] = "0123456789ABCDEF";

    vga_print("0x");

    for (int i = 7; i >= 0; i--) {
        uint8_t digit = (value >> (i * 4)) & 0xF;
        vga_print_char(hex[digit]);
    }
}

void vga_set_color(uint8_t foreground, uint8_t background) {
    current_color = (background << 4 | foreground);
}

// Cursor Enabling
void vga_cursor_enable(void) {
    outb(VGA_INDEX_PORT, VGA_CURSOR_START);
    outb(VGA_DATA_PORT, 0x06);

    outb(VGA_INDEX_PORT, VGA_CURSOR_END);
    outb(VGA_DATA_PORT, 0x07);
}

void vga_cursor_disable(void) {
    outb(VGA_INDEX_PORT, VGA_CURSOR_START);
    outb(VGA_DATA_PORT, 0x20);
}

// Cursor position

void vga_set_cursor(uint8_t x, uint8_t y) {
    uint16_t p = y * VGA_WIDTH + x;

    outb(VGA_INDEX_PORT, VGA_CURSOR_HIGH);
    outb(VGA_DATA_PORT, (p >> 8));

    outb(VGA_INDEX_PORT, VGA_CURSOR_LOW);
    outb(VGA_DATA_PORT, p & 0xff);

    cursor_x = x;
    cursor_y = y;
    cursor_position = p;
}

void vga_set_cursor_position(uint16_t p) {

    outb(VGA_INDEX_PORT, VGA_CURSOR_HIGH);
    outb(VGA_DATA_PORT, (p >> 8));

    outb(VGA_INDEX_PORT, VGA_CURSOR_LOW);
    outb(VGA_DATA_PORT, p & 0xff);

    cursor_position = p;
    cursor_x = p % VGA_WIDTH;
    cursor_y = p / VGA_WIDTH;
}

uint16_t vga_get_cursor_position(void) {

    outb(VGA_INDEX_PORT, VGA_CURSOR_HIGH);
    uint8_t p1 = inb(VGA_DATA_PORT);

    outb(VGA_INDEX_PORT, VGA_CURSOR_LOW);
    uint8_t p2 = inb(VGA_DATA_PORT);

    return ((uint16_t)p1 << 8) | p2;
}

uint8_t vga_get_cursor_x(void) { return vga_get_cursor_position() % VGA_WIDTH; }

uint8_t vga_get_cursor_y(void) { return vga_get_cursor_position() / VGA_WIDTH; }

uint8_t vga_calc_cursor_x(uint16_t pos) { return pos % VGA_WIDTH; }

uint8_t vga_calc_cursor_y(uint16_t pos) { return pos / VGA_WIDTH; }

uint16_t vga_calc_cursor_position(uint8_t x, uint8_t y) { return y * VGA_WIDTH + x; }

// Clear

void vga_clear(void) {

    vga_set_cursor(0, 0);

    for (int i = 0; i < VGA_RES; i++) {
        vga_buf[i] = ((uint16_t)0 << 8) | ' ';
    }

    vga_set_cursor(0, 0);
}

// secrole

void vga_scroll_up(void) {
    /* Move every row one line upward */
    for (uint16_t y = 0; y < VGA_HEIGHT - 1; y++) {

        for (uint16_t x = 0; x < VGA_WIDTH; x++) {

            uint16_t dst = y * VGA_WIDTH + x;
            uint16_t src = (y + 1) * VGA_WIDTH + x;

            vga_buf[dst] = vga_buf[src];
        }
    }

    /* Clear the new bottom line */
    for (uint16_t x = 0; x < VGA_WIDTH; x++) {

        uint16_t pos = (VGA_HEIGHT - 1) * VGA_WIDTH + x;

        vga_buf[pos] = ((uint16_t)current_color << 8) | ' ';
    }

    vga_set_cursor(0, VGA_HEIGHT - 1);
}


