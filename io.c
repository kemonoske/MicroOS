__asm
	(
		".code16gcc\n"
	);
	
#include "io.h"
	
char color=(BG_BLACK|FG_WHITE);

void prints(char *str)
	{
		char chr;
		static unsigned char X=0,Y=0;
		unsigned short offset=160*Y+X;
		__asm
			(	
				"PUSHW %%GS\n"
				"PUSHW %%DI\n"
				"PUSHW $0xB800\n"
				"POPW %%GS\n"
				"MOVW %0,%%DI\n"
				:"=m"(offset)
			);
		__asm(
			"PUSH %%DI\n"
			"MOV %0,%%DI\n"
			"MOV %%FS:(%%DI),%%AH\n"
			"MOV %%AH,%1\n"
			"POP %%DI\n"
			:"=m"(str)
			:"m" (chr)
		);
		
		while(chr!='\0')
			{
				if(chr=='\n')
					{
						X=0;
						Y++;
						offset=160*Y+X;
						__asm

							(
								"MOVW %0,%%DI\n"
								:"=m"(offset)
							);
						str++;
				
						__asm(
							"PUSH %%DI\n"
							"MOV %0,%%DI\n"
							"MOV %%FS:(%%DI),%%AH\n"
							"MOV %%AH,%1\n"
							"POP %%DI\n"
							:"=m"(str)
							:"m" (chr)
						);
						continue;
					}	
				else if(chr=='\r')
					{
						X=0;
						offset=160*Y+X;
						__asm

							(
								"MOVW %0,%%DI\n"
								:"=m"(offset)
							);
						str++;
				
						__asm(
							"PUSH %%DI\n"
							"MOV %0,%%DI\n"
							"MOV %%FS:(%%DI),%%AH\n"
							"MOV %%AH,%1\n"
							"POP %%DI\n"
							:"=m"(str)
							:"m" (chr)
						);
						continue;
					}	
				__asm
					(
						"MOVB %0,%%AH\n"
						"MOVB %%AH,%%GS:(%%DI)\n"
						"INCW %%DI\n"
						"MOVB %1,%%AH\n"
						"MOVB %%AH,%%GS:(%%DI)\n"
						"INCW %%DI\n"
						:"=m"(chr),"=m"(color)
					);
					
				offset+=2;
				Y+=(X==158)?1:0;
				X=(X==158)?0:X+2;
				if(Y>=25)	{
					Y = 0;
					X = 0;	
					offset=160*Y+X;
					__asm

						(
							"MOVW %0,%%DI\n"
							:"=m"(offset)
						);
					clear();
					continue;
				}
				str++;		
				
				__asm(
					"PUSH %%DI\n"
					"MOV %0,%%DI\n"
					"MOV %%FS:(%%DI),%%AH\n"
					"MOV %%AH,%1\n"
					"POP %%DI\n"
					:"=m"(str)
					:"m" (chr)
				);
			}
		__asm
			(
				"POPW %DI\n"
				"POPW %GS\n"
			);
	}

unsigned char lastkey(void)
	{
		unsigned char key=0;
		__asm
			(
				"PUSHW %%AX\n"
				"MOVB $0x11,%%AH\n"
				"INT $0x16\n"
				"JZ kbd_handler\n"
				"MOVB $0x0010,%%AH\n"
				"INT $0x16\n"
				"MOVB %%AH,%0\n"
				"kbd_handler:\n"
				"POPW %%AX\n"
				:
				:"m"(key)
			);
		return key;
	}

void printd(unsigned long n)
	{
		static char *string="\r\r\r\r\r\r\r\r\r\r";
		char *p=string+9;
		if(!n)
			*p='0';
		while(n>0)
			{
				*p--=n%10+'0';
				n/=10;
			}	
		while(*p=='\r')
			p++;		
		prints(p);
		while(p<=(string+9))
			*p++='\r';	
	}

void printh(unsigned long n)
	{
		static char *string="\r\r\r\r\r\r\r\r\r\r";
		char *p=string+9;
		if(!n)
			*p='0';
		while(n>0)
			{
				*p--=n%0x10+(((n%0x10)<10)?'0':('A'-10));
				n/=16;
			}	
		while(*p=='\r')
			p++;		
		prints(p);
		while(p<=(string+9))
			*p++='\r';	
	}

unsigned char getch(void)
	{
		char c = '\r';
		
		__asm
			(	
				"PUSHW %%AX\n"
				"PUSHW %%DI\n"
				"XOR %%AX,%%AX\n"
				"XOR %%DX,%%DX\n"
				"MOV $0x10,%%AH\n"
				"INT $0x16\n"
				"MOV %%AL,%0\n"
				:"=m"(c)
			);
		if(!c)
			__asm
				(
					"MOV %%AH,%0\n"
					:
					:"m"(c)
				);
		__asm(
		"POPW %DI\n"
		"POPW %AX\n"
		);
		return c;
	}

void clear(void){
	
		__asm
			(	
				"PUSHW %GS\n"
				"PUSHW %DI\n"
				"PUSHW $0xB800\n"
				"POPW %GS\n"
				"XORW %DI,%DI\n"
			);
		unsigned short i=0;		
		while(i++<4000)		
			__asm
				(
					"MOVB $0x00,%GS:(%DI)\n"
					"INCW %DI\n"				
				);
		__asm
			(
				"POPW %DI\n"
				"POPW %GS\n"
			);
			
	}

void getl(char *s)	{
	
	while((*s++=getch())!='\r')	{
		*s='\0';
		prints(s-1);
	}
	*(s-1)='\0';
	prints("\n");
}

unsigned long atoi(char *str){
	
	unsigned long nr = 0;
	unsigned int i = 0;
	
	while(*str != '\0'){
		
		if(*str >= '0' || *str <= '9')
			nr = nr * 10 + (*str - '0');
		
		str++;
	}
	
	return nr;
	
}

inline unsigned char inportb(unsigned int port){
	
	unsigned char ret;
	asm volatile ("inb %%dx,%%al":"=a" (ret):"d" (port));
	return ret;
	
}

inline void outportb(unsigned int port,unsigned char value){
	
	asm volatile ("outb %%al,%%dx": :"d" (port), "a" (value));
	
}	
	
