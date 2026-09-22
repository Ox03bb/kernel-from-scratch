#ifndef VGA_LIB_H
#define VGA_LIB_H

#include "types.h"

/*
 * Logging
 */
void vga_log_info(const char *str0, const char *str, const char *str2);
void vga_log_success(const char *str0, const char *str, const char *str2);
void vga_log_warning(const char *str0, const char *str, const char *str2);
void vga_log_error(const char *str0, const char *str, const char *str2);

/*
 * Text utilities
 */
void vga_println(const char *str);
void vga_print_at_end(const char *str);
void vga_print_at_end_c(const char *str, uint8_t color);
void vga_print_line_at(uint8_t row, const char *str);
void vga_print_centered(const char *str, uint8_t row);

/*
 * Screen utilities
 */
void vga_clear_line(uint8_t row);
void vga_clear_screen(void);

/*
 * Cursor utilities
 */
void vga_newline(void);
void vga_home(void);

#endif