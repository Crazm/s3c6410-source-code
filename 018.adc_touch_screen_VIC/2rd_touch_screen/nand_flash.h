#ifndef _nand_flash_H
#define _nand_flash_H

extern void nand_init(void);
extern void nand_flash_test(void);
extern void nand_read(unsigned int addr, unsigned char *buf, unsigned int len);


#endif /* _nand_flash_H */

