all:
	arm-linux-gcc -c -o led_on.o led_on.S
	arm-linux-ld -Ttext 0 led_on.o -o led_on.elf
	arm-linux-objcopy -O binary -S led_on.elf led_on.bin
clean:
	rm *.o *.elf *.bin
