use32
org 0x10000
start:
  mov  ax, 0x10  ;  second descriptor is our data segment
  mov  ds, ax
  mov  es, ax
  mov  gs, ax
  mov  fs, ax
  mov  ss, ax
;  sti
	
;  mov  edi, 0xB8000  ; video memory address
;  mov  esi, hello  ; message
;  cld
	
;message_loop:
;  lodsb
;  test al, al
;  jz   exit
;  stosb
;  mov  al, 7
;  stosb
;  jmp  message_loop

	MOV	ESI,hello
	CALL	print

exit:
 
  cli
  hlt
  jmp exit
  
hello  db 'Hello World!',0
include "io.inc"