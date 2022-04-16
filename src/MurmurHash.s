section .text
global MurmurHash

;==============================================
; Returns 64 bit murmur hash
; Expects:
;   Fastcall: rdi - const char *data
;             rsi - int data_len
; Returns:
;             rax - hash
;==============================================

MurmurHash:
    mov rcx, 24           ; rcx = r
    mov r10, 0x5bd1e995   ; r10 = m
    mov r11, 0x123456789F ; r11 = some seed

    xor r11, rsi          ; h = seed ^ len

.Loop:
    cmp rsi, 4
    jb  .LoopEnd
    
    mov eax, dword [rdi]  ; load 4 bytes into r12 = k

    mul r10               ; k *= m

    mov r9, rax           ; r9 = k
    shr r9, cl            ; r9 = k >> r
    xor rax, r9           ; k ^= k >> r

    mul r10               ; k *= m
    mov r9, rax           ; save k to r9

    mov rax, r11          ;
    mul r10               ; h *= m

    xor rax, r9           ; h ^= k
    mov r11, rax          ; save h to r11

    add rdi, 4
    sub rsi, 4
    jmp .Loop

.LoopEnd:
    mov r9, [.jmpTable + rsi * 8] ; switch (rsi)
    jmp r9

;------------------ <Jump table> --------------
.jmpTable:
    dq .0symLeft
    dq .1symLeft
    dq .2symLeft
    dq .3symLeft

.3symLeft:
    xor rax, rax
    mov al, byte [rdi + 2]
    shl rax, 16

.2symLeft:
    xor rax, rax
    mov al, byte [rdi + 1]
    shl rax, 8

.1symLeft:
    xor rax, rax
    mov al, byte [rdi]

    xor r11, rax

    mov rax, r11          ; rax = h
    mul r10               ; h *= m
    
    mov r11, rax          ; rax = r11 = h

.0symLeft:
    mov rax, r11

    shr r11, 13           ; r11 = h >> 13
    xor rax, r11          ; h ^= h >> 13

    mul r10               ; h *= m

    mov r11, rax          ; r11 = rax = h

    shr r11, 15           ; r11 = h >>15
    xor rax, r11          ; h ^= h >> 15
    
    ret                   ; return h = rax
