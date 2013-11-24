;TIMES   512*31    DB      00h;pentru a ocupa locul LBA=32 in binar
use16;
org 0x0000
sof:
;____________________________________________________________
include 'MicroOS.inc'
;__________________________________________________________
msg1	DB	"Process",10,13,00
;_________________________________________________________________
start:
		MOV	DX,ES
		MOV	BX,msg1
		syscall	PRINT
		nop;
		nop;
		nop;
		nop;
		nop;
		nop;
		nop;
		nop;
		nop;
		nop;
		nop;
		nop;
		nop;
		nop;
		nop;
		nop;
		nop;
		nop;
		nop;
		nop;
		nop;
		nop;
		nop;
		nop;
		nop;
		nop;
		nop;
JMP start
;______________________________________________________________
eof:
TIMES	512-eof+sof   DB 00h
;TIMES   1457664 DB 00h