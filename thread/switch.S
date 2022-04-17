[bits 32]
section .text
global switch_to
switch_to:
;back up cur task
    push esi
    push edi
    push ebx
    push ebp

    mov eax,[esp + 20]
    mov [eax],esp
;recover nxt task
    mov eax,[esp + 24]
    mov esp,[eax]

    pop ebp
    pop ebx
    pop edi
    pop esi
    ret