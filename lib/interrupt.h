#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__
#include "stdint.h"
typedef void* intr_handler;
void idt_init(void);
enum intr_status{
    INTR_OFF,
    INTR_ON
};
// static void make_idt_desc(struct gate_desc* p_gdesc,uint8_t attr,intr_handler function);
// static void general_idtr_handler(uint8_t vec_nr);
// static void idt_desc_init(void);
// static void pic_init(void);
// static void exception_init(void);
enum intr_status intr_get_status(void);
enum intr_status intr_set_status(enum intr_status);
enum intr_status intr_enable(void);
enum intr_status intr_disable(void);
void register_handler(uint8_t vec_no,intr_handler function);
#endif
