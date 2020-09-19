#include "i2c_controller.h"
#include "../s3c6410.h"
#include "../interrupt.h"
#include "../led.h"

static p_i2c_msg p_cur_msg;

void i2c_interrupt_func(void)
{
    unsigned int i2cstat = IICSTAT_REG;
    p_cur_msg->cnt_transfered++;
    int index;
    
    /* 每传输完一个数据将产生一个中断 */

    /*对于每次传输，第一个中断是已经发出了设备地址 */
    if(p_cur_msg->flags == 0)  /* write */
    {
        /* 对于第一个中断，它是发送出设备地址后产生的
         * 需要判断是否有ACK
         * 有ACK ：设备存在
         * 无ACK ：无设备，出错，结束传输
         */
         if(p_cur_msg->cnt_transfered == 0)
         {
            if(i2cstat & 0x1)
            {   /* no ack */
                /* 停止传输 */
                IICSTAT_REG = 0xd0;
                IICCON_REG &= ~(1<<4);
                p_cur_msg->err = -1;
                delay(1000);
                return;
            }

            if(p_cur_msg ->cnt_transfered < p_cur_msg ->len)
            {
                /* 发送下一个数据 */
                IICDS_REG = p_cur_msg->buf[p_cur_msg->cnt_transfered];
                IICCON_REG &= ~(1<<4);
            }
            else
            {
               /* 停止传输 */
                IICSTAT_REG = 0xd0;
                IICCON_REG &= ~(1<<4);
                p_cur_msg->err = -1;
                delay(1000);
            }
         }
         

         /* 对应于其他中断，要继续发送下一个数据 */
    }
    else       /* read */
    {
        /* 对于第一个中断，它是发送出设备地址后产生的
         * 需要判断是否有ACK
         * 有ACK ：设备存在 恢复I2c 传输，这样在下一个中断才能得到数据
         * 无ACK ：无设备，出错，结束传输
         */

         if(p_cur_msg->cnt_transfered == 0)
         {
            if(i2cstat & 0x1)
            {   /* no ack */
                /* 停止传输 */
                IICSTAT_REG = 0x90;
                IICCON_REG &= ~(1<<4);
                p_cur_msg->err = -1;
                delay(1000);
                return;
            }
            else    /* 有ack */
            {
                /* 恢复I2c传输 */
                IICCON_REG &= ~(1<<4);
                return;
            }
          }
         

        /* 非第一个中断，表示得到一个新数据
         * 从IICDS读出，保存
         */
        if(p_cur_msg ->cnt_transfered < p_cur_msg ->len)
        {
        
             index = p_cur_msg ->cnt_transfered -1;
             p_cur_msg ->buf[index] = IICDS_REG;

            /* 恢复I2c传输 */
             IICCON_REG &= ~(1<<4);
        }
        else
        {
            IICSTAT_REG = 0x90;
            IICCON_REG &= ~(1<<4);
            delay(1000);
        }
    }
}


/* 实现i2c_controller
 * .init
 * .master_xfer
 * .name
 */
void s3c6410_i2c_con_init(void)
{
    /* IIC总线控制寄存器 */
    /*IICCON:
     *[7]   :IIC总线确认有效位:0：无效 1：有效 (rx_mode)
     *[6]   :发送时钟源选择:0: IICCLK = fPCLK /16 1: IICCLK = fPCLK /512  
     *[5]   :IIC总线发送/接收中断使能位:0：无效 1：有效
     *[4]   :IIC中断等待标志:0：（1）无中断等待（读时）；
     *                          （2）清除等待条件并且恢复操作（写时）
     *                       1：（1）等待中断（读时）；（2）N/A（写时）
     *[3:0] :IIC总线发送时钟预分频:IIC总线发送时钟频率由4位预分频值决定，以下
     *      面的格式：发送时钟 = IICCLK/(IICCON[3:0]+1)
     *      Txclk = 100khz
     */
     IICCON_REG = (1<<7) |(1<<6) |(1<<5) |(1<<0);

     /* 注册IIC中断 */
     register_irq(50,i2c_interrupt_func);   
}

int do_master_tx(p_i2c_msg msg)
{
    p_cur_msg = msg;
    msg ->cnt_transfered = -1;
    msg ->err = 0;
    
    /* 设置寄存器启动传输 */
        /* 1.配置为 master tx mode */
            IICSTAT_REG = (3<<6);
        /* 2.把从设备地址写入IICDS */
            IICADD_REG  = msg->addr<<1;
        /* 3.把0xF0写入到IICSTAT,数据即被发送出去，将导致中断产生 */
            IICSTAT_REG = 0xf0;
        
    /* 后续的传输由中断驱动 */

    /* 循环等待中断处理完毕 */
    while(!msg->err && msg ->cnt_transfered != msg->len)

    if(msg ->err)
        return -1;
    else 
        return 0;
}

int do_master_rx(p_i2c_msg msg)
{
    p_cur_msg = msg;
    msg ->cnt_transfered = -1;
    msg ->err = 0;
    
    /* 设置寄存器启动传输 */
        /* 1.配置为 master tx mode */
            //IICSTAT_REG = (3<<6);
        /* 2.把从设备地址写入IICDS */
            IICADD_REG  = msg->addr<<1;
        /* 3.把0xF0写入到IICSTAT,数据即被发送出去，将导致中断产生 */
            IICSTAT_REG = 0xb0;
        
    /* 后续的传输由中断驱动 */

    
    /* 循环等待中断处理完毕 */
    while(!msg->err && msg ->cnt_transfered != msg->len)

    if(msg ->err)
        return -1;
    else 
        return 0;
}

int s3c6410_i2c_master_xfer(p_i2c_msg msgs ,int num)
{
    int i;
    int err;
    for(i=0; i< num ; i++)
    {
        if(msgs->flags == 0) /* write */
            err = do_master_tx(msgs[i]);
        else
            err = do_master_rx(msgs[i]);
        if(err)
            return err;
    }

    return 0;
}

static i2c_controller s3c6410_i2c_con ={
    .name        = "s3c6410",
    .master_xfer = s3c6410_i2c_master_xfer,
    .init        = s3c6410_i2c_con_init,

};

void s3c6410_i2c_con_add(void);
{
    register_i2c_controller(&s3c6410_i2c_con);
}
