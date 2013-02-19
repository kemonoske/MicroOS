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
	CMP	[copy],00h;Daca nu e 00
	JNE	after;trecem peste procedura de copiere
	CALL	cls;Curatam ecran
	MOV	SI,msg1;Ofsetul mesajului in SI
	CALL	print;Afisam msg1
	CALL	pau
	MOV	[copy],01h;Indicam ca copierea a avut loc
	CALL	autocpy;Autocopierea la 7F00
after:
	CALL	cls
	MOV	[tCol],02h
	MOV	SI,msg2;Ofsetul mesajului in SI
	CALL	print;Afisam msg2
	MOV	SI,msg3
	CALL	print
	CALL	read
	PUSH	[aux]
	POP	[LBA]
	CALL	LBA_CHS;convertim sin LBA in CHS
	CALL	stage2;Incarcam de la CHS:0:0:2=>[07C0:0000]
JMP	07C0h:0000h;Transferam controlul codului incarcat
;_________________________________________________________________
autocpy:;Rutina de autocopiere a programului la alta adresa de memorie
	PUSH	SI DI CX;Stokam registrii folositi in stack
	XOR	SI,SI;[07C0:0000]=>
	MOV	DI,0300h;[07C0:0300]=[07F0:0000]
	MOV	CX,512d;transferam 512 bytes
	REP	MOVSB;operatia de transfer repetata
	POP	CX DI SI;restabilim registrii folositi din stack
JMP	07F0h:start;tranferam controlul la codul copiat
RET
;_________________________________________________________________
stage2:;rutina de citire a unui sector de pe purtator intr-o zona de memorie
	PUSHA;Incarcam registrii cu destin generala in stiva
	MOV	AH,02h;Citire sectoare
	MOV	AL,01h;Un sector
	MOV	DL,[dev];nr dispozitivului
	MOV	CX,[C];Transferam cilindrii in CX
	XCHG	CH,CL;interschimbam CH cu CL
	OR	CL,[S];formam in CX combinatia C:S
	MOV	DH,[H];transferam in DH nr de capuri
	PUSH	07C0h;transmitem prin stack
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
msg2	DB	"Stage 1 copy.",10,13,00;Mesaj afisat cin copia programului primeshte controlul
msg3	DB	"Introduceti LBA sectorului citit:",10,13,00;Mesaj de eroare
copy	DB	00h;Va fi 1 dupa autocopiere
dev	DB	00h;Nr dispozitivului
C	DW	0000h;Cilindri
H	DB	00h;Capuri
S	DB	00h;Sector
LBA	DW	01d ;LBA
include "io.asm"
TIMES	510-$	DB 00h;Umplem cu zerouri pina la 510 bytes
	DW 0AA55h;Ultimii byti vor contine signatura MBR
