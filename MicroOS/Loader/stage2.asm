;TIMES   512*31    DB      00h;pentru a ocupa locul LBA=32 in binar
use16;
org 0000h
;__________________________________________________________
JMP	07C0h:start;7C0h=>[CS]
;__________________________________________________________
start:
	MOV	AX,CS;[CS]=>[AX]
	MOV	DS,AX;[AX]=>[DS]
	MOV	ES,AX;[AX]=>[ES]
	MOV	SS,AX;[AX]=>[SS]
;__________________________________________________________
	CALL	cls
	MOV	[tCol],04h;Culoare Rosie
	MOV	SI,msg1;Ofsetul la mesaj
	CALL	print;Afisam mesajul
	CALL   pau;tastarea oricarei taste
	JMP	$
;__________________________________________________________
msg1   DB      "Stage 2:Sistemul de operare al studentului Bostanica Ion.",10,13,00
include "io.asm"
TIMES	512-$	DB 00h
;TIMES   1457664 DB 00h