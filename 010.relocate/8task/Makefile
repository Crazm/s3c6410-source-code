all:
	arm-linux-gcc -fno-builtin -c -o  main.o main.c
	arm-linux-gcc -fno-builtin -c -o  uart.o uart.c
	arm-linux-gcc -fno-builtin -c -o  my_printf.o my_printf.c
	arm-linux-gcc -c -o sdram_init.o sdram_init.c
	arm-linux-gcc -c -o start.o start.S
	arm-linux-gcc -c -o lib1funcs.o lib1funcs.S
	arm-linux-ld -T sdram.lds start.o sdram_init.o uart.o  main.o lib1funcs.o my_printf.o -o main.elf
	arm-linux-objcopy -O binary -S main.elf main.bin
	arm-linux-objdump -D main.elf > main.dis
clean:
	rm *.o *.elf *.bin *.dis
