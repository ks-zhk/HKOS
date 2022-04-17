#ifndef __KERNEL_DEBUG_H__
#define __KERNEL_DEBUG_H__
void panic_spin(char * filename,int line,const char *func,const char *condition);
#ifndef __NULL__
#define __NULL__ 0
#define NULL __NULL__
#endif
#define PANIC(...) panic_spin(__FILE__,__LINE__,__func__,__VA_ARGS__)
#ifdef NDEBUG
    #define ASSERT(CONDITION) ((void)0)
#else
#define ASSERT(CONDITION) \
if(CONDITION){}else{PANIC(#CONDITION);}
#endif /* NDEBUG */
#endif /* __KERNEL_DEBUG_H__ */