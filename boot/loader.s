%include "/home/ksdeer/Documents/hkos/boot/boot.inc"
section loader vstart=LOADER_BASE_ADDR
LOADER_STACK_TOP equ LOADER_BASE_ADDR

GDT_BASE:
    dd 0x00000000
    dd 0x00000000
CODE_DESC:
    dd 0x0000ffff
    dd DESC_CODE_HIGH4
DATA_STACK_DESC:
    dd 0x0000ffff
    dd DESC_DATA_HIGH4

VIDEO_DESC:
    dd 0x80000007
    dd DESC_VIDEO_HIGH4

    GDT_SIZE equ $ - GDT_BASE
    GDT_LIMIT equ GDT_SIZE - 1
    times 60 dq 0
    SELECTOR_CODE equ (0x0001 << 3) + TI_GDT + RPL0
    SELECTOR_DATA equ (0x0002 << 3) + TI_GDT + RPL0
    SELECTOR_VIDEO equ (0x0003 << 3) + TI_GDT + RPL0

    total_mem_bytes dd 0

    gdt_ptr dw GDT_LIMIT
            dd GDT_BASE

    ards_buf times 244 db 0
    ards_num dw 0

loader_start:

    ;int 0x15 eax=0x0000e820,edx = 0x534d4150
    xor ebx,ebx;set ebx = 0
    mov edx,0x534d4150
    mov di,ards_buf
    mov ax,0
    mov es,ax
.e820_mem_get_loop:
    mov eax,0x0000e820
    mov ecx,20
    int 0x15
    jc .e820_failed_so_try_e801
    add di,cx
    inc word [ards_num]
    cmp ebx,0
    jnz .e820_mem_get_loop
;analyse in ards
    mov cx,[ards_num]
    mov ebx,ards_buf
    xor edx,edx
.find_max_men_area:
    mov eax,[ebx]
    add eax,[ebx+8]
    add ebx,20
    cmp edx,eax
    jge .next_ards
    mov edx,eax
.next_ards:
    loop .find_max_men_area
    jmp .mem_get_ok

;---------int 0x15 ,ax=0xe801
.e820_failed_so_try_e801:
    mov ax,0xe801
    int 0x15
    jc .e801_failed_so_try88
;cal 15MB
    mov cx,0x400
    mul cx
    shl edx,16
    and eax,0x0000ffff
    or edx,eax
    add edx,0x100000
    mov esi,edx
;cal more than 16MB
    xor eax,eax
    mov ax,bx
    mov ecx,0x10000
    mul ecx

    add esi,eax

    mov edx,esi
    jmp .mem_get_ok

;-------int 0x15 ah=0x88---------
.e801_failed_so_try88:
    mov ah,0x88
    int 0x15
    jc .error_hlt
    and eax,0x0000ffff
    mov cx,0x400
    mul cx
    shl edx,16
    or edx,eax
    add edx,0x100000

.mem_get_ok:
    mov [total_mem_bytes],edx
    jmp .true_start
.error_hlt:
    jmp $
.true_start:
    ;mov sp,LOADER_BASE_ADDR
    ;mov bp,loadermsg
    ;mov cx,17
    ;mov ax,0x1301
    ;mov bx,0x001f
    ;mov dx,0x1800
    ;int 0x10

    ;open A20
    ;load GDT
    ;set cr0_PE = 1

    in al,0x92
    or al,0000_0010b
    out 0x92,al

    lgdt [gdt_ptr]

    mov eax,cr0
    or eax,0x00000001
    mov cr0,eax

    jmp dword SELECTOR_CODE:p_mode_start

[bits 32]
p_mode_start:
    mov ax,SELECTOR_DATA
    mov ds,ax
    mov es,ax
    mov ss,ax
    mov esp,LOADER_STACK_TOP
    mov ax,SELECTOR_VIDEO
    mov gs,ax
    mov eax,KERNEL_START_SECTOR
    mov ebx,KERNEL_BIN_BASE_ADDR
    mov ecx,200
    call rd_disk_m_32

    call setup_page
    sgdt [gdt_ptr]
    mov ebx,[gdt_ptr+2]
    or dword [ebx+0x18+4],0xc0000000
    add dword [gdt_ptr+2],0xc0000000
    add esp,0xc0000000
    mov eax,PAGE_DIR_TABLE_POS
    mov cr3,eax

    mov eax,cr0
    or eax,0x80000000
    mov cr0,eax

    lgdt [gdt_ptr]

    mov byte [gs:160],'V'
    jmp SELECTOR_CODE:enter_kernel
enter_kernel:
    call kernel_init
    mov esp,0xc009f000
    jmp KERNEL_ENTRY_POINT

kernel_init:
    xor eax,eax
    xor ebx,ebx
    xor ecx,ecx
    xor edx,edx

    mov dx,[KERNEL_BIN_BASE_ADDR + 42]
    mov ebx,[KERNEL_BIN_BASE_ADDR + 28]

    add ebx,KERNEL_BIN_BASE_ADDR
    mov cx,[KERNEL_BIN_BASE_ADDR + 44]
.each_segment:
    cmp byte[ebx + 0],PT_NULL
    je .PTNULL
    push dword [ebx + 16]
    mov eax,[ebx+4]
    add eax,KERNEL_BIN_BASE_ADDR
    push eax
    push dword [ebx + 8]
    call mem_cpy
    add esp,12

.PTNULL:
    add ebx,edx

    loop .each_segment
    ret


setup_page:
    mov ecx,4096
    mov esi,0
.clear_page_dir:
;clear the mem of page dir
    mov byte [PAGE_DIR_TABLE_POS + esi],0
    inc esi
    loop .clear_page_dir
.create_pde:
    ;create page directory entry
    mov eax,PAGE_DIR_TABLE_POS
    add eax,0x1000
    mov ebx,eax
    or eax,PG_US_U | PG_RW_R | PG_P

    mov [PAGE_DIR_TABLE_POS + 0x0],eax
    mov [PAGE_DIR_TABLE_POS + 0xc00],eax

    sub eax,0x1000
    mov [PAGE_DIR_TABLE_POS + 4092],eax

    mov eax,PAGE_DIR_TABLE_POS
    add eax,0x1000
    mov ecx,256
    mov esi,0
    mov edx,PG_US_U | PG_RW_R | PG_P
.create_pte:
    mov [ebx+esi*4],edx
    add edx,4096
    inc esi
    loop .create_pte

    mov eax,PAGE_DIR_TABLE_POS
    add eax,0x2000
    or eax,PG_US_U | PG_RW_R | PG_P
    mov ebx,PAGE_DIR_TABLE_POS
    mov ecx,254
    mov esi,769
.create_kernel_pde:
    mov [ebx+esi*4],eax
    inc esi
    add eax,0x1000
    loop .create_kernel_pde
    ret

rd_disk_m_32:
;1 写入待操作磁盘数
;2 写入LBA 低24位寄存器 确认扇区
;3 device 寄存器 第4位主次盘 第6位LBA模式 改为1
;4 command 写指令
;5 读取status状态寄存器 判断是否完成工作
;6 完成工作 取出数据
 
 ;;;;;;;;;;;;;;;;;;;;;
 ;1 写入待操作磁盘数
 ;;;;;;;;;;;;;;;;;;;;;
    mov esi,eax   ; !!! 备份eax
    mov di,cx     ; !!! 备份cx
    
    mov dx,0x1F2  ; 0x1F2为Sector Count 端口号 送到dx寄存器中
    mov al,cl     ; !!! 忘了只能由ax al传递数据
    out dx,al     ; !!! 这里修改了 原out dx,cl
    
    mov eax,esi   ; !!!袄无! 原来备份是这个用 前面需要ax来传递数据 麻了
    
;;;;;;;;;;;;;;;;;;;;;
;2 写入LBA 24位寄存器 确认扇区
;;;;;;;;;;;;;;;;;;;;;
    mov cl,0x8    ; shr 右移8位 把24位给送到 LBA low mid high 寄存器中

    mov dx,0x1F3  ; LBA low
    out dx,al 
    
    mov dx,0x1F4  ; LBA mid
    shr eax,cl    ; eax为32位 ax为16位 eax的低位字节 右移8位即8~15
    out dx,al
    
    mov dx,0x1F5
    shr eax,cl
    out dx,al
    
;;;;;;;;;;;;;;;;;;;;;
;3 device 寄存器 第4位主次盘 第6位LBA模式 改为1
;;;;;;;;;;;;;;;;;;;;;

    		 
    		  ; 24 25 26 27位 尽管我们知道ax只有2 但还是需要按规矩办事 
    		  ; 把除了最后四位的其他位置设置成0
    shr eax,cl
    

    
    and al,0x0f 
    or al,0xe0   ;!!! 把第四-七位设置成0111 转换为LBA模式
    mov dx,0x1F6 ; 参照硬盘控制器端口表 Device 
    out dx,al

;;
;;;;;;;;;;;;;;;;;;;;;
;4 向Command写操作 Status和Command一个寄存器
;;;;;;;;;;;;;;;;;;;;;

    mov dx,0x1F7 ; Status寄存器端口号
    mov ax,0x20  ; 0x20是读命令
    out dx,al
    
;;;;;;;;;;;;;;;;;;;;;
;5 向Status查看是否准备好惹 
;;;;;;;;;;;;;;;;;;;;;
    
		   ;设置不断读取重复 如果不为1则一直循环
  .not_ready:     
    nop           ; !!! 空跳转指令 在循环中达到延时目的
    in al,dx      ; 把寄存器中的信息返还出来
    and al,0x88   ; !!! 0100 0100 0x88
    cmp al,0x08
    jne .not_ready ; !!! jump not equal == 0
    
    
;;;;;;;;;;;;;;;;;;;;;
;6 读取数据
;;;;;;;;;;;;;;;;;;;;;

    mov ax,di      ;把 di 储存的cx 取出来
    mov dx,256
    mul dx        ;与di 与 ax 做乘法 计算一共需要读多少次 方便作循环 低16位放ax 高16位放dx
    mov cx,ax      ;loop 与 cx相匹配 cx-- 当cx == 0即跳出循环
    mov dx,0x1F0
 .go_read_loop:
    in ax,dx      ;两字节dx 一次读两字
    mov [ebx],ax
    add ebx,2
    loop .go_read_loop
    ret ;与call 配对返回原来的位置 跳转到call下一条指令

mem_cpy:
    cld
    push ebp
    mov ebp,esp
    push ecx

    mov edi,[ebp + 8]
    mov esi,[ebp + 12]
    mov ecx,[ebp + 16]
    rep movsb

    pop ecx
    pop ebp
    ret