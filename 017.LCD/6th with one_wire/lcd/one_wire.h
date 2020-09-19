#ifndef _ONE_WIRE_H
#define _ONE_WIRE_H

extern void InitOneWire(void);
extern int GetInfo(unsigned char *Lcd, unsigned short *FirmwareVer);
extern int SetBacklightOfLCD(unsigned Brightness);



#endif /* _ONE_WIRE_H */
