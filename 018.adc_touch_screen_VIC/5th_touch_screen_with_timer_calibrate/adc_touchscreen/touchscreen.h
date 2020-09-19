
#ifndef _TOUCHSCREEN_H
#define _TOUCHSCREEN_H

void touchscreen_init(void);
void ts_read_raw(int *px, int *py, int *ppressure);


#endif /* _TOUCHSCREEN_H */

