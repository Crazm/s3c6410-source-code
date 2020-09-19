#ifndef _GEOMETRY_H
#define _GEOMETRY_H

extern void draw_circle(int x, int y, int r, unsigned char color);
extern void draw_line(int x1,int y1,int x2,int y2,unsigned char color);
extern void fill_fb(int start_x, int end_x, int start_y, int end_y, unsigned char val);
extern void clr_fb_b(void);
extern void clr_fb_w(void);



#endif /* _GEOMETRY_H */
