;Incaracator nivelul 1 aflat la CHS:0:0:1
;Incarca in memorie alt incarcator de la LBA 1
use16;Cod 16 bit
org 0000h;Offsetul 0 de la adresa controlului
;_________________________________________________________________
JMP	07C0h:start;7C0h=>[CS]
;_________________________________________________________________
start:
	MOV	AX,CS;[CS]=>[AX]
	MOV	DS,AX;[AX]=>[DS]
	MOV	ES,AX;[AX]=>[ES]
	MOV	SS,AX;[AX]=>[SS]
	MOV	[dev],DL;Nr dispozitivului =>variabila dev
;_________________________________________________________________
after:
	CALL	cls
	MOV	[tCol],02h
	MOV	SI,msg1;Ofsetul mesajului in SI
	CALL	print;Afisam msg2
	MOV	SI,msg2
	CALL	print
	CALL	read
	PUSH	[aux]
	POP	[LBA]
	CALL	LBA_CHS;convertim sin LBA in CHS
	CALL	stage2;Incarcam de la CHS:0:0:2=>[07C0:0000]
JMP	0800h:0000h;Transferam controlul codului incarcat
;_________________________________________________________________
stage2:;rutina de citire a unui sector de pe purtator intr-o zona de memorie
	PUSHA;Incarcam registrii cu destin generala in stiva
	MOV	AH,02h;Citire sectoare
	MOV	AL,04d;Un sector
	MOV	DL,[dev];nr dispozitivului
	MOV	CX,[C];Transferam cilindrii in CX
	XCHG	CH,CL;interschimbam CH cu CL
	OR	CL,[S];formam in CX combinatia C:S
	MOV	DH,[H];transferam in DH nr de capuri
	PUSH	0800h;transmitem prin stack
	POP	ES;adresa de segment la care se va efectua scrierea sectorului citit
	XOR	BX,BX;vom incarca la 07C0:0000
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
msg1	DB	"Stage 1.",10,13,00;Mesaj afisat la incarcarea LBA 0 de BIOS
msg2	DB	"Introduceti LBA sectorului citit:",10,13,00;Mesaj de eroare
copy	DB	00h;Va fi 1 dupa autocopiere
LBA	DW	01d ;LBA
include "io.inc"
TIMES	510-$	DB 00h;Umplem cu zerouri pina la 510 bytes
	DW 0AA55h;Ultimii byti vor contine signatura MBR
