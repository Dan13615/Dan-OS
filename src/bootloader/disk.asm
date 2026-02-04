; Read sector from LBA
; Input: EDI = LBA, ESI = buffer address, ECX = sector count
ata_read_sectors:
    push rax
    push rdx

    ; Wait for drive ready
.wait_ready:
    mov dx, 0x1F7          ; Status port
    in al, dx
    test al, 0x80          ; BSY bit
    jnz .wait_ready
    test al, 0x08          ; DRQ bit
    jz .wait_ready

    ; Send sector count
    mov dx, 0x1F2
    mov al, cl
    out dx, al

    ; Send LBA
    mov dx, 0x1F3
    mov eax, edi
    out dx, al             ; LBA bits 0-7

    mov dx, 0x1F4
    shr eax, 8
    out dx, al             ; LBA bits 8-15

    mov dx, 0x1F5
    shr eax, 8
    out dx, al             ; LBA bits 16-23

    mov dx, 0x1F6
    shr eax, 8
    and al, 0x0F
    or al, 0xE0            ; LBA mode, master drive
    out dx, al

    ; Send READ command
    mov dx, 0x1F7
    mov al, 0x20
    out dx, al

    ; Wait and read data
.read_loop:
    call .wait_ready

    mov dx, 0x1F0          ; Data port
    mov ecx, 256           ; 512 bytes = 256 words
    mov rdi, rsi
    rep insw               ; Read words from port to [RDI]

    add rsi, 512
    dec cl
    jnz .read_loop

    pop rdx
    pop rax
    ret