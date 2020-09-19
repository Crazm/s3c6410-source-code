#ifndef _GEOMETRY_H
#define _GEOMETRY_H

extern void draw_circle(int x, int y, int r, unsigned int color);
extern void draw_line(int x1,int y1,int x2,int y2,unsigned int color);
extern void fill_fb(int start_x, int end_x, int start_y, int end_y, unsigned int val);
extern void clear_display(unsigned int color);
extern void fb_disp_cross(unsigned int x,unsigned y,unsigned color);



#endif /* _GEOMETRY_H */
