%include "/home/ksdeer/Documents/hkos/boot/boot.inc"
SECTION MBR vstart=0x7c00
    mov ax,cs
    mov ds,ax
    mov es,ax
    mov fs,ax
    mov ss,ax
    mov sp,0x7c00
    mov ax,0xb800
    mov gs,ax




    ;use int 0x10 to clear the screen
    mov ax,0600h
    mov bx,0700h
    mov cx,0
    mov dx,184fh

    int 10h

    mov byte [gs:0x00],'1'
    mov byte [gs:0x01],0xA4

    mov byte [gs:0x02],' '
    mov byte [gs:0x03],0xA4

    mov byte [gs:0x04],'M'
    mov byte [gs:0x05],0xA4

    mov byte [gs:0x06],'B'
    mov byte [gs:0x07],0xA4

    mov byte [gs:0x08],'R'
    mov byte [gs:0x09],0xA4 

    ;prepare for loader

    mov eax,LOADER_START_SECTOR
    mov bx,LOADER_BASE_ADDR
    mov cx,4

    call rd_disk_m_16
    ; mov ax,ds
    ; mov es,ax
    ; mov bx,LOADER_BASE_ADDR
    ; mov ah,0x02
    ; mov al,0x04
    ; mov ch,0x0
    ; mov cl,0x03
    ; mov dh,0x00
    ; mov dl,0x07
    ; int 13h
    jmp LOADER_BASE_ADDR+0x300

rd_disk_m_16:
    mov esi,eax
    mov di,cx
;step 1:set the secotr count to be read
    mov dx,0x1f2    ;select sector count port
    mov al,cl       ;al=sector count
    out dx,al       ;set sector count
    mov byte [gs:0x0a],'1'
    mov byte [gs:0x0b],0xA4
    mov eax,esi     ;restore eax


;step 2:set the LBA address
    ;LBA low 8bits
    mov dx,0x1f3
    out dx,al
    ;LBA mid 8bits
    mov cl,8
    shr eax,cl;move right 8 bits
    mov dx,0x1f4
    out dx,al
    ;LBA high 8bits
    shr eax,cl
    mov dx,0x1f5
    out dx,al
    ;LBA last 4bits in reg_device
    shr eax,cl
    and al,0x0f
    or al,0xe0
    mov dx,0x1f6
    out dx,al
;step 3:write read command to port
    mov dx,0x1f7
    mov al,0x20
    out dx,al

;step 4:check the status of hhd

.not_ready:
    nop
    in al,dx
    and al,0x88

    cmp al,0x08
    jnz .not_ready

    mov ax,di
    mov dx,256
    mul dx
    mov cx,ax

    mov dx,0x1f0
.go_on_read:
    in ax,dx
    mov [bx],ax
    add bx,2
    loop .go_on_read
    ret

    times 510-($-$$) db 0
    db 0x55,0xaa
