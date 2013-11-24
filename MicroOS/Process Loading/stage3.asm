;TIMES   512*31    DB      00h;pentru a ocupa locul LBA=32 in binar
use16;
org 0x0000
sof:
;__________________________________________________________
JMP	start;7C0h=>[CS]
;____________________________________________________________
include 'io.inc'
;__________________________________________________________
msg1	DB	"Process",10,13,00
;_________________________________________________________________
start:
	CALL	cls
	MOV	[tCol],07h;Culoare Rosie
	MOV	SI,msg1;Ofsetul la mesaj
	CALL	print;Afisam mesajul
	CALL	pau;tastarea oricarei taste
	JMP	0x2000:$
;______________________________________________________________
eof:
TIMES	512-eof+sof   DB 00h
;TIMES   1457664 DB 00h