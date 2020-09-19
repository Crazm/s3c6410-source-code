all : start.o clock.o sdram.o  interrupt.o exception.o main.o  uart.o timer_pwm.o led_on.o lib/libc.a
	arm-linux-ld -T sdram.lds $^ -o main.elf
	arm-linux-objcopy -O binary -S main.elf main.bin
	arm-linux-objdump -D main.elf > main.dis


.PHONY:	lib/libc.a
lib/libc.a:
	cd lib; make; cd ..

	
%.o : %.c
	arm-linux-gcc -fno-builtin -c -o  $@ $<

%.o : %.S
	arm-linux-gcc -c -o $@ $<

clean:
	make  clean -C lib
	-rm *.o *.elf *.bin *.dis
