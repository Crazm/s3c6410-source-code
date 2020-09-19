#include "vsprintf.h"
#include "string.h"
#include "printf.h"
#include "ctype.h"


extern void putc(unsigned char c);
extern unsigned char getc(void);


#define	OUTBUFSIZE	1024
#define	INBUFSIZE	1024


static char g_pcOutBuf[OUTBUFSIZE];
static char g_pcInBuf[INBUFSIZE];


int printf(const char *fmt, ...)
{
	int i;
	int len;
	va_list args;

	va_start(args, fmt);
	len = vsprintf(g_pcOutBuf,fmt,args);
	va_end(args);
	for (i = 0; i < strlen(g_pcOutBuf); i++)
	{
		putc(g_pcOutBuf[i]);
	}
	return len;
}



int scanf(const char * fmt, ...)
{
	int i = 0;
	unsigned char c;
	va_list args;
	
	while(1)
	{
		c = getc();
		putc(c);
		if((c == 0x0d) || (c == 0x0a))
		{
			g_pcInBuf[i] = '\0';
			break;
		}
		else
		{
			g_pcInBuf[i++] = c;
		}
	}
	
	va_start(args,fmt);
	i = vsscanf(g_pcInBuf,fmt,args);
	va_end(args);

	return i;
}


char *gets(char s[])
{ 
	int i = 0;
	char c;
	
	while(1)
	{
		c = getc();

		/* 回显 */
		putc(c);
		if (c == '\n')
			putc('\r');
		else if (c == '\r')
			putc('\n');
		
		if((c == '\n') || (c == '\r'))
		{
			s[i] = '\0';
			break;
		}
		else
		{
			s[i++] = c;
		}
	}
	
	return s;
}

int get_int(void)
{
	char str[100];
	gets(str);
	return simple_strtol(str, NULL, 0);
}

unsigned int get_uint(void)
{
	char str[100];
	gets(str);
	return simple_strtoul(str, NULL, 0);
}



