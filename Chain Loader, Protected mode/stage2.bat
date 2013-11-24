gcc -c -pedantic-errors kernel.c -o kernel.o -Ttext 0x10000 -Tdata 0x10000 -nostdlib -m32 
objcopy -O binary -j .text kernel.o stage2.bin
pause