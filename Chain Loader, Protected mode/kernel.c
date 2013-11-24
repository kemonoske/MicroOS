__asm
	(
		"MOV $0x10,%EAX\n"
		"MOV %EAX,%DS\n"
		"MOV %EAX,%ES\n"
		"MOV %EAX,%GS\n"
		"MOV %EAX,%FS\n"
		"MOV %EAX,%SS\n"
	);

void k_main()
{
char *vid = ( char * ) 0xb8000;
static char *msg = "Hello\0";
int i = 0, j = 0;
vid[i++] = msg[j++];
vid[i++] = 0x07;
vid[i++] = msg[j++];
vid[i++] = 0x07;
vid[i++] = msg[j++];
vid[i++] = 0x07;
vid[i++] = msg[j++];
vid[i++] = 0x07;
vid[i++] = msg[j++];
vid[i++] = 0x07;


while ( 1 )
{
}
}