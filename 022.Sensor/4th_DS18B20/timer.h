
#ifndef _timer_h
#define _timer_h

typedef void (*timer_func)(void);

typedef struct timer_desc{
        char * name;
        timer_func fp;
}timer_desc,*p_timer_desc;


void timer0_init(void);
void timer0_pwm(unsigned int frq, unsigned duty_cycle);
int register_timer(char *name, timer_func fp);
int unregister_timer(char *name);

void udelay(int us);
void mdelay(int ms);
void delay_test(void);





#endif /* _timer_h */

