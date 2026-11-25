#ifndef RTC_H
#define RTC_H

#include "types.h"

/* CMOS / RTC I/O ports */
#define RTC_INDEX_PORT 0x70
#define RTC_DATA_PORT  0x71

/* CMOS registers (standard PC RTC / real-time clock) */
#define RTC_REG_SECONDS  0x00
#define RTC_REG_MINUTES  0x02
#define RTC_REG_HOURS    0x04
#define RTC_REG_DAY      0x07
#define RTC_REG_MONTH    0x08
#define RTC_REG_YEAR     0x09
#define RTC_REG_STATUS_A 0x0A
#define RTC_REG_STATUS_B 0x0B

/* Status A: bit 7 = update in progress (UIP) */
#define RTC_STATUS_A_UIP 0x80

/* Status B: bit 2 = 24-hour mode, bit 1 = data mode (0 = BCD, 1 = binary) */
#define RTC_STATUS_B_24H 0x02
#define RTC_STATUS_B_BIN 0x01

typedef struct {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint16_t year;
} datetime_t;

/* Initialize the RTC: force 24-hour mode and latch the current time. */
void rtc_init(void);

/* Read the current date/time into `dt` (binary values). Returns 0 on success. */
int rtc_get_datetime(datetime_t *dt);

#endif
