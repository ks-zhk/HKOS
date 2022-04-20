#include "thread.h"
#include "stdint.h"
#include "string.h"
#include "global.h"
#include "memory.h"
#include "interrupt.h"
#include "debug.h"
#include "list.h"
#include "print.h"
#define PAGE_SIZE 4096
#define NULL 0
struct task_struct* main_thread;
struct list thread_ready_list;
struct list thread_all_list;
static struct list_elem* thread_tag;

extern void switch_to(struct task_struct* cur,struct task_struct* nxt);

struct task_struct* running_thread(){
    uint32_t esp;
    asm("mov %%esp, %0":"=g"(esp));
    // put_str("stack = ");
    // put_int(esp & 0xfffff000);
    // put_char('\n');
    return (struct task_struct*)(esp & 0xfffff000);
}
static void kernel_thread(thread_func* function,void * func_arg){
    intr_enable();
    function(func_arg);
}
void thread_create(struct task_struct* pthread,thread_func function,void* func_arg){
    pthread->self_kstack -= sizeof(struct intr_stack);

    pthread->self_kstack -= sizeof(struct thread_stack);

    struct thread_stack* kthread_stack = (struct thread_stack*)pthread->self_kstack;

    kthread_stack ->eip = kernel_thread;
    kthread_stack ->function = function;
    kthread_stack ->func_arg = func_arg;
    kthread_stack -> ebp = kthread_stack->ebx = \
    kthread_stack -> esi = kthread_stack->edi = 0;
}

void init_thread(struct task_struct* pthread, char* name, int prio){
    memset(pthread,0,sizeof(*pthread));
    strcpy(pthread->name,name);
    if(pthread == main_thread){
        pthread->status = TASK_RUNNING;
    }else{
        pthread->status = TASK_READY;
    }
    // pthread->status = TASK_RUNNING;
    pthread->priority = prio;
    pthread->elapsed_ticks = 0;
    pthread->ticks = prio;
    pthread->pgdir = NULL;
    pthread->self_kstack = (uint32_t*)((uint32_t)pthread + PAGE_SIZE);
    pthread->stack_magic = 0x20010614;
    pthread->general_tag.nxt = NULL;
    pthread->general_tag.pre = NULL;
}
static void make_main_thread(void){
    main_thread = running_thread();
    init_thread(main_thread,"main",31);
    ASSERT(!elem_find(&thread_all_list,&main_thread->all_list_tag));
    list_append(&thread_all_list,&main_thread->all_list_tag);
    // put_str("task status = ");
    put_int(main_thread->status);
    // put_char('\n');
}
struct task_struct* thread_start(char* name, \
                                int prio, \
                                thread_func function, \
                                void* func_arg){
    struct task_struct* thread = (struct task_struct*)get_kernel_pages(1);
    init_thread(thread,name,prio);
    thread_create(thread,function,func_arg);
    ASSERT(!elem_find(&thread_ready_list,&thread->general_tag));
    list_append(&thread_ready_list,&thread->general_tag);

    ASSERT(!elem_find(&thread_all_list,&thread->all_list_tag));
    list_append(&thread_all_list,&thread->all_list_tag);
    // asm("movl %0, %%esp;\
    //     pop %%ebp; pop %%ebx; pop %%edi; pop %%esi;\
    //     ret"::"g"(thread->self_kstack):"memory");
    return thread;

}

void schedule(){
    // put_str("in schedule\n");
    ASSERT(intr_get_status() == INTR_OFF);
    struct task_struct* cur = running_thread();
    if(cur->status == TASK_RUNNING){
        ASSERT(!elem_find(&thread_ready_list,&cur->general_tag));
        list_append(&thread_ready_list,&cur->general_tag);
        cur->ticks = cur->priority;
        cur->status = TASK_READY;
    }else{

    }
    ASSERT(!list_empty(&thread_ready_list));
    thread_tag = NULL;
    thread_tag = list_pop(&thread_ready_list);
    struct task_struct* nxt = elem2entry(struct task_struct,general_tag,thread_tag);
    // put_int((uint32_t)cur);
    // put_char('\n');
    nxt->status = TASK_RUNNING;
    // put_str("in switch and nxt_thread priority = ");
    // put_int(nxt ->priority);
    // put_char('\n');
    switch_to(cur,nxt);
}
void thread_block(enum task_status stat){
    ASSERT((stat == TASK_BLOCKED) || (stat == TASK_WAITING) || (stat == TASK_HANDING));
    enum intr_status old_status = intr_disable();
    //disable intr to be nuclear opration

    struct task_struct* cur_thread = running_thread();
    cur_thread->status = stat;
    schedule();
    intr_set_status(old_status);
}

void thread_unblock(struct task_struct* pthread){
    enum intr_status old_status = intr_disable();
    ASSERT((pthread->status == TASK_BLOCKED) || (pthread->status == TASK_WAITING) || (pthread->status == TASK_HANDING));
    if(pthread->status != TASK_READY){
        ASSERT(!elem_find(&thread_ready_list,&pthread->general_tag));
        if(elem_find(&thread_ready_list,&pthread->general_tag)){
            PANIC("thread_unblock:blocked thread in ready list\n");
        }
        list_push(&thread_ready_list,&pthread->general_tag);
        pthread->status = TASK_READY;
    }
    intr_set_status(old_status);
}
void thread_init(){
    put_str("thread init start!\n");
    list_init(&thread_all_list);
    list_init(&thread_ready_list);
    make_main_thread();
    put_str("thread init done\n");
}
