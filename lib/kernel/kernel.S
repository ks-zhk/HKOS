[bits 32]
%define ERROR_CODE nop
;if cpu has push error_code,in order to keep the stack same, do nothing
%define ZERO push 0
;if cpu hasn't push error_code,push 0 to keep the stack same

extern put_str
extern idt_table
extern intr_name
section .data
global intr_entry_table

intr_entry_table:

%macro VECTOR 2
section .text
intr%1entry:
    %2
    push ds
    push es
    push fs
    push gs
    pushad
    mov al,0x20
    out 0xa0,al
    out 0x20,al
    
    push %1

    call [idt_table + %1*4]
    jmp intr_exit
section .data
    dd intr%1entry
%endmacro

VECTOR 0x00,ZERO
VECTOR 0X01,ZERO
VECTOR 0X02,ZERO
VECTOR 0x03,ZERO
VECTOR 0X04,ZERO
VECTOR 0X05,ZERO
VECTOR 0x06,ZERO
VECTOR 0X07,ZERO
VECTOR 0X08,ERROR_CODE
VECTOR 0x09,ZERO
VECTOR 0X0A,ZERO
VECTOR 0X0B,ZERO
VECTOR 0x0C,ZERO
VECTOR 0X0D,ZERO
VECTOR 0X0E,ZERO
VECTOR 0x0F,ZERO
VECTOR 0X10,ZERO
VECTOR 0X11,ERROR_CODE
VECTOR 0x12,ZERO
VECTOR 0X13,ZERO
VECTOR 0X14,ZERO
VECTOR 0x15,ZERO
VECTOR 0X16,ZERO
VECTOR 0X17,ZERO
VECTOR 0x18,ZERO
VECTOR 0X19,ZERO
VECTOR 0X1A,ZERO
VECTOR 0x1B,ZERO
VECTOR 0X17C,ZERO
VECTOR 0X1D,ZERO
VECTOR 0X1E,ERROR_CODE
VECTOR 0X1F,ZERO
VECTOR 0x20,ZERO

section .text
global intr_exit
intr_exit:
    add esp,4
    popad
    pop gs
    pop fs
    pop es
    pop ds
    add esp,4
    iretd