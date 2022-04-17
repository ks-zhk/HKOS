#include "interrupt.h"
#include "stdint.h"
#include "global.h"
#include "print.h"
#include "io.h"
#define IDT_DESC_CNT 0x21
#define PIC_M_CTRL 0x20
#define PIC_M_DATA 0x21
#define PIC_S_CTRL 0xa0
#define PIC_S_DATA 0xa1
#define EFLAGS_IF   0x00000200
#define GET_EFLAGS(EFLAG_VAR) asm("pushfl; popl %0": "=g"(EFLAG_VAR))
struct gate_desc{
    uint16_t func_offset_low_word;
    uint16_t selector;
    uint8_t dcount;
    uint8_t attribute;
    uint16_t func_offset_high_word;
    //altogether:8 bytes (16 + 16 + 8 + 8 + 16 = 64bits = 8 bytes)
};
// typedef struct gate_desc GATE_DESC;
// static void make_idt_desc(struct gate_desc* p_gdesc,uint8_t attr,intr_handler function);
extern intr_handler intr_entry_table[IDT_DESC_CNT];
intr_handler idt_table[IDT_DESC_CNT];
char * intr_name[IDT_DESC_CNT];
struct gate_desc idt[IDT_DESC_CNT];
static void make_idt_desc(struct gate_desc* p_gdesc,uint8_t attr,intr_handler function);
static void general_idtr_handler(uint8_t vec_nr);
static void idt_desc_init(void);
static void pic_init(void);
static void exception_init(void);
static void make_idt_desc(struct gate_desc* p_gdesc,uint8_t attr,intr_handler function){
    p_gdesc->func_offset_low_word = (uint32_t)function & 0x0000ffff;
    p_gdesc->selector = SELECTOR_K_CODE;
    p_gdesc->dcount = 0;
    p_gdesc->attribute = attr;
    p_gdesc->func_offset_high_word = ((uint32_t)function & 0xffff0000) >> 16;
}
static void pic_init(void){
    put_str("   pic_init\n");
    outb(PIC_M_CTRL,0x11);
    outb(PIC_M_DATA,0x20);

    outb(PIC_M_DATA,0x04);
    outb(PIC_M_DATA,0x01);

    outb(PIC_S_CTRL,0x11);
    outb(PIC_M_DATA,0x28);
    
    outb(PIC_S_DATA,0x02);
    outb(PIC_S_DATA,0x01);

    outb(PIC_M_DATA,0xfe);
    outb(PIC_S_DATA,0xff);

    put_str("   pic_init done\n");
}
static void idt_desc_init(void){
    // put_int((uint32_t)intr_entry_table);
    // put_char('\n');
    put_str("   idt_desc_init\n");
    int i;
    for(i=0;i<IDT_DESC_CNT;i++){
        make_idt_desc(&idt[i],IDT_DESC_ATTR_DPL0,intr_entry_table[i]);
    }
    put_str("   idt_desc_init done\n");
}
static void general_idtr_handler(uint8_t vec_nr){
    if(vec_nr == 0x27 || vec_nr == 0x2f){
        return;
    }
    set_cur(0);
    int cur_pos = 0;
    while(cur_pos < 320){
        put_char(' ');
        cur_pos++;
    }
    set_cur(0);
    put_str("!!!!!!!    excetion message begin  !!!!!!!!\n");
    set_cur(88);
    put_str(intr_name[vec_nr]);

    if(vec_nr == 14){
        int page_fault_vaddr = 0;
        asm("movl %%cr2,%0":"=r"(page_fault_vaddr));
        put_str("\npage fault addr is ");
        put_int(page_fault_vaddr);
    }
    put_str("\n!!!!!!!    excetion message end  !!!!!!!!\n");
    while(1);
}
static void exception_init(void){
    put_str((uint8_t *)"   exception_init\n");
    int i;
    for(i=0;i<IDT_DESC_CNT;i++){
        idt_table[i] = general_idtr_handler;
        intr_name[i] = "unknown";
    }
    intr_name[0] = "#DE Divide Error";
    intr_name[1] = "#DB Debug Exception";
    intr_name[2] = "#NMI Interrupt";
    intr_name[3] = "#BP Breakpoint Exception";
    intr_name[4] = "#OF Overflow Exception";
    intr_name[5] = "#BR BOUND Range Exceeded Exception";
    intr_name[6] = "#UD Invalid Opcode Exception";
    intr_name[7] = "#NM Device Not Avaliable Exception";
    intr_name[8] = "#DF Double Fault Exception";
    intr_name[9] = "#CSO Coprocessor Segment Overrun";
    intr_name[10] = "#TS Invalid TSS Exception";
    intr_name[11] = "#NP Segment Not Present";
    intr_name[12] = "#SS Stack Fault Exception";
    intr_name[13] = "#GP Gerneral Protection Exception";
    // asm("nop");
    // asm("nop");
    // asm("nop");
    // intr_name[13] = "#";
    intr_name[14] = "#PG Page-Fault Exception";
    intr_name[15] = "reserved";
    intr_name[16] = "#MF x87 FPU Floating-Point Error";
    intr_name[17] = "#AC Alignment Check Exception";
    intr_name[18] = "#MC Machine-Check Exception";
    intr_name[19] = "#XF SIMD Floating-Point Exception";
    put_str((uint8_t *)"   exception_init done\n");
}
enum intr_status intr_enable(){
    enum intr_status old_status;
    if(INTR_ON == intr_get_status()){
        old_status = INTR_ON;
        return old_status;
    }else{
        old_status = INTR_OFF;
        asm("sti");
        return old_status;
    }
}
enum intr_status intr_disable(){
    enum intr_status old_status;
    if(INTR_OFF == intr_get_status()){
        old_status = INTR_OFF;
        return old_status;
    }else{
        old_status = INTR_ON;
        asm("cli": : : "memory");
        return old_status;
    }
}
enum intr_status intr_set_status(enum intr_status status){
    if(status == INTR_ON)
        intr_enable();
    else
        intr_disable();
    return status;
}
enum intr_status intr_get_status(void){
    uint32_t eflags = 0;
    GET_EFLAGS(eflags);
    return (EFLAGS_IF & eflags) ? INTR_ON : INTR_OFF;
}
void idt_init(){
    put_str((uint8_t *)"idt_init start\n");
    idt_desc_init();
    pic_init();
    uint64_t idt_operand = ((sizeof(idt) - 1) | ((uint64_t)((uint32_t)idt << 16)));
    // put_int(idt_operand);
    exception_init();
    asm("lidt %0"::"m"(idt_operand));
    // asm("sti");
    put_str((uint8_t *)"idt_init done\n");
    return;
}
void register_handler(uint8_t vec_no,intr_handler function){
    idt_table[vec_no] = function;
}