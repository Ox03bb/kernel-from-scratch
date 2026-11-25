#include "driver/rtc.h"

#include "types.h"
#include "utils/utils.h"

/* -------------------------------------------------------------------------- */
/* CMOS / RTC access                                                          */
/* -------------------------------------------------------------------------- */

/* Write the RTC index register and return the data register contents.
 * Bit 7 of the index port masks NMI while the CMOS is addressed. */
static uint8_t cmos_read(uint8_t reg) {
    outb(RTC_INDEX_PORT, reg | 0x80);
    io_wait();
    uint8_t value = inb(RTC_DATA_PORT);
    io_wait();
    return value;
}

/* Wait until an RTC update is not in progress so a consistent read is possible. */
static void rtc_wait_not_updating(void) {
    while ((cmos_read(RTC_REG_STATUS_A) & RTC_STATUS_A_UIP) != 0) {
        /* spin until UIP clears */
    }
}

/* Convert a packed BCD byte to binary. */
static uint8_t bcd_to_bin(uint8_t bcd) { return (uint8_t)((bcd & 0x0F) + ((bcd >> 4) * 10)); }

/* -------------------------------------------------------------------------- */
/* Public                                                                     */
/* -------------------------------------------------------------------------- */

void rtc_init(void) {
    /* Force 24-hour mode (status B bit 2). This makes the hours register
     * report 0-23 directly. */
    uint8_t status_b = cmos_read(RTC_REG_STATUS_B);
    cmos_read(RTC_REG_STATUS_B); /* re-select in case the write above changed state */
    outb(RTC_INDEX_PORT, RTC_REG_STATUS_B | 0x80);
    io_wait();
    outb(RTC_DATA_PORT, status_b | RTC_STATUS_B_24H);
    io_wait();

    /* Wait for any in-progress update to finish before the first read. */
    rtc_wait_not_updating();
}

int rtc_get_datetime(datetime_t *dt) {
    uint8_t status_b;

    if (dt == NULL)
        return -1;

    rtc_wait_not_updating();

    uint8_t seconds = cmos_read(RTC_REG_SECONDS);
    uint8_t minutes = cmos_read(RTC_REG_MINUTES);
    uint8_t hours = cmos_read(RTC_REG_HOURS);
    uint8_t day = cmos_read(RTC_REG_DAY);
    uint8_t month = cmos_read(RTC_REG_MONTH);
    uint8_t year = cmos_read(RTC_REG_YEAR);

    status_b = cmos_read(RTC_REG_STATUS_B);
    int bcd = (status_b & RTC_STATUS_B_BIN) == 0;

    dt->second = bcd ? bcd_to_bin(seconds) : seconds;
    dt->minute = bcd ? bcd_to_bin(minutes) : minutes;
    dt->hour = bcd ? bcd_to_bin(hours) : hours;
    dt->day = bcd ? bcd_to_bin(day) : day;
    dt->month = bcd ? bcd_to_bin(month) : month;
    dt->year = 2000u + (bcd ? bcd_to_bin(year) : year);

    return 0;
}
