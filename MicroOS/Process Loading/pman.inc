;__________________________________________
;include 'io.inc'
;__________________________________________
TASKS DB 0x00
FIRST_SEG		EQU	0x1000	    ;first empty segment
LAST_SEG		EQU	0x9000
SEG_NUM 		EQU	9
task_table		DW	0x7FFF
TASK_CODE_START 	EQU	0x0100
TASK_SECTOR		DW	0x0003
TASK_POINTER		DD	0x00000000
full_stack_msg	DB	"Eroare: Toate 15 segmente de procese sunt incarcate.",10,13,00
number_printer			   DB	  00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00
;;___________________________________________________
malloc:
    PUSHA
    PUSHF

    MOV     BX, word FIRST_SEG
    XOR     AX, AX
    INC     AX
    MOV     CX, word SEG_NUM


    malloc_loop:
	TEST	[CS:task_table], AX
	JNZ	malloc_exit_loop
	ADD	BX, 1000h
	SHL	AX, 1
    LOOP	malloc_loop

    malloc_exit_loop:

	TEST	CX, CX
	JNZ	malloc_no_error
	MOV	SI,full_stack_msg
	CALL	print
	POP	AX
	OR	AX,0x100 ;set zero flag
	PUSH	AX
       POPF
	POPA
	RET

    malloc_no_error:
	OR     [CS:task_table], AX

    MOV     ES, BX

    POPF
    POPA
RET
;____________________________________________________________
exec:
    PUSHF
    PUSH AX BX CX
    ;Primim din AX parametrul
    MOV      [TASK_SECTOR],AX
    ;Alocarea segmentului pentru procedura
    CALL      malloc

    MOV      AX,[TASK_SECTOR]
    AND      AX,0xFF00
    SHR      AX,8
    CALL     hex
    MOV      AX,[TASK_SECTOR]
    AND      AX,0x00FF
    CALL     hex
    ;Calcularea din LBA->CHS a sectorului cu procedura
    CALL     LBA_CHS

    ;Citirea procedurii in segmentul alocat
    CALL     read_sect


    ;Stocam pointerul 32 bit la proces in memorie
    ;pentru realizarea saltului intersegment
    MOV     word[TASK_POINTER+2],ES
    MOV     word[TASK_POINTER],0x0000
    MOV     AX,ES
    MOV     DS,AX;[AX]=>[DS]
    ;MOV     SS,AX;[AX]=>[S]
    ;OR      AX,0xFFFF
    MOV     SP,AX
    MOV     BP,SP
    ;MOV     ES,AX;[AX]=>[ES]
    ;MOV     FS,AX;[AX]=>[ES]
    ;MOV     GS,AX;[AX]=>[ES]
    ;MOV     SS,AX;[AX]=>[SS]
    ;MOV      SI,0x0000
    ;CALL     print
    ;PUSH     ES
    ;POP      CS

    JMP     dword[GS:TASK_POINTER]
;doStart8:
;        mov al,[tasknum]
;
;        inc al
;        mov [tasknum],al
;        ;Marim numarul de lucrari
;        mov [cs:di],al
;        ; Salvam punctul de intrare în descriptorul lucrarilor
;        mov [Word Ptr cs:di+1],100h
;        mov [cs:di+3],es
;        ;Salvam punctul de intrare în descriptorul lucrarilor  ES:100h
	; Pregatim ambele stive
;        mov [Word Ptr es:0FFFEh],00000h ; 0– daca programul se va termina
;        ; cu RET, atunci CS:IP=CS:0000,
;        ;iar  acolo int 20h
;        mov [Word Ptr es:0FFFCh],00232h ;FLAGS
;        mov [Word Ptr es:0FFFAh],es ;CS PSP
;        mov [Word Ptr es:0FFF8h],00100h ; IP 100h
;        mov [Word Ptr es:0FFF6h],00111h ; DI 0000
;        mov [Word Ptr es:0FFF4h],00222h ; BX 0000
;        mov [Word Ptr es:0FFF2h],00333h ; AX 0000
;        mov [Word Ptr es:0FFF0h],es ; ES PSP
;        mov [Word Ptr es:0FFEEh],es ; DS PSP
;        mov [Word Ptr es:0FFECh],00000h ;SI 0000
;        mov [Word Ptr cs:di+513],0FFECh ;SS FFEC
;        mov [Word Ptr cs:di+511],ES ;SP seg
;        mov [Word Ptr cs:di+509],00000h ;CX 0000
;        mov [Word Ptr cs:di+507],00000h ;DX 0000
;        mov [Word Ptr cs:di+505],00000h ;BP 0000
    POP CX BX AX
    POPF
RET
;____________________________________________________________
LBA_CHS:
	PUSH	ES
	PUSHA ;incarcam in stack registrii cu destinatie generala
	PUSHF
	MOV	AH,08h;citirea parametrilor dispozitivului
	MOV	DL,[dev];nr dispozitivului
	INT	13h;citim parametrii
	;JC      exit
	AND	CL,00111111b;eliminam 2 biti superiori din SPT
	INC	DH;incrementam HPC pentru a primi valoarea reala
	PUSH	CX DX;stokam CX si DX in memorie
;________________
	XOR	AX,AX;Zerografiem AX
	MOV	AL,DH;HPC =>AL
	MUL	CL;HPC*SPT =>AX
	XOR	DX,DX;zerografiem DX
	MOV	BX,[TASK_SECTOR];LBA=>BX
	XCHG	AX,BX;HPC*SPT =>BX; LBA=>AX
	DIV	BX;LBA/HPC*SPT=>AX
	MOV	[C],AX;nr cilindrilor in variabila respectiva
;________________
	POP	DX CX;restabilim HPC, SPT
	PUSH	CX DX;Stockam HPC,SPT
;________________
	MOV	AX,[TASK_SECTOR];LBA=>AX
	DIV	CL;LBA/SPT=>AL
	XOR	AH,AH;zerografiem AH
	DIV	DH;(LBA/SPT)%HPC=>AH
	MOV	[H],AH;nr capurilor in variabila respectiva
;________________
	POP	DX CX;restabilim HPC, SPT
;________________
	MOV	AX,[TASK_SECTOR];LBA=>AX
	DIV	CL;LBA%SPT=>AH
	INC	AH;(LBA%SPT)+1=>AH
	MOV	[S],AH;nr sectoarelor in variabila respectiva
;_______________
	POPF
	POPA
	POP	ES
RET
;_________________________________________________________________
read_sect:;rutina de citire a unui sector de pe purtator intr-o zona de memorie
	PUSHA;Incarcam registrii cu destin generala in stiva
	PUSHF
	MOV	AH,02h;Citire sectoare
	MOV	AL,01h;4096 bytes
	MOV	DL,[dev];nr dispozitivului
	MOV	CX,[C];Transferam cilindrii in CX
	XCHG	CH,CL;interschimbam CH cu CL
	OR	CL,[S];formam in CX combinatia C:S
	MOV	DH,[H];transferam in DH nr de capuri
	;PUSH    07C0h;transmitem prin stack
	;POP     ES;adresa de segment la care se va efectua scrierea sectorului citit
	XOR	BX,BX;vom incarca la X000:0000
	INT	13h;Efectuam citirea
	POPF
	POPA;Restabilim registrii cu destinatie generala
RET
;_________________________________________________________________
; #proc free segment
; #input: es - segment
;free:
;    push    ax cx
;
;    mov     cx, es
;
;    cmp     cx, FIRST_SEG
;    jb      free_err
;    cmp     cx, LAST_SEG
;    ja      free_err
;
;    sub     cx, FIRST_SEG
;
;        shr             cx, 12
;    xor     ax, ax
;    inc     ax
;        test    cx, cx
;        jz              free_skip_shl
;        shl             ax, cl
;  free_skip_shl:
;    or      [cs:mem_table], ax
;    jmp     free_exit
;  free_err:
;    error   ERR_FREE
;  free_exit:
;    pop     cx ax
;    ret
;______________________________________________________