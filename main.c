__asm
	(
		".code16gcc\n"
		"JMP hilevel\n"
	);

/*Primirea EIP*/
__asm(

	"get_eip: MOVL (%ESP),%EAX\n"
    "ret\n"

);
	
/*Interupt 0x1C
 * Timer*/
__asm
	(
"i1c:\n"
		"PUSHL %EAX\n"
		"PUSHL %EDX\n"
		"XOR %EAX,%EAX\n"
		"MOV $0x10,%AX\n"
		"MOV %ES,%DX\n"
		"INT $0x50\n"
		"POPL %EDX\n"
		"POPL %EAX\n"
		"IRETW\n"
	);
	
/*Interupt 0x50
 * SysCall */
__asm
	(
"i50:\n" 
		"PUSHW %DS\n"
		"PUSHW %ES\n"
		"PUSHW %FS\n"
		"PUSHW %GS\n"
		"PUSH %EAX\n"
		"MOV %CS,%AX\n"
		"MOV %AX,%DS\n"
		"MOV %AX,%ES\n"
		"POP %EAX\n"
		"PUSHL %EBP\n"
		"MOVL %ESP, %EBP\n"
		"PUSH %EDX\n"
		"PUSH %EBX\n"
		"PUSH %EAX\n"
		"CALL syscall\n"
		"MOVL %EBP, %ESP\n"
		"POP %EBP\n"
		"POPW %GS\n"
		"POPW %FS\n"
		"POPW %ES\n"
		"POPW %DS\n"
		"IRETW\n"
	);
	
/* Interupt 0x51
 * Process Terminate*/
 /*__asm(
 
	
 
 );*/

__asm("hilevel:\n");

#include "io.h"

typedef struct
	{
		unsigned int c;
		unsigned char h;
		unsigned char s;
	} chs;

typedef struct
	{
		unsigned short flags;
		unsigned short di;
		unsigned short si;
		unsigned short seg; 
		unsigned short ebp;
		unsigned short esp;
		
	} context;

void replace(unsigned short n,unsigned short *off,unsigned short *seg);
unsigned char strc(char *str1,char *str2);
unsigned short salloc();
chs lba_to_chs(unsigned long int lba);
void load_sector(unsigned int nr,unsigned short sector);
void syscall(unsigned short nr, unsigned short offset, unsigned short segment);
void exit(void);
void save_context(unsigned char nr,unsigned short ip);
void load_context(unsigned char nr);
void interpret(void);

extern char color;
	
unsigned char process_table[] = {1,0,0,0,0,0};
context context_table[6];
unsigned short p_seg = 0x0000;
unsigned long p_sec = 0x00000000;

char hot_key = 0;
unsigned char block = 0;
unsigned char process = 0;
unsigned short off=0,seg=0;

int main(void)
    {
		unsigned int nr=0;
		
		__asm/*Initilizam registrii de segment*/
			(
				"MOV	%CS,%AX\n"
				"MOV	%AX,%DS\n"
				"MOV	%AX,%ES\n"
				"MOV	%AX,%FS\n"
				"MOV	%AX,%SS\n"
				"MOV	%AX,%GS\n"
			);
			
		/*Instalam syscall intreruperea*/
		__asm
			(	
				"MOV $i50,%%AX\n"
				"MOV %%AX,%0\n"
				"MOV %%DS,%1\n"
				:				
				:"m"(off),"m"(seg)
			);
		replace(0x0050,&off,&seg);
		__asm
			(	
				"MOV $i1c,%%AX\n"
				"MOV %%AX,%0\n"
				"MOV %%DS,%1\n"
				:				
				:"m"(off),"m"(seg)
			);
		replace(0x001C,&off,&seg);
		clear();/*Curatam ecranul*/
		
		color=(BG_BLACK|FG_LIGHT_BLUE);/*Culoare text albastru deskis fon negru*/
		
		prints("Micro0S 0.0.6540 [preAlpha]\n");
		
		prints("\\>Tastati enter/esc pentru a schimba intreruperea.\n");
		
		while(1) {
		
			if(hot_key == 0x1C)	{
				
				interpret();
				color=(BG_BLACK|FG_LIGHT_BLUE);/*Culoare text albastru deskis fon negru*/
				hot_key = 0;
				
			}	
				
		}
		
		return 0;
    }
    
void interpret(void){
	
					char s[100];
					color=(BG_BLACK|FG_LIGHT_BLUE);
					prints("\\>");
					getl(s);
					if(!strc(s,"shutdown"))	{
						color=(BG_BLACK|FG_LIGHT_RED);
						prints("Shutdown command received\nPress any key to shutdown");
						getch();
						/*Setam starea dispozitivelor in power off utilizind  APM*/
						__asm	(
							"MOV $0x53,%AH\n"
							"MOV $0x07,%AL\n"
							"MOV $0x0001,%BX\n"
							"MOV $0x0003,%CX\n"
							"INT $0x15\n"
						);
					} else if(!strc(s,"reboot"))	{
						color=(BG_BLACK|FG_LIGHT_RED);
						prints("Reset command received\nPress any key to reset.");
						getch();
						unsigned char good = 0x02;
					    while ((good & 0x02) != 0)
							good = inportb(0x64);
						/*Trimitem CTRL-ALT-DEL pe portul keyboard*/
					    outportb(0x64, 0xFE);
					} else if(!strc(s,"exec"))	{
						prints("Introduceti nr sectorului: ");
						getl(s);
						color=(BG_BLACK|FG_LIGHT_RED);
						p_sec = atoi(s);
						p_seg = salloc();
						load_sector(p_sec,p_seg);
						prints("\n\r");
						color=(BG_BLACK|FG_WHITE);
						
						__asm(
							"PUSHA\n"
							"LEA %0,%%DI\n"
							"PUSHF\n"
							"POP %%AX\n"
							"MOV %%AX,%%GS:(%%DI)\n"
							"ADD $2,%%DI\n"
							"MOV %%DI,%%GS:(%%DI)\n"
							"ADD $2,%%DI\n"
							"MOV %%SI,%%GS:(%%DI)\n"
							"ADD $2,%%DI\n"
							"PUSH %%FS\n"
							"POP %%AX\n"
							"MOV %%AX,%%GS:(%%DI)\n"
							"ADD $2,%%DI\n"
							"MOV %%BP,%%GS:(%%DI)\n"
							"ADD $2,%%DI\n"
							"MOV %%SP,%%GS:(%%DI)\n"
							"POPA\n"
							:"=m"(context_table[0])
						);
						
						process = 1;
						__asm(
						
								"MOVW %0,%%AX\n"
								"MOVW %%AX,%%DS\n"
								"MOVW %%AX,%%ES\n"
								//"PUSHL %%EBP\n"
								//"MOVL %ESP, %EBP\n"
								//"LJMP $0x2000,$0x0000\n"
								"PUSHW $0x0000\n"
								"PUSHW %%AX\n"
								"PUSHW $0x0000\n"
								"IRETW\n"
								"ret_point:\n"
								"CLI\n"
								"MOV $0x20,%%AL\n"
								"OUT %%AL,$0x20\n"
								"STI\n"
								/*"MOV %SS,%AX\n"
								"MOV %AX,%DS\n"
								"MOV %AX,%CS\n"*/
								//"MOVL %EBP, %ESP\n"
								//"POP %EBP\n"
								:
								:"m" (p_seg)
							);
							
						process = 0;
							
					}	else {
						color=(BG_BLACK|FG_LIGHT_RED);
						prints("Unknown command\r\n");
					}
					
					
	
}

void replace(unsigned short n,unsigned short *off,unsigned short *seg)
	{
		unsigned short origoff=0,
					origseg=0;
		n*=4;
		__asm
			(	
				"CLI\n"
				"PUSHW %%GS\n"
				"PUSHW %%DI\n"
				"PUSHW %%AX\n"
				"PUSHW $0x0000\n"
				"POPW %%GS\n"
				"MOVW %0,%%DI\n"
				"MOVW %%GS:(%%DI),%%AX\n"
				"MOVW %%AX,%1\n"
				"INCW %%DI\n"
				"INCW %%DI\n"
				"MOVW %%GS:(%%DI),%%AX\n"
				"MOVW %%AX,%2\n"
				"STI\n"
				:"=m"(n)
				:"m"(origoff),"m"(origseg)
			);
		__asm
			(	
				"MOVW %0,%%DI\n"
				"MOVW %1,%%AX\n"
				"MOVW %%AX,%%GS:(%%DI)\n"
				"INCW %%DI\n"
				"INCW %%DI\n"
				"MOVW %2,%%AX\n"
				"MOVW %%AX,%%GS:(%%DI)\n"
				"POPW %%AX\n"
				"POPW %%DI\n"
				"POPW %%GS\n"
				"STI\n"
				:"=m"(n),"=m"(*off),"=m"(*seg)
			);
		*off=origoff;
		*seg=origseg;
	}


unsigned char strc(char *str1,char *str2)	{
	while(*str1++==*str2++ && *(str1-1)!='\0'&& *(str2-1)!='\0');
	return *(str2-1)-*(str1-1);
}
	

unsigned short salloc(){
	
	short i = 0;
	short seg = 0;
	prints("Alocam segmentul pentru proces...\n\r");
	
	for(; i < 6; i++)
		if(process_table[i] != 1)	{
			
			seg = 0x1000 * (i + 1);
			prints("SEG = 0x");
			printh(seg);
			prints("\n\r");
			process_table[i] = 1;
			
			return seg; 
		}
	
	return 0;
	
}

chs lba_to_chs(unsigned long int lba){
	
	unsigned char SPT = 0,
		HPC = 0;
	
	prints("Citim parametrii purtatorului...\n\r");
	
	__asm(
	
		"PUSHAW\n"
		"PUSH %%GS\n"
		"MOV $0x08,%%AH\n"
		"MOV $0x80,%%DL\n"
		"INT $0x13\n"
		"LEA %0,%%DI\n"
		"MOV %%DH,%%GS:(%%DI)\n"
		"AND $0xC0,%%CL\n"
		"LEA %1,%%DI\n"
		"MOV %%CL,%%GS:(%%DI)\n"
		"POP %%GS\n"
		"POPAW\n"
		:"=m"(HPC), "=m"(SPT)
		
	);
	
	prints("HPC = ");
	printd(HPC);
	prints("; SPT = ");
	printd(SPT);
	prints(";\n\r");
	
	chs aux;
	
	prints("Transformam LBA -> CHS...\n\r");
	
	aux.c = lba / (SPT * HPC);
	aux.h = (lba / SPT) % HPC;
	aux.s = (lba % SPT) + 1;
	
	prints("C = ");
	printd(aux.c);
	prints("; H = ");
	printd(aux.h);
	prints("; S = ");
	printd(aux.s);
	prints(";\n\r");
	
	return aux;
	
}

void load_sector(unsigned int lba, unsigned short sector){
	
	chs aux = lba_to_chs(lba);
	__asm(
		
			"PUSHAW\n"
			"PUSH %%ES\n"
			"PUSH %%GS\n"
			"MOV %3,%%AX\n"
			"MOV %%AX,%%ES\n"
			"MOV $0x02,%%AH\n"
			"MOV $0x01,%%AL\n"
			"XOR %%BX,%%BX\n"
			"XOR %%CX,%%CX\n"
			"LEA %0,%%DI\n"
			"MOVW %%GS:(%%DI),%%CX\n"
			"LEA %2,%%DI\n"
			"OR %%GS:(%%DI),%%CL\n"
			"LEA %1,%%DI\n"
			"MOV %%GS:(%%DI),%%DH\n"
			"MOV $0x80,%%DL\n"
			"CLC\n"
			"INT $0x13\n"
			"POP %%GS\n"
			"POP %%ES\n"
			"POPAW\n"
			"JNC err\n"
			: "=m" (aux.c), "=m"(aux.h), "=m"(aux.s)
			: "m" (sector)
		);
		
	prints("Eroare de citire.");
	
	__asm("err:\n");
	
	//prints("check");
	
	
}

void syscall(unsigned short nr,unsigned short offset, unsigned short segment){
	
	
	switch(nr)	{
		
		case 0x01:
		
			__asm(

				"PUSH %%FS\n"
				"MOV %0,%%DX\n"
				"MOV %%DX,%%FS\n"
				:"=m"(segment)
			);
		
			prints(offset);
			
			__asm(
			
				"POP %FS\n"
			
			);
			break;
			
		case 0x02:
		
			load_context(0);
			
			break;
			
		case 0x03:
		
			getch();
			
			break;
			
		case 0x10:	{
			
			if(hot_key)	{
				
				if(hot_key == 0x19 && process == 1)	{
					process = 0;
					
					
					__asm(

						"PUSH %%FS\n"
						"MOV %0,%%DX\n"
						"MOV %%DX,%%FS\n"
						:"=m"(segment)
					);
				
					__asm(
							"PUSHA\n"
							"LEA %0,%%DI\n"
							"PUSHF\n"
							"POPW %%AX\n"
							"MOV %%AX,%%GS:(%%DI)\n"
							"ADD $2,%%DI\n"
							"MOV %%DI,%%GS:(%%DI)\n"
							"ADD $2,%%DI\n"
							"MOV %%SI,%%GS:(%%DI)\n"
							"ADD $2,%%DI\n"
							"PUSHW %%FS\n"
							"POPW %%DX\n"
							"MOV %%DX,%%GS:(%%DI)\n"
							"ADD $2,%%DI\n"
							"MOV %%BP,%%GS:(%%DI)\n"
							"ADD $2,%%DI\n"
							"MOV %%SP,%%GS:(%%DI)\n"
							"POPA\n"
							:"=m"(context_table[1])
						);
					
					__asm(
					
						"PUSH %CS\n"
						"POP %FS\n"
					
					);
					
					load_context(0);
					
				} else if (hot_key == 0x19 && process == 0){
					
					process = 1;
					
					color=(BG_BLACK|FG_WHITE);
						
					__asm(
						"LEA %0,%%DI\n"
						"MOV %%GS:(%%DI),%%AX\n"
						"PUSH %%AX\n"
						"POPF\n"
						"ADD $2,%%DI\n"
						//DI aici
						"ADD $2,%%DI\n"
						"MOV %%GS:(%%DI),%%SI\n"
						"ADD $2,%%DI\n"
						"MOV %%GS:(%%DI),%%AX\n"
						"MOV %%AX,%%DS\n"
						"MOV %%AX,%%ES\n"
						"MOV %%AX,%%FS\n"
						"MOV %%AX,%%GS\n"
						"ADD $2,%%DI\n"
						"MOV %%GS:(%%DI),%%AX\n"
						"MOV %%AX,%%BP\n"
						"ADD $2,%%DI\n"
						"MOV %%GS:(%%DI),%%SP\n"
						//"STI\n"
						"PUSHF\n"
						"PUSHW %%FS\n"
						"LEA %0,%%DI\n"
						"PUSHW $0x0000\n"
						"IRETW\n"
						:
						:"m"(context_table[1])
					);
				}
				
				hot_key = 0;
				
			} else {
				
				hot_key = lastkey();
				
			}
		
			
			}
			break;

	}

}
	
void exit(void){
	
	
	
}

void save_context(unsigned char nr, unsigned short ip){
	
	__asm(
		"LEA %0,%%DI\n"
		"MOV %1,%%AX\n"
		"MOV %%AX,%%GS:(%%DI)\n"
		"ADD $2,%%DI\n"
		"PUSHF\n"
		"POP %%AX\n"
		"MOV %%AX,%%GS:(%%DI)\n"
		"ADD $2,%%DI\n"
		"MOV %%DI,%%GS:(%%DI)\n"
		"ADD $2,%%DI\n"
		"MOV %%SI,%%GS:(%%DI)\n"
		"ADD $2,%%DI\n"
		"PUSH %%FS\n"
		"POP %%AX\n"
		"MOV %%AX,%%GS:(%%DI)\n"
		"ADD $2,%%DI\n"
		"MOV %%BP,%%GS:(%%DI)\n"
		"ADD $2,%%DI\n"
		"MOV %%SP,%%GS:(%%DI)\n"
		:"=m"(context_table[nr])
		:"m"(ip)
	);
	/*
	printh(context_table[nr].ip);
	prints("\n\r");
	printh(context_table[nr].flags);
	prints("\n\r");
	printh(context_table[nr].di);
	prints("\n\r");
	printh(context_table[nr].si);
	prints("\n\r");
	printh(context_table[nr].seg);
	prints("\n\r");
	printh(context_table[nr].ebp);
	prints("\n\r");
	printh(context_table[nr].esp);
	prints("\n\r");*/
	
}


void load_context(unsigned char nr){
	/*
	printh(context_table[nr].ip);
	prints("\n\r");
	*/
	__asm(
		"LEA %0,%%DI\n"
		"MOV %%GS:(%%DI),%%AX\n"
		"PUSH %%AX\n"
		"POPF\n"
		"ADD $2,%%DI\n"
		//DI aici
		"ADD $2,%%DI\n"
		"MOV %%GS:(%%DI),%%SI\n"
		"ADD $2,%%DI\n"
		"MOV %%GS:(%%DI),%%AX\n"
		"MOV %%AX,%%DS\n"
		"MOV %%AX,%%ES\n"
		"MOV %%AX,%%FS\n"
		"MOV %%AX,%%GS\n"
		"ADD $2,%%DI\n"
		"MOV %%GS:(%%DI),%%AX\n"
		"MOV %%AX,%%BP\n"
		"ADD $2,%%DI\n"
		"MOV %%GS:(%%DI),%%SP\n"
		//"STI\n"
		"PUSHF\n"
		"PUSHW $0x0810\n"
		"PUSHW $ret_point\n"
		"IRETW\n"
		:
		:"m"(context_table[nr])
	);
	/*
	printh(context_table[nr].ip);
	prints("\n\r");
	printh(context_table[nr].flags);
	prints("\n\r");
	printh(context_table[nr].di);
	prints("\n\r");
	printh(context_table[nr].si);
	prints("\n\r");
	printh(context_table[nr].seg);
	prints("\n\r");
	printh(context_table[nr].ebp);
	prints("\n\r");
	printh(context_table[nr].esp);
	prints("\n\r");*/
}

