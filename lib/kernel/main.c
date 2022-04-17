#include "print.h"
#include "init.h"
#include "stdint.h"
#include "debug.h"
#include "string.h"
#include "memory.h"
#include "thread.h"
#include "interrupt.h"
void k_thread_a(void*);
void k_thread_b(void*);
int main(void){
    init_all();
    // void* addr = get_kernel_pages(3);
    // put_int((uint32_t)addr);
    // put_char('\n');
    // put_char('\n');
    
    thread_start("k_thread_a",31,k_thread_a,"argA ");
    thread_start("k_thread_b",20,k_thread_b,"argB ");
    // // ASSERT(1==2);
    // // put_str("return to main\n");
    // // asm("int $0xd");
    // // asm("int $0x6");
    intr_enable();
    while(1){
        put_str("Main ");
    };
    return 0;
}
void k_thread_a(void* arg){
    char *para = arg;
    while(1){
        put_str(para);
    }
}
void k_thread_b(void* arg){
    char *para = arg;
    while(1){
        put_str(para);
    }
}