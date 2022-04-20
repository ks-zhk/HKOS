#ifndef __DEVICE_CONSOLE_H__
#define __DEVICE_CONSOLE_H__
#include "stdint.h"
// #include "sync.h"
// #include "thread.h"

void console_init(void);
void console_acquire(void);
void console_release(void);
void console_put_str(uint8_t *str);
void console_put_char(char ascii);
void console_put_int(uint32_t num);
#endif