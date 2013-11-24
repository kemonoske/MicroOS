__asm
	(
		".code16gcc\n"
	);
	
#ifndef IO_H
#define IO_H

#define FG_BLACK		 0x00
#define FG_BLUE  		 0x01
#define FG_GREEN  	 	 0x02
#define FG_CYAN   	 	 0x03
#define FG_RED    	 	 0x04
#define FG_VIOLET 	 	 0x05
#define FG_BROWN  	 	 0x06
#define FG_WHITE  	 	 0x07
#define FG_GREY   	 	 0x08
#define FG_LIGHT_BLUE   	 0x09
#define FG_LIGHT_GREEN  	 0x0A
#define FG_LIGHT_CYAN   	 0x0B
#define FG_LIGHT_RED    	 0x0C
#define FG_LIGHT_VIOLET 	 0x0D
#define FG_YELLOW  	 	 0x0E
#define FG_LIGHT_WHITE  	 0x0F
#define BG_BLACK		 0x00
#define BG_BLUE  		 0x10
#define BG_GREEN  	 	 0x20
#define BG_CYAN   	 	 0x30
#define BG_RED    	 	 0x40
#define BG_VIOLET 	 	 0x50
#define BG_BROWN  	 	 0x60
#define BG_WHITE  	 	 0x70
#define BG_GREY   	 	 0x80
#define BG_LIGHT_BLUE   	 0x90
#define BG_LIGHT_GREEN  	 0xA0
#define BG_LIGHT_CYAN   	 0xB0
#define BG_LIGHT_RED    	 0xC0
#define BG_LIGHT_VIOLET 	 0xD0
#define BG_YELLOW  	 	 0xE0
#define BG_LIGHT_WHITE  	 0xF0


void prints(char *str);
void printd(unsigned long n);
void printh(unsigned long n);
void clear(void);
unsigned char getch(void);
inline unsigned char inportb(unsigned int port);
inline void outportb(unsigned int port,unsigned char value);
unsigned char lastkey(void);
void getl(char *s);
unsigned long atoi(char *str); 

#endif
