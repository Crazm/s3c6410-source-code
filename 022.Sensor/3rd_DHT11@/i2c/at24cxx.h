#ifndef _AT24CXX_H
#define _AT24CXX_H

int at24cxx_write(unsigned int addr,unsigned char *data,int len);
int at24cxx_read(unsigned int addr,unsigned char *data,int len);

#endif /* _AT24CXX_H */
