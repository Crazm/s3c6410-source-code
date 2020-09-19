
objs =start.o clock.o sdram.o nand_flash.o interrupt.o exception.o main.o my_printf.o uart.o timer_pwm.o led_on.o string_utils.o lib1funcs.o

objs += lcd/font.o
objs += lcd/framebuffer.o
objs += lcd/geometry.o
objs += lcd/lcd.o
objs += lcd/lcd_4.3.o
objs += lcd/lcd_controller.o
objs += lcd/lcd_test.o
objs += lcd/s3c6410_lcd_controller.o
objs += lcd/font_8x16.o

objs += adc_touchscreen/adc.o
objs += adc_touchscreen/adc_test.o
objs += adc_touchscreen/touchscreen.o
objs += adc_touchscreen/touchscreen_test.o


all :  $(objs)
	arm-linux-ld -T sdram.lds $^ libgcc.a  -o main.elf
	arm-linux-objcopy -O binary -S main.elf main.bin
	arm-linux-objdump -D main.elf > main.dis
	
%.o : %.c
	arm-linux-gcc -Wall -Os -fno-builtin -nostdinc  -c -o  $@ $<

%.o : %.S
	arm-linux-gcc -Wall -Os -fno-builtin -nostdinc  -c -o $@ $<

clean:
	-rm *.o *.elf *.bin *.dis
