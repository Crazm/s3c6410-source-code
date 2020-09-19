#include "i2c_controller.h"
#include "../s3c6410.h"
#include "../interrupt.h"
#include "../include/stdio.h"



static p_i2c_msg p_cur_msg;

void i2c_interrupt_func(void)
{
    unsigned int i2cstat = IICSTAT_REG;
    p_cur_msg->cnt_transfered++;
    int index;
    //printf("i2c_interrupt_func,flag = %d\n\r",p_cur_msg->flags);
    //printf("IICSTAT_REG:%2x,IICADD_REG:%2x,cnt_tr:%d\n\r",IICSTAT_REG,IICADD_REG,p_cur_msg->cnt_transfered);
    
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
                printf("write no ack\n\r");
                return;
            }
         }

         /* 对应于其他中断，要继续发送下一个数据 */
         if(p_cur_msg ->cnt_transfered < p_cur_msg ->len)
         {
             /* 发送下一个数据 */
             IICDS_REG  = p_cur_msg->buf[p_cur_msg->cnt_transfered];
             IICCON_REG &= ~(1<<4);
         }
         else
         {
            /* 停止传输 */
             IICSTAT_REG = 0xd0;
             IICCON_REG &= ~(1<<4);
             delay(1000);
             return;
         }    
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
                printf("read no ack\n\r");
                delay(1000);
                return;
            }
            else    /* 有ack ,读过程这个部分不能少 */
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
            if(p_cur_msg ->cnt_transfered == (p_cur_msg ->len - 1) )
               IICCON_REG &= ~(1<<7);     //EEPROM最后一位不发回应
            else
               IICCON_REG |= (1<<7);      //EEPROM最后一位发回应
            
            index = p_cur_msg ->cnt_transfered -1;
            p_cur_msg ->buf[index] = IICDS_REG & 0xff;

            /* 恢复I2c传输 */
             IICCON_REG &= ~(1<<4);
        }
        else
        {
          /* 发出停止信号 */
            IICSTAT_REG = 0x90;
            IICCON_REG &= ~(1<<4);
            delay(1000);
            return;
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

    /* 配置i2c引脚 */
     GPBCON_REG = (2<<20)|(2<<24);
     GPBPUD_REG = (2<<10)|(2<<12);
    //printf("GPBCON_REG:%08x,GPBPUD_REG:%08x\n\r",GPBCON_REG,GPBPUD_REG);
     
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
     IICCON_REG = (0<<7) |(1<<6) |(1<<5) |(9<<0);  //txclk = 372khz

     /* 注册IIC中断 */
     register_irq(50,i2c_interrupt_func);  

     
}

int do_master_tx(p_i2c_msg msg)
{
    msg ->cnt_transfered = -1;
    msg ->err = 0;
    p_cur_msg = msg;

    /* Wait until IIC bus is free or Arbitration successful */
    while( IICSTAT_REG & (1<<5) );

       IICCON_REG |= (1<<7); /* 在ACK周期，释放SDA */

    /* 使能IIC总线数据输出有效 */
        IICSTAT_REG |= (1<<4);

    /* 把从设备地址写入IICDS */
       IICDS_REG  = (msg->addr<<1);
       //printf("IICDS:%2x\n\r",IICDS_REG);
            
    /*  把0xF0写入到IICSTAT,数据即被发送出去，将导致中断产生 */
       IICSTAT_REG = 0xf0;
       
    /* 后续的传输由中断驱动 */

    /* 循环等待中断处理完毕 */
        while(!p_cur_msg->err && p_cur_msg ->cnt_transfered != p_cur_msg->len);

        if(p_cur_msg ->err)
            return -1;
         
        return 0;
}

int do_master_rx(p_i2c_msg msg)
{
    msg ->cnt_transfered = -1;
    msg ->err = 0;
    p_cur_msg = msg;

    /* Wait until IIC bus is free or Arbitration successful */
        while( IICSTAT_REG & (1<<5) );

        IICCON_REG |= (1<<7); /* 在ACK周期，释放SDA */
    
    /* 1.使能IIC总线数据输出有效 */
        IICSTAT_REG |= (1<<4);

    /* 2.把从设备地址写入IICDS */
       IICDS_REG  = (msg->addr<<1)|(1<<0);
       //printf("IICDS:%2x\n\r",IICDS_REG);
        
    /* 3.把0xF0写入到IICSTAT,数据即被发送出去，将导致中断产生 */
        IICSTAT_REG = 0xb0;
    
    /* 后续的传输由中断驱动 */

    
    /* 循环等待中断处理完毕 */
    while(!p_cur_msg->err && p_cur_msg ->cnt_transfered != p_cur_msg->len);
    if(p_cur_msg ->err)
        return -1;

    return 0;
}

int s3c6410_i2c_master_xfer(p_i2c_msg msgs ,int num)
{
    int i;
    int err;
    for(i=0; i< num ; i++)
    {
        if(msgs[i].flags == 0) /* write */
            err = do_master_tx(&msgs[i]);
        else
            err = do_master_rx(&msgs[i]);
        if(err)
            return err;
    }
    return 0;
}

struct i2c_controller s3c6410_i2c_con ={

    .name        = "s3c6410",
    .master_xfer = s3c6410_i2c_master_xfer,
    .init        = s3c6410_i2c_con_init,

};

void s3c6410_i2c_con_add(void)
{
    register_i2c_controller(&s3c6410_i2c_con);
}
