#include "console.h"
#include "thread.h"
#include "sync.h"
#include "stdint.h"
#include "print.h"
static struct lock console_lock;

void console_init(void){
    lock_init(&console_lock);
}
void console_acquire(void){
    lock_acquire(&console_lock);
}
void console_release(void){
    lock_release(&console_lock);
}
void console_put_str(uint8_t *str){
    console_acquire();
    put_str(str);
    console_release();
}
void console_put_char(char ascii){
    console_acquire();
    put_char(ascii);
    console_release();
}
void console_put_int(uint32_t num){
    console_acquire();
    put_int(num);
    console_release();
}