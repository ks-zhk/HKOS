TI_GDT equ 0
RPL0 equ 0
SELECTOR_VIDEO equ (0x0003 << 3) + TI_GDT + RPL0
section .data
put_int_buffer dq 0
[bits 32]
section .text
global put_char
global put_str
global put_int
global set_cur
put_int:
    ;4 byte integer data
    ;data
    ;eip
    pushad
    mov ebp,esp
    mov eax,[ebp + 4*9]
    mov edx,eax
    mov edi,7
    mov ecx,8
    mov ebx,put_int_buffer

.16based_4bits:
    and edx,0x0000000F
    cmp edx,9
    jg .is_A2F
    add edx,'0'
    jmp .store
.is_A2F:
    sub edx,10
    add edx,'A'

.store:
    mov byte [ebx+edi],dl
    dec edi
    shr eax,4
    mov edx,eax
    loop .16based_4bits

.ready_to_print:
    inc edi
.skip_prefix_0:
    cmp edi,8
    
    je .full0
.go_on_skip:
    mov cl,[put_int_buffer + edi];
    inc edi
    cmp cl,'0'
    je .skip_prefix_0
    dec edi
    jmp .put_each_num

.full0:
    mov cl,'0'
.put_each_num:
    push ecx
    call put_char
    add esp,4
    inc edi
    mov cl,[put_int_buffer + edi]
    cmp edi,8
    jl .put_each_num
    popad
    ret

put_str:
    push ebx
    push ecx
    xor ecx,ecx
    mov ebx,[esp + 12]
.goon:
    mov cl,[ebx]
    cmp cl,0
    jz .str_over
    push ecx
    call put_char
    add esp,4
    inc ebx
    jmp .goon
.str_over:
    pop ecx
    pop ebx
    ret

put_char:
    pushad;保存寄存器的状态
    mov ax,SELECTOR_VIDEO;选取显存区域的选择子
    mov gs,ax;将选择子赋值给段寄存器

    mov dx,0x03d4;选中寄存器数组  
    mov al,0x0e;索引下标，本寄存器用于获取高8位光标数据
    out dx,al;发出命令，表示需要获取高8位的数据
    mov dx,0x03d5;选中数据传输寄存器
    in al,dx;读入高8位数据
    mov ah,al;放入ax的高8位

    mov dx,0x03d4;选中寄存器数组
    mov al,0x0f;索引下标，本寄存器用于获取低8位的光标位置数据
    out dx,al;发出选中命令
    mov dx,0x03d5;选中数据传输寄存器
    in al,dx;获取低8位数据

    ;将光标数据存入bx寄存器
    mov bx,ax
    mov ecx,[esp + 36]
    ;pushad 压入了32个字节，加上主调函数的4字节的返回地址，因此+36来获取待打印的字符的内存单元

    cmp cl,0xd;?
    jz .is_carriage_return;回车键
    cmp cl,0xa
    jz .is_line_feed;换行键

    cmp cl,0x8
    jz .is_backspace;删除键
    jmp .put_other;打印其他字符

.is_backspace:
    dec bx
    shl bx,1

    mov byte [gs:bx],0x20;字符值
    inc bx
    mov byte [gs:bx],0x07;字符属性
    shr bx,1
    jmp .set_cursor
.put_other:
    shl bx,1
    mov byte [gs:bx],cl
    inc bx
    mov byte [gs:bx],0x07
    shr bx,1
    inc bx
    cmp bx,2000
    jl .set_cursor

.is_line_feed:
.is_carriage_return:
    xor dx,dx
    mov ax,bx
    mov si,80
    div si
    sub bx,dx
.is_carriage_return_end:
    add bx,80
    cmp bx,2000
.is_line_feed_end:
    jl .set_cursor

.roll_screen:
    cld
    mov ecx,960
    mov esi,0xc00b80a0
    mov edi,0xc00b8000
    rep movsd

    mov ebx,3840
    mov ecx,80
.cls:
    mov word [gs:ebx],0x0720;
    add ebx,2
    loop .cls
    mov bx,1920
.set_cursor:
    mov dx,0x03d4
    mov al,0x0e
    out dx,al
    mov dx,0x03d5
    mov al,bh
    out dx,al

    mov dx,0x03d4
    mov al,0x0f
    out dx,al
    mov dx,0x03d5
    mov al,bl
    out dx,al
.put_char_down:
    popad
    ret

set_cur:
    pushad
    mov ebp,esp
    mov ebx,[ebp + 4*9]
    mov dx,0x03d4
    mov al,0x0e
    out dx,al
    mov dx,0x03d5
    mov al,bh
    out dx,al

    mov dx,0x03d4
    mov al,0x0f
    out dx,al
    mov dx,0x03d5
    mov al,bl
    out dx,al

    popad
    ret