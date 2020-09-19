
objs =start.o clock.o sdram.o nand.o interrupt.o exception.o main.o  uart.o timer.o led.o 

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
objs += adc_touchscreen/tslib.o

objs += i2c/at24cxx.o
objs += i2c/i2c_controller.o
objs += i2c/i2c_test.o
objs += i2c/s3c6410_i2c_controller.o
objs += i2c/oled.o

objs += lib/libc.a

all :  $(objs)
	arm-linux-ld -T sdram.lds $^ libgcc.a  -o main.elf
	arm-linux-objcopy -O binary -S main.elf main.bin
	arm-linux-objdump -D main.elf > main.dis
	
.PHONY : lib/libc.a
lib/libc.a:
	cd lib; make; cd ..
	
%.o : %.c
	arm-linux-gcc -Wall -Os -fno-builtin -nostdinc  -c -o  $@ $<

%.o : %.S
	arm-linux-gcc -Wall -Os -fno-builtin -nostdinc  -c -o $@ $<

clean:
	 make  clean -C lib
	 rm -f *.o *.elf *.bin *.dis
