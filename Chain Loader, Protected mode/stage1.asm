;Incaracator nivelul 1 aflat la CHS:0:0:1
;Incarca in memorie alt incarcator de la LBA 1
use16;Cod 16 bit
org 0x7C00;Offsetul 0 de la adresa controlului
sof:
;_________________________________________________________________
JMP	0x0000:start;7C0h=>[CS]
;_________________________________________________________________
start:
;        MOV     AX,CS;[CS]=>[AX]
;        MOV     DS,AX;[AX]=>[DS]
;        MOV     ES,AX;[AX]=>[ES]
;        MOV     SS,AX;[AX]=>[SS]
	MOV	[dev],DL;Nr dispozitivului =>variabila dev
;_________________________________________________________________
	CALL	cls
	MOV	[tCol],02h
	MOV	SI,msg1;Ofsetul mesajului in SI
	CALL	print;Afisam msg1
;        MOV     SI,msg3
;        CALL    print
;        CALL    read
;        PUSH    [aux]
;        PUSH    1
;        POP     [LBA]
	CALL	LBA_CHS;convertim sin LBA in CHS
	CALL	stage2;Incarcam de la CHS:0:0:2=>[1000:0000]
	CALL	pau
entr:
	xor ax,ax
	mov ds,ax
	mov es,ax

	lgdt  [gdtr]   ; Load descriptors table

	in al, 0x92  ; Turn on A20 allow addressing over 1M
	or al, 2
	out 0x92, al

	mov eax, cr0  ; Turn on PE bit in CR0 (I.e. turn on protected mode)
	or al, 1
	mov cr0, eax
	jmp 0x8:_protected; initialize cs by this jump. 32 bit instructions will be after that

use32
_protected:
  ; jump to kernel
  jmp 0x8:0x10000

use16
;_________________________________________________________________
stage2:;rutina de citire a unui sector de pe purtator intr-o zona de memorie
	PUSHA;Incarcam registrii cu destin generala in stiva
	MOV	AH,02h;Citire sectoare
	MOV	AL,20d;Un sector
	MOV	DL,[dev];nr dispozitivului
	MOV	CX,[C];Transferam cilindrii in CX
	XCHG	CH,CL;interschimbam CH cu CL
	OR	CL,[S];formam in CX combinatia C:S
	MOV	DH,[H];transferam in DH nr de capuri
	PUSH	1000h;transmitem prin stack
	POP	ES;adresa de segment la care se va efectua scrierea sectorului citit
	XOR	BX,BX;vom incarca la 1000:0000
	INT	13h;Efectuam citirea
	POPA;Restabilim registrii cu destinatie generala
RET
;_________________________________________________________________
LBA_CHS:
	PUSHA ;incarcam in stack registrii cu destinatie generala
	MOV	AH,08h;citirea parametrilor dispozitivului
	MOV	DL,[dev];nr dispozitivului
	INT	13h;citim parametrii
	AND	CL,00111111b;eliminam 2 biti superiori din SPT
	INC	DH;incrementam HPC pentru a primi valoarea reala
	PUSH	CX DX;stokam CX si DX in memorie
;________________
	XOR	AX,AX;Zerografiem AX
	MOV	AL,DH;HPC =>AL
	MUL	CL;HPC*SPT =>AX
	XOR	DX,DX;zerografiem DX
	MOV	BX,[LBA];LBA=>BX
	XCHG	AX,BX;HPC*SPT =>BX; LBA=>AX
	DIV	BX;LBA/HPC*SPT=>AX
	MOV	[C],AX;nr cilindrilor in variabila respectiva
;________________
	POP	DX CX;restabilim HPC, SPT
	PUSH	CX DX;Stockam HPC,SPT
;________________
	MOV	AX,[LBA];LBA=>AX
	DIV	CL;LBA/SPT=>AL
	XOR	AH,AH;zerografiem AH
	DIV	DH;(LBA/SPT)%HPC=>AH
	MOV	[H],AH;nr capurilor in variabila respectiva
;________________
	POP	DX CX;restabilim HPC, SPT
;________________
	MOV	AX,[LBA];LBA=>AX
	DIV	CL;LBA%SPT=>AH
	INC	AH;(LBA%SPT)+1=>AH
	MOV	[S],AH;nr sectoarelor in variabila respectiva
;________________
	POPA
RET
;_________________________________________________________________
msg1	DB	"Stage 1.",10,13,00;Mesaj afisat cin copia programului primeshte controlul
;msg3    DB      "Introduceti LBA sectorului citit:",10,13,00;Mesaj de eroare
dev	DB	00h;Nr dispozitivului
C	DW	0000h;Cilindri
H	DB	00h;Capuri
S	DB	00h;Sector
LBA	DW	01d ;LBA
include "io.asm"

align 4

gdt:			; Address for the GDT
gdt_null:	      ; Null Segment
	dd 0
	dd 0

gdt_code:		; Code segment, read/execute, nonconforming
	dw 0FFFFh
	dw 0
	db 0
	db 0x9a
	db 0xcf
	db 0

gdt_data:		; Data segment, read/write, expand down
	dw 0FFFFh
	dw 0
	db 0
	db 0x92
	db 0xcf
	db 0
		
gdt_code16:
	dw 0FFFFh
	dw 0
	db 0
	db 0x9e
	db 0
	db 0

gdt_data16:
	dw 0FFFFh
	dw 0
	db 0
	db 0x92
	db 0
	db 0		

gdtr:
	dw gdtr-gdt_null-1
	dd gdt

finish:
TIMES	0x01FE-finish+sof   DB 00h;Umplem cu zerouri pina la 510 bytes
	DW 0AA55h;Ultimii byti vor contine signatura MBR
