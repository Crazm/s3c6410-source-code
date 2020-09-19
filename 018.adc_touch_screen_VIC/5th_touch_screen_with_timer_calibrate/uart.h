
#ifndef _uart_h
#define _uart_h

void uart0_init(void);
extern void putc(unsigned char c);
extern unsigned char getc(void);
extern void puts(const char *s);
extern void printHex(unsigned int val);


#endif
