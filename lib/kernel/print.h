#ifndef __LIB_KERNEL_PRINT_H__
#define __LIB_KERNEL_PRINT_H__
#include "stdint.h"
void put_char(uint8_t char_asci);
void put_str(uint8_t *message);
void put_int(uint32_t num);
void set_cur(uint32_t cur);
#endif