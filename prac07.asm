default rel
section .text
global strlen_asm, strcpy_asm, strcmp_asm, itoa_asm, atoi_asm


strlen_asm:
    push rdi
    mov rdi, rcx
    xor eax, eax
    mov rcx, -1
    cld
    repne scasb
    not rcx
    dec rcx
    mov rax, rcx
    pop rdi
    ret

strcpy_asm:
    push rdi
    push rsi
    mov r8, rcx
    mov rdi, rdx
    xor eax, eax
    mov rcx, -1
    cld
    repne scasb
    not rcx
    mov rsi, rdx
    mov rdi, r8
    rep movsb
    mov rax, r8
    pop rsi
    pop rdi
    ret

strcmp_asm:
    push rsi
    push rdi
    mov rsi, rcx
    mov rdi, rdx
    cld

.cmp_loop:
    lodsb
    scasb
    jne .cmp_diff
    test al, al
    jnz .cmp_loop
    xor eax, eax
    jmp .cmp_done

.cmp_diff:
    movzx eax, byte [rsi - 1]
    movzx edx, byte [rdi - 1]
    sub eax, edx

.cmp_done:
    pop rdi
    pop rsi
    ret


itoa_asm:
    push rbx
    mov r8, rdx
    mov r11, rdx
    mov eax, ecx
    xor r10d, r10d
    test eax, eax
    jns .itoa_abs
    mov r10d, 1
    neg eax

.itoa_abs:
    mov ebx, 10
    xor r9d, r9d
    test eax, eax
    jnz .itoa_convert
    push qword 0
    inc r9d
    jmp .itoa_write

.itoa_convert:
    xor edx, edx
    div ebx
    push rdx
    inc r9d
    test eax, eax
    jnz .itoa_convert

.itoa_write:
    test r10d, r10d
    jz .itoa_digits
    mov byte [r8], '-'
    inc r8

.itoa_digits:
    mov ecx, r9d

.write_loop:
    pop rdx
    add dl, '0'
    mov [r8], dl
    inc r8
    loop .write_loop

    mov byte [r8], 0
    mov rax, r11
    pop rbx
    ret


atoi_asm:
    push rsi
    mov rsi, rcx
    xor r8d, r8d
    xor r9d, r9d
    cld
    lodsb

    cmp al, '-'
    jne .atoi_check_plus
    mov r8d, 1
    lodsb
    jmp .atoi_digits

.atoi_check_plus:
    cmp al, '+'
    jne .atoi_digits
    lodsb

.atoi_digits:
    cmp al, '0'
    jb .atoi_done
    cmp al, '9'
    ja .atoi_done

    imul r9, r9, 10
    movzx edx, al
    sub edx, '0'
    add r9, rdx
    lodsb
    jmp .atoi_digits

.atoi_done:
    mov rax, r9
    test r8d, r8d
    jz .atoi_return
    neg rax

.atoi_return:
    pop rsi
    ret
