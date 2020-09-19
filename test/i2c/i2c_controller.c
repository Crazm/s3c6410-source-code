#include "i2c_controller.h"
#include "s3c6410_i2c_controller.h"

#define I2C_CONTROLLER_NUM 10
static p_i2c_controller p_i2c_controllers[I2C_CONTROLLER_NUM];
static p_i2c_controller p_i2c_con_selected;


/* 用一个i2c_controller数组用来存放各种不同芯片的操作结构体 */
void register_i2c_controller(p_i2c_controller p)
{
    int i;
    for(i=0; i<I2C_CONTROLLER_NUM; i++)
    {
        if(!p_i2c_controllers[i])
        {
            p_i2c_controllers[i] = p;
            return;
        }
    }
}

extern int strcmp(const char * cs,const char * ct);


/* 根据名字来选择某款I2C控制器 */
int select_i2c_controller(char *name)
{
    int i;
    for(i=0; i<I2C_CONTROLLER_NUM; i++)
    {
        if((p_i2c_controllers[i]->name) && (!strcmp(p_i2c_controllers[i]->name,name)))
        {
            p_i2c_con_selected = p_i2c_controllers[i];
            return 0;
        }
    }
    return -1;
}

/* 实现i2c_transfer接口函数 */
int i2c_transfer(p_i2c_msg msgs, int num)
{
    return p_i2c_con_selected->master_xfer(msgs,num);
}

void i2c_init(void)
{
    /* 注册下面的I2c控制器 */
    s3c6410_i2c_con_add();
    
    /* 选择某款I2c控制器 */
    select_i2c_controller("s3c6410");

    /* 调用它的初始化函数 */
    p_i2c_con_selected->init();
    
    
}

