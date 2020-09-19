#ifndef _CIRCLE_BUFFER_H
#define _CIRCLE_BUFFER_H

char ir_event_put(p_irda_raw_event pd);
char ir_event_get(p_irda_raw_event pd);
int ir_event_get_timeout(p_irda_raw_event pd);


#endif /* _CIRCLE_BUFFER_H */
