;/*************
; * EddOS     *
; * 2020-2023 *
; *************/
; A simple mouse driver implementation written
; in FASM, x86 assembly. It supports basic event
; handling (such as keypresses, etc.)

format binary as "img"
use16
org 0x7C00

label interfata
label reset
 mov ah, 0eh
 mov al, 7d
 int 10h
 int  10h

 ; Read current cursor position
 ; Interrupt 10h for video services
 mov ah, 01h
 mov cl, 07h
 mov ch, 0h
 int 10h

 ; Enter VGA text-mode
 mov ax, 1003h
 mov bl, 0x00
 mov bh, 0
 int 10h

 ; Clear the screen
 mov ax, 0h
 mov ah, 06h
 xor al, al
 xor cx, cx
 mov dx, 184fh
 mov bh, 83h
 int 10h

 ; Move the cursor to row 0, column 0
 ; Begin printing out the top bar. This part is pretty boring.
 mov ah, 02h
 mov bh, 0h
 mov dh, 0h
 mov dl, 0h
 int 10h

 mov ah, 0eh
 mov al, 178d
 int 10h

 mov ah, 02h
 mov dl, 01h
 int 10h

 mov ah, 09h
 mov al, 254d
 mov bx, 001ah

 mov cx, 0001d

 int 10h

 mov ah, 02h
 mov dl, 02h
 int 10h

 mov ah, 09h

 ; This part is put after the clock is shown.
 ; Note we left some free space to the left.
 mov al, ']'
 mov bx, 001fh
 mov cx, 0001d
 int 10h

 mov bx, 0013h

 mov ah, 02h
 mov bh, 0h
 mov dh, 0h
 mov dl, 49h
 int 0h

 mov dl, 03h
 int 10h


 mov ah, 0eh
 mov al, 16d
 int 10h

 mov ah, 09h
 mov al, 176d
 mov cx, 76d

 int 10h

 mov ah, 02h
 mov dl, 4ah
 int 10h


 mov bh, 0h
 mov dh, 2h
 mov dl, 0h
 int 10h

 mov ah, 09h

 mov al, 223
 mov bx, 0080h
 mov cx, 0001d
 int 10h

 mov ah, 02h
 mov bh, 0h
 mov dh, 2h
 mov dl, 6h
 int 10h

 mov ah, 09h
 mov cx, 00074d
 int 10h

 mov ah, 02h
 mov bh, 0h
 mov dh, 18h
 mov dl, 0h

 int 10h

 mov ah, 0eh    ; Print mode
 mov al, 'E'    ; Write out letter by letter
 int 10h

 mov al, 'd'
 int 10h

 mov al, 'd'
 int 10h

 mov al, 'O'
 int 10h


 mov al, 'S'
 int 10h

 mov ah, 02h
 mov bh, 0h
 mov dh, 3h
 mov dl, 1h
 int 10h        ; ... and print the message


 mov ah, 02h
 mov bh, 0h
 mov dh, 1h
 mov dl, 0h
 int 10h
 mov bl, 3h
 mov cl, 3h
 jmp _timp

_again:
 mov ah, 02h

 cmp cl, 1h
 je _crctX1

 cmp cl, 17h
 je _crctX2


 mov dl, bl
 mov dh, cl

 int 10h
 int 10h

 mov ah, 00h
 int 16h

 cmp al, 77h    ;CASE 1: up
 je _sus        ;Call the function

 cmp al, 73h    ;CASE 2: down
 je _jos        ;Call the function

 cmp al, 61h    ;CASE 3: left
 je _stanga     ;Call the function

 cmp al, 64h    ;CASE 4: right
 je _dreapta    ;Call the function

 cmp al, 65h    ;CASE 5: beep
 je _bip        ;Call the function

 cmp al, 72h    ;CASE 5: refresh time
 je _timp       ;Call the function

 cmp al, 70h    ;CASE 6: reset
 je _urgenta    ;Call the function

 jmp _again     ; The program loops infinitely, taking input

_crctX1:        ; Sometimes, the cursor position breaks
 mov cl, 2h
 jmp _again

_crctX2:        ; -"-, other case
 mov cl, 16h    ; New value
 jmp _again     ; Try again

_sus:
 dec cl
 cmp cl, 2
 jg _mousejos
 jmp _again


_jos:
 inc cl
 cmp cl, 3
 je nu
 cmp cl, 21
 jng _mousejos
 label labeljos
 jmp _again
_stanga:
 dec bl
 cmp cl, 2
 jg _mousejos
 ;mov ah, 02h
 ;mov dl, bl
 ;mov dh, cl
 ;int 10h
 ;mov ah, 0eh
 ;mov al, 178d
 ;int 10h


 jmp _again

_dreapta:
 inc bl
 cmp cl, 2
 jg _mousedrj

 ;mov al, 178d
 ;int 10h
 ;jmp _again

_bip:           ; Hardware beep
 mov ah, 0eh    ; Print a char
 mov al, 7d     ; Bell character
 int 10h        ; Call interrupt

 jmp _again

_reset:
 mov bl, 3h
 mov cl, 3h
 mov ah, 02h
 jmp _again

_urgenta:       ; This restarts the bootloader.
 jmp interfata  ; ...

_timp:          ; Show the hardware clock value
 mov dl, 00h 
 mov dh, 01h
 mov ah, 02h    ; New cursor position
 int 10h        ; Call interrupt
 mov ah, 09h
 mov al, 23d
 mov bx, 0071h
 mov cx, 0001d
 int 10h

 mov bx, 0d

 mov dl, 01h
 mov dh, 01h
 mov ah, 02h
 int 10h

 int 1ah         ; Get hardware clock and store in 2 BCD registers
 mov ax, cx      ; Store backup
 mov cx, 4

_urmator:        ; Print contents of CX
 inc bx          ; Algorithm begin
 cmp bx, 3d
 je _douapuncte
 label _continuare
 push cx
 mov cl, 4
 rol ax, cl
 push ax
 and al, 0fh
 add al, '0'
 cmp al, '9'
 jle _final

 add al, 'a'-'9'-1

_final:          ; End the clock alg
 mov ah, 0eh
 int 10h
 pop ax
 pop cx
 loop _urmator
 mov     ah, 09h

 mov al, ']'     ; We write over a part of the top bar.
 mov bx, 001fh   ; For some reason I couldn't find a better fix.
 mov cx, 0001d
 int 10h

 mov ah, 02h
 mov bh, 0h
 mov dh, 1h
 mov dl, 7h
 int 10h


 mov ah, 09h

 mov al, 205
 mov bx, 0019h
 mov cx, 0073d
 int 10h

 jmp _reset

_douapuncte:     ; Show a colon sign in the clock
 mov bh, ah
 mov ah, 0eh
 mov al, ':'
 int 10h
 mov ah, bh
 jmp _continuare
 
_mousejos:       ; Handle "s" key event
 mov ah, 0eh
 mov al, ' '
 int 10h
 label nu
 mov ah, 02h
 mov dh, cl
 mov dl, bl
 int 10h
 mov ah, 0eh
 mov al, 178d
 int 10h
 jmp labeljos
_mousedrj:       ; Handle bottom left behavior
 mov ah, 0eh
 mov al, ' '
 int 10h
 jmp _mousejos

 ;-----BIOS SIGNATURE BEGIN-----
 times 510-($-$$)db 0
 dw 0xAA55
 ;-----BIOS SIGNATURE END-----
