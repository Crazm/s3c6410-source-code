#include "../s3c6410.h"

/* 用GPIO模拟SPI */
static void SPI_GPIO_Init(void)
{
    /* GPC4 MISO输入，GPC5 CLK输出，
     * GPC6 MOSI输出，GPC7 CS 输出 
     */
    GPCCON_REG =(1<<28)|(1<<24)|(1<<20)|(0<<16);
    GPCPUD_REG =0xaa00;  //使能上拉
}

static void Spi_Set_CLK(char val)
{
    if(val)
        GPCDAT_REG |= (1<<5);
    else
        GPCDAT_REG &= ~(1<<5);
}

static void Spi_Set_MOSI(char val)
{
    if(val)
        GPCDAT_REG |= (1<<6);
    else
        GPCDAT_REG &= ~(1<<6);
}

static char Spi_Get_MISO(void)
{
    if(GPCDAT_REG & 0x10)
        return 1;
    else
        return 0;
}

void SPISendByte(unsigned char val)
{   
    int i;
    for(i=0 ; i < 8; i++)
    {
        Spi_Set_CLK(0);
        Spi_Set_MOSI(val &0x80);
        Spi_Set_CLK(1);
        val <<= 1;
    } 
}

unsigned char SPIReceByte(void)
{   
    int i;
    unsigned char val = 0;
    for(i = 0; i < 8; i++)
    {
        val <<= 1;
        Spi_Set_CLK(0);
        if(Spi_Get_MISO())
            val |= 1;
        Spi_Set_CLK(1);
    }
    return val;
}


void SPIInit(void)
{
   SPI_GPIO_Init();    //GPIO初始化 
}



 
