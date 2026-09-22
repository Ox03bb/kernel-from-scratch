#ifndef VGA_H
#define VGA_H

#include "types.h"

#define VGA_ADDRESS 0xB8000

#define VGA_WIDTH     80
#define VGA_HEIGHT    25
#define VGA_RES       (VGA_HEIGHT * VGA_WIDTH)
#define VGA_TAB_WIDTH 4

#define VGA_INDEX_PORT 0x3D4
#define VGA_DATA_PORT  0x3D5

#define VGA_CURSOR_HIGH 0x0E
#define VGA_CURSOR_LOW  0x0F

#define VGA_CURSOR_START 0x0A
#define VGA_CURSOR_END   0x0B

#define BLACK      0x00 // DARK_GRAY
#define BLUE       0x01 // LIGHT_BLUE
#define GREEN      0x02 // LIGHT_GREEN
#define CYAN       0x03 // LIGHT_CYAN
#define RED        0x04 // LIGHT_RED
#define MAGENTA    0x05 // LIGHT_MAGENTA
#define BROWN      0x06 // YELLOW
#define LIGHT_GRAY 0x07 // WHITE

#define DARK_GRAY     BLACK | LIGHT_FLAG
#define LIGHT_BLUE    BLUE | LIGHT_FLAG
#define LIGHT_GREEN   GREEN | LIGHT_FLAG
#define LIGHT_CYAN    CYAN | LIGHT_FLAG
#define LIGHT_RED     RED | LIGHT_FLAG
#define LIGHT_MAGENTA MAGENTA | LIGHT_FLAG
#define YELLOW        BROWN | LIGHT_FLAG
#define WHITE         LIGHT_GRAY | LIGHT_FLAG

#define LIGHT_FLAG 0x08

#define DEFUALT_F LIGHT_GRAY
#define DEFUALT_B BLACK

typedef union vga_entry {
    uint16_t value;

    struct {
        uint8_t character;
        uint8_t attribute;
    };
} vga_entry;

void vga_init(void);

void vga_clear(void);

void vga_scroll_up(void);

void vga_print_char(char c);

void vga_print(const char *str);

void vga_print_hex(uint32_t value);

void vga_scan_char(char c);

void vga_scan(const char *str);

void vga_set_color(uint8_t foreground, uint8_t background);

void vga_cursor_enable(void);

void vga_cursor_disable(void);

void vga_set_cursor(uint8_t x, uint8_t y);

void vga_set_cursor_position(uint16_t p);

uint16_t vga_get_cursor_position(void);

uint8_t vga_get_cursor_x(void);

uint8_t vga_get_cursor_y(void);

void vga_put_at(uint8_t x, uint8_t y, char c, uint8_t color);
void vga_clear_region(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color);

void vga_print_at(uint8_t x, uint8_t y, const char *str, uint8_t color);
void vga_print_at_center(uint8_t y, const char *str, uint8_t color);

#endif