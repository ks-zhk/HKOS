#ifndef __LIB_IO_H__
#define __LIB_IO_H__

#include "stdint.h"
static inline void outb(uint16_t port,uint8_t data);
static inline void outsw(uint16_t port,const void* addr,uint32_t word_cnt);
static inline uint8_t inb(uint16_t port);
static inline void insw(uint16_t port,void *addr,uint32_t word_cnt);


static inline void outb(uint16_t port,uint8_t data){
    asm("outb %b0, %w1"::"a"(data),"Nd"(port));
}
static inline void outsw(uint16_t port,const void* addr,uint32_t word_cnt){
    asm("cld; rep outsw": "+S"(addr), "+c"(word_cnt) : "d"(port));
}
static inline uint8_t inb(uint16_t port){
    uint8_t data;
    asm("inb %w1, %b0" : "=a"(data):"Nd"(port));
    return data;
}
static inline void insw(uint16_t port,void *addr,uint32_t word_cnt){
    asm("cld; rep insw":"+D"(addr),"+c"(word_cnt) : "d"(port) : "memory");
}
#endif
