;CLI
;MOV al,0x20
;out 0x20,al
STI
JMP	start;
macro syscall num
{
    mov     ah,0x00
    mov     al, num
    int       50h
}

PRINT equ 0x01;
EXIT equ 0x02;
GETCH equ 0x02;