#include "irda_raw.h"

#define NEXT_PLACE(i)  ((i+1)&0x3FF)

static irda_raw_event g_event[1024];
static int g_r = 0;
static int g_w = 0;

static char is_ir_event_buf_empty(void)
{
    return (g_r == g_w);
}

static char is_ir_event_buf_full(void)
{
    return (NEXT_PLACE(g_w) == g_r);
}

char ir_event_put(p_irda_raw_event pd)
{
    if(is_ir_event_buf_full())
        return -1;
    g_event[g_w] = *pd;
    g_w = NEXT_PLACE(g_w);
    return 0;
}

char ir_event_get(p_irda_raw_event pd)
{
    if(is_ir_event_buf_empty())
        return -1;
    *pd = g_event[g_r];
    g_r = NEXT_PLACE(g_r);
    return 0;
}
