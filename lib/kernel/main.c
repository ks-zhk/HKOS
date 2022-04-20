#include "print.h"
#include "init.h"
#include "stdint.h"
#include "debug.h"
#include "string.h"
#include "memory.h"
#include "thread.h"
#include "interrupt.h"
#include "console.h"
void k_thread_a(void*);
void k_thread_b(void*);
void fuckNvidia(void* arg);
int main(void){
    init_all();
    thread_start("k_thread_a",31,k_thread_a,"argA ");
    thread_start("k_thread_b",31,k_thread_b,"argB ");
    // thread_start("fuckNvidia",31,fuckNvidia,"fuck nvidia! ");
    intr_enable();
    while(1){
        console_put_str("Main\n");
        console_put_int(running_thread()->ticks);
        console_put_char('\n');
    };
    return 0;
}
void k_thread_a(void* arg){
    char *para = arg;
    while(1){
        console_put_str("argA ");
        console_put_int(running_thread()->ticks);
        console_put_char('\n');
    }
}
void k_thread_b(void* arg){
    char *para = arg;
    while(1){
        console_put_str("argB ");
        console_put_int(running_thread()->ticks);
        console_put_char('\n');
    }
}