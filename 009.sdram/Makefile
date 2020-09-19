all:
	arm-linux-gcc -fno-builtin -c -o  main.o main.c
	arm-linux-gcc -fno-builtin -c -o  uart.o uart.c
	arm-linux-gcc -fno-builtin -c -o  led_on.o led_on.c
	arm-linux-gcc -c -o start.o start.S
	arm-linux-gcc -c -o sdram.o sdram.c
	arm-linux-ld -Ttext 0 start.o main.o uart.o sdram.o led_on.o -o main.elf
	arm-linux-objcopy -O binary -S main.elf main.bin
	arm-linux-objdump -D main.elf > main.dis
clean:
	rm *.o *.elf *.bin *.dis
