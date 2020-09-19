CC      = arm-linux-gcc
AR      = arm-linux-ar

INCLUDEDIR	:= ../include
CFLAGS		:= -Wall -Os -fno-builtin
CPPFLAGS	:= -nostdinc -I$(INCLUDEDIR)


objs := div64.o lib1funcs.o ctype.o muldi3.o printf.o string.o vsprintf.o

libc.a: $(objs)
	${AR} -r -o $@ $^
	
%.o:%.c
	${CC} $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

%.o:%.S
	${CC} $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

clean:
	rm -f libc.a *.o		
	
