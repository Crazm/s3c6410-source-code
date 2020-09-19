#include "one_wire.h"
#include "../s3c6410.h"

#define TINT_CSTAT_REG  __REG(0x7F006044)

#define rGPFCON GPFCON_REG
#define rGPFDAT GPFDAT_REG
#define rTCNTB3 TCNTB3_REG
#define rTINT_CSTAT TINT_CSTAT_REG
#define rTCON TCON_REG
#define TINT_CSTAT_INTMASK(n) ( (n&0x1FU))
#define TIMER3_PENDING_CLEAR (1U<<8)
#define TIMER3_INTERRUPT_ENABLE (1<<3)
#define SYS_TIMER_PRESCALER 2
#define SYS_TIMER_DIVIDER 1
#define S3C6410_PCLK    66500000

#define SAMPLE_BPS 9600

#define REQ_INFO          0x60U

static void set_pin_as_input(void)
{
	unsigned tmp;
	tmp = rGPFCON;
	tmp &= ~(3U << 30);
	rGPFCON = tmp;
}

static void set_pin_as_output(void)
{
	unsigned tmp;
	tmp = rGPFCON;
	tmp = (tmp & ~(3U << 30)) | (1U << 30);
	rGPFCON = tmp;
}

static void set_pin_value(int v)
{
	unsigned tmp;
	tmp = rGPFDAT;
	if (v) {
		tmp |= (1 << 15);
	} else {
		tmp &= ~(1 << 15);
	}
	rGPFDAT = tmp;
}

static int get_pin_value(void)
{
	int v;
	unsigned long tmp;
	tmp = rGPFDAT;
	v = !!(tmp & (1 << 15));
	return v;
}

static void InitTimer(void)
{
	rTCNTB3 = (S3C6410_PCLK / SYS_TIMER_PRESCALER / SYS_TIMER_DIVIDER / SAMPLE_BPS - 1);
	rTINT_CSTAT = TINT_CSTAT_INTMASK(rTINT_CSTAT) | TIMER3_PENDING_CLEAR;
	rTINT_CSTAT = TINT_CSTAT_INTMASK(rTINT_CSTAT) | TIMER3_INTERRUPT_ENABLE;
}

static void StartTimer(void)
{
	rTCON &= ~(0xf << 16);	// Timer3 Stop
	rTCON |= (1 << 17);	// update TCNTB3
	rTCON &= ~(1 << 17);
	rTCON |= ((1 << 19) | (1 << 16));	// AutoReload mode, Timer3 Start
}

static void StopTimer(void)
{
	unsigned tcon;
	tcon = rTCON;
	tcon &= ~(1 << 16);
	rTCON = tcon;
}

static void WaitTimerTick(void)
{
	while ((rTINT_CSTAT & (1U << 8)) == 0) ;
	rTINT_CSTAT = TINT_CSTAT_INTMASK(rTINT_CSTAT) | TIMER3_PENDING_CLEAR;
}

static unsigned char crc8(unsigned v, unsigned len);

static int OneWireSession(unsigned char req, unsigned char res[])
{
	unsigned Req;
	unsigned *Res;
	unsigned int i;

	Req = (req << 24) | (crc8(req << 24, 8) << 16);
	Res = (unsigned *)res;

	set_pin_value(1);
	set_pin_as_output();
	StartTimer();
	for (i = 0; i < 60; i++) {
		WaitTimerTick();
	}
	set_pin_value(0);
	for (i = 0; i < 2; i++) {
		WaitTimerTick();
	}
	for (i = 0; i < 16; i++) {
		int v = !!(Req & (1U << 31));
		Req <<= 1;
		set_pin_value(v);
		WaitTimerTick();
	}
	WaitTimerTick();
	set_pin_as_input();
	WaitTimerTick();
	for (i = 0; i < 32; i++) {
		(*Res) <<= 1;
		(*Res) |= get_pin_value();
		WaitTimerTick();
	}
	StopTimer();
	set_pin_value(1);
	set_pin_as_output();

	return crc8(*Res, 24) == res[0];
}

static int TryOneWireSession(unsigned char req, unsigned char res[])
{
	int i;
	for (i = 0; i < 3; i++) {
		if (OneWireSession(req, res)) {
			return 1;
		}
	}
	return 0;
}

void InitOneWire(void)
{
	InitTimer();
}

int GetInfo(unsigned char *Lcd, unsigned short *FirmwareVer)
{
	unsigned char res[4];

	if (!TryOneWireSession(REQ_INFO, res)) {
		return 0;
	}
	*Lcd = res[3];
	*FirmwareVer = res[2] * 100 + res[1];
	return 1;
}

int SetBacklightOfLCD(unsigned Brightness)
{
	unsigned char res[4];
	int ret;
	if (Brightness > 127) {
		Brightness = 127;
	}
	ret = TryOneWireSession(Brightness | 0x80U, res);
	return ret;
}

static unsigned char crc8(unsigned v, unsigned len)
{
	unsigned char crc = 0xACU;
	while (len--) {
		if ((crc & 0x80U) != 0) {
			crc <<= 1;
			crc ^= 0x7U;
		} else {
			crc <<= 1;
		}
		if ((v & (1U << 31)) != 0) {
			crc ^= 0x7U;
		}
		v <<= 1;
	}
	return crc;
}

