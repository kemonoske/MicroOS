;_________________________________________________________________
;hex:
;        SHL     AX,4
;        SHR     AL,4
;        XCHG    AH,AL
;        CMP     AH,0Ah
;        JB      decah
;        ADD     AH,'A'-'0'-0Ah
;decah:
;        ADD     AH,'0'
;        CMP     AL,0Ah
;        JB      decal
;        ADD     AL,'A'-'0'-0Ah
;decal:
;        ADD     AL,'0'
;        MOV     word [bta],AX
;RET
;_________________________________________________________________
;read:;Citeshte de la utilizator o valoare zeecimala max 16 bit
;        PUSHA;stocam registrii cu destinatie generala in
;        MOV     [aux],0000h;zerografiem variabila auxiliara de citire
;get:
;        MOV     BX,10d;10=>BX
;        MOV     AH,00h;Citire tastatura
;        INT     16h
;        CMP     AH,0Eh;Daca
;        JE      bksp;e tastat back space salt la bksp
;        CMP     AH,1Ch;daca
;        JE      entr;e tastat enter salt la entr
;        CMP     AL,'0';daca codul ascii primit
;        JB      get;e mai mic ca codul ascii al caracterului '0' salt la get citim dinnou
;        CMP     AL,'9';daca codul ascii primit
;        JA      get;e mai mare ca codul ascii al caracterului '9' salt la get citim dinnou
;        MOV     [char],AL;transferam codul ascii a caracterului citit in variabila char
;        MOV     SI,char;transferam offsetul la variabila char in SI
;        CALL    print;afisam caracterul citit
;        SUB     [char],'0';scadem din codul ascii a caracterului codul ascii al caracterulu '0' si primim valoare cifrei
;        MOV     AX,[aux];transferam auxiliara in ax
;        MUL     BL;Inmultim valoarea curenta in auxiliara cu 10d
;        CLC     ;CF=0
;        ADD     AL,[char];Adunam AL cu cifra citita
;        ADC     AH,0;efectuam transferul
;        MOV     [aux],AX;AX=>aux
;        JMP     get;citim urmatorul caracter
;bksp:
;        CMP     [X],00h;daca nu e introdusa nici o valoare
;        JE      get;salt la get citim mai departe
;        SUB     [X],02h;bytes pe linie -2
;        MOV     [char],20h;codul ascii al caracterului space=>char
;        MOV     SI,char;offsetul la char =>SI
;        CALL    print;afisam un spatiu peste caracterul precedent afisat
;        SUB     [X],02h;bytes pe linie -2
;        MOV     AX,[aux];aux=>AX
;        XOR     DX,DX;zerografiem AX
;        CLC     ;CF=0
;        DIV     BX;[DX:AX]/10
;        MOV     [aux],AX;AX=>aux
;        JMP     get;citim urmatorul caracter
;entr:
;        POPA;restabilim registrii cu destinatie generala
;RET
;______________________________________
print:;rutina afiseaza un text ce se termina cu  caracterul NULL, ce se afla la adresa DS:SI
	PUSHA	;stokam registrii cu destinatie generala in stiva
	PUSH	ES;stocarea valorii ES in stiva
	PUSH	0B800h;Adresa buferului Video pentru modul 03h
	POP	ES;Adresa bufer video=>ES
	MOV	BL,[bkCol];Culoare fundal=>BL
	SHL	BL,4;permutam valoarea BL cu 4 biti la stinga
	OR	BL,[tCol];Culoare text tetrada inferioara din BL
			 ;Culoare fundal tetrada superioara din BL
type:
	XOR	AX,AX;zerografiem AX
	MOV	AL,160d;Nr maxim de caractere pe linie *2 2byte per char
	MUL	[Y];nr de bytes pe linie * nr liniei curente
	ADD	AL,[X];+nr bytes pe ultima linie
	ADC	AH,0;efectuam transferul de bit superior daca e cazul
	MOV	DI,AX;ofesetul unde trebuie sa fie inscris urmatorul caracter=>DI
	CMP	byte [SI],10d;daca caracterul afisat
	JNE	no_ret;nu e return trecem la no_ret
	MOV	[X],00h;daca e return nr de bytes pe linia curenta se zerografiaza
	INC	SI;trecem la urmatorul caracter care trebue afisat
	JMP	type;salt la type
no_ret:
	CMP	byte [SI],13d;daca caracterul afisat
	JNE	no_nl;nu e line feed salt la no_nl
	INC	[Y];incrementam linia cureta
	INC	SI;trecem la urmatorul caracter care trebuie afisat
	JMP	type;salt la type
no_nl:
	CMP	byte [SI],00h;daca caracterul afisat
	JE	xit;este NULL salt la xit si parasim rutina
	MOVSB	;[DS:SI]=>[ES:DI]afisarea caracterului(copierea in bufferul video)
	MOV	[ES:DI],BL;atributul caracterului afisat
	ADD	[X],02h;nr bytes pe linie +2
	JMP	type;salt la type
xit:
	POP	ES;restabilim ES din stiva
	POPA;restabilim registrii cu destinatie generala din stiva
RET
;______________________________________
pau:;Rutina foloseshte intreruperea 16h pentru a opri rularea pina nu e tastata o tasta
	PUSH	AX;stokam AX in stiva
	MOV	AH,00h;Citire tastatura
	INT	16h;apel intrerupere
	POP	AX;restabilim AX din stiva
RET
;______________________________________
cls:;Rutina seteaza modul Video curent si reseteaza indecsii de offset in buferul video
	PUSHA;stocam registrii cu destinatie generala in stack
	MOV	[X],00h;zerografiem bytii pe linia curenta
	MOV	[Y],00h;zerografiem nr liniei curente
	MOV	AX,0003h;Setarea  video|80x25 16 culori
	INT	10h
	POPA;restabilim din stack registrii cu destinatie generala
RET
;______________________________________
tCol	DB	07h;Culoarea, implicit alba
bkCol	DB	00h;Culoarea fundalului implicit neagra
X     DB	00h;pozitia X in buferul video
Y     DB	00h;pozitia Y
aux	DW	0000h;Auxiliara pentru citire
char	DB	00h,00h;variabila pentru ecou la citire