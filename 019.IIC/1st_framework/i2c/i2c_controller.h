#ifndef _I2C_CONTROLLER_H
#define _I2C_CONTROLLER_H

typedef struct i2c_msg {
        int cnt_transfered;
        int err;
        unsigned int addr; //7bits
        int flags;    /* 0:write 1:read */   
        int len;
        unsigned char *buf;
}i2c_msg,*p_i2c_msg;

typedef struct i2c_controller {
     int (*init)(void);
     int (*master_xfer)(p_i2c_msg msgs ,int num);
     char *name;
}i2c_controller,*p_i2c_controller;


void register_i2c_controller(p_i2c_controller *p);


#endif /* _I2C_CONTROLLER_H */
