all : start.o clock.o sdram.o  interrupt.o exception.o main.o  uart.o led_on.o 
	arm-linux-ld -T sdram.lds $^ -o main.elf
	arm-linux-objcopy -O binary -S main.elf main.bin
	arm-linux-objdump -D main.elf > main.dis
clean:
	rm *.o *.elf *.bin *.dis
	
%.o : %.c
	arm-linux-gcc -fno-builtin -c -o  $@ $<

%.o : %.S
	arm-linux-gcc -c -o $@ $<
