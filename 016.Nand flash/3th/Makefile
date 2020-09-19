all : start.o clock.o sdram.o nand_flash.o interrupt.o exception.o main.o my_printf.o uart.o timer_pwm.o led_on.o string_utils.o lib1funcs.o 
	arm-linux-ld -T sdram.lds $^ -o main.elf
	arm-linux-objcopy -O binary -S main.elf main.bin
	arm-linux-objdump -D main.elf > main.dis
	
%.o : %.c
	arm-linux-gcc -Wall -Os -fno-builtin -nostdinc  -c -o  $@ $<

%.o : %.S
	arm-linux-gcc -Wall -Os -fno-builtin -nostdinc  -c -o $@ $<

clean:
	-rm *.o *.elf *.bin *.dis
