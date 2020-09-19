all:
	arm-linux-gcc -c -o led_on_c.o led_on_c.c
	arm-linux-gcc -c -o start.o start.S
	arm-linux-ld -Ttext 0 start.o led_on_c.o -o led_on_c.elf
	arm-linux-objcopy -O binary -S led_on_c.elf led_on_c.bin
	arm-linux-objdump -D led_on_c.elf > led_on_c.dis
clean:
	rm *.o *.elf *.bin *.dis
