#!/bin/bash
fasm stage3.asm
gcc -c  io.c main.c -m32 
ld -o main -Ttext 0x0 -Tdata 0x2000 -e main -N  main.o io.o -m elf_i386
objcopy -R .note -R .comment -S -O binary main stage2.bin
cat stage1.bin  stage3.bin stage2.bin > boot.bin
sudo dd if=boot.bin of=/dev/sdb bs=512 count=20
exit

