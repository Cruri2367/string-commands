default rel
section .text
global memset_asm, memcpy_asm


memset_asm:
    push rdi
    mov r9, rcx
    mov rdi, rcx
    mov al, dl
    mov rcx, r8
    cld
    rep stosb
    mov rax, r9
    pop rdi
    ret


memcpy_asm:
    push rdi
    push rsi
    mov r9, rcx
    mov rdi, rcx
    mov rsi, rdx
    mov rcx, r8
    cld
    rep movsb
    mov rax, r9
    pop rsi
    pop rdi
    ret
