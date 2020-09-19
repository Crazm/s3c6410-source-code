objs := div64.o lib1funcs.o ctype.o delay.o muldi3.o printf.o string.o  vsprintf.o

libc.a: $(objs)
	arm-linux-ar -r -o $@ $^
	
%.o:%.c
	arm-linux-gcc -nostdinc -I../include -Wall -Os -fno-builtin -c -o $@ $<

%.o:%.S
	arm-linux-gcc -nostdinc -I../include -Wall -Os -fno-builtin -c -o $@ $<

clean:
	rm -f libc.a *.o		
	
