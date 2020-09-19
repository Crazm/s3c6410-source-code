#ifndef _DHT11_H
#define _DHT11_H

void dht11_test(void);
void dht11_init(void);
char dht11_read(char *hum,char *temp);

#endif /* _DHT11_H */
