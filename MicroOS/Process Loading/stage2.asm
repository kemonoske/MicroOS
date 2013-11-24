;TIMES   512*31    DB      00h;pentru a ocupa locul LBA=32 in binar
use16;
org 0x0000
sof:
;__________________________________________________________
JMP	0x0800:start;7C0h=>[CS]
;____________________________________________________________
include 'io.inc'
include 'pman.inc'
;__________________________________________________________
msg1	DB	"Kernel:Controlul este preluat de Kernel",10,13,00
msg2	DB	"Kernel:ES out of limits",10,13,00
;_________________________________________________________________
start:
	;Initializam registrii de segment
	MOV	AX,CS;[CS]=>[AX]
	MOV	DS,AX;[AX]=>[DS]
	MOV	ES,AX;[AX]=>[ES]
	MOV	FS,AX;[AX]=>[ES]
	MOV	GS,AX;[AX]=>[ES]
	MOV	SS,AX;[AX]=>[SS]
	;MOV     AX,0x7BFF
	;MOV     SP,AX
	;MOV     BP,SP
	;Initializam tabela de procese cu 0xFFFF
	;ce indica ca nu e pornit nici un proces
	MOV	 AX,0xFFFF
	MOV	 [CS:task_table],AX
;__________________________________________________________
	CALL	cls
	MOV	[tCol],04h;Culoare Rosie
	MOV	SI,msg1;Ofsetul la mesaj
	CALL	print;Afisam mesajul
	CALL	pau;tastarea oricarei taste
	MOV	AX,0x0003;Sectorul cu procesul
	CALL	exec;Executia
	MOV	SI,msg1;Ofsetul la mesaj
	CALL	print;Afisam mesajul
	CALL	pau
	JMP	0x0800:$
;______________________________________________________________
eof:
TIMES	1024-eof+sof   DB 00h
;TIMES   1457664 DB 00h