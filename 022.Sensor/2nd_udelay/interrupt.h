#ifndef _interrupt_h_
#define _interrupt_h_

typedef void (*irq_func)(void);

void irq_init(void);

void register_irq(int irq, irq_func fp);

#endif /* _interrupt_h_ */

