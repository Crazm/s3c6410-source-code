all:
	arm-linux-gcc -c -o clock.o clock.c
	arm-linux-gcc -c -o start.o start.S
	arm-linux-ld -Ttext 0 start.o clock.o -o clock.elf
	arm-linux-objcopy -O binary -S clock.elf clock.bin
	arm-linux-objdump -D clock.elf > clock.dis
clean:
	rm *.o *.elf *.bin *.dis
