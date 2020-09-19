#include "sdram_init.h"
#define P1MEMCCMD		(*((volatile unsigned int *)0x7e001004))
#define P1REFRESH		(*((volatile unsigned int *)0x7e001010))
#define P1CASLAT		(*((volatile unsigned int *)0x7e001014))
#define MEM_SYS_CFG		(*((volatile unsigned int *)0x7e00f120))
#define P1MEMCFG		(*((volatile unsigned int *)0x7e00100c))
#define P1T_DQSS		(*((volatile unsigned int *)0x7e001018))
#define P1T_MRD			(*((volatile unsigned int *)0x7e00101c))
#define P1T_RAS			(*((volatile unsigned int *)0x7e001020))
#define P1T_RC			(*((volatile unsigned int *)0x7e001024))
#define P1T_RCD			(*((volatile unsigned int *)0x7e001028))
#define P1T_RFC			(*((volatile unsigned int *)0x7e00102c))
#define P1T_RP			(*((volatile unsigned int *)0x7e001030))
#define P1T_RRD			(*((volatile unsigned int *)0x7e001034))
#define P1T_WR			(*((volatile unsigned int *)0x7e001038))
#define P1T_WTR			(*((volatile unsigned int *)0x7e00103c))
#define P1T_XP			(*((volatile unsigned int *)0x7e001040))
#define P1T_XSR			(*((volatile unsigned int *)0x7e001044))
#define P1T_ESR			(*((volatile unsigned int *)0x7e001048))
#define P1MEMCFG2		(*((volatile unsigned int *)0X7e00104c))
#define P1_chip_0_cfg	(*((volatile unsigned int *)0x7e001200))
#define P1MEMSTAT		(*((volatile unsigned int *)0x7e001000))
#define P1MEMCCMD		(*((volatile unsigned int *)0x7e001004))
#define P1DIRECTCMD		(*((volatile unsigned int *)0x7e001008))

	
#define HCLK			133000000
#define nstoclk(ns)		( ns/(1000000000/HCLK)+1 )			//+1是四舍五入

/* sdram初始化 */
void sdram_init(void)
{
	/*1.以‘3’b100’执行 memc_cmd，使得 DRAM 控制器输入‘配置’状态。*/
	P1MEMCCMD  = 0x4;  //使DRAM控制器处于“配置”状态

	/* 写存储器时间参数，芯片配置和 ID 配置寄存器*/

	/*2.1设置刷新周期 由数据手册可得刷新周期为7.8us，
	所以需要7.8*10^-6 /(1/133MHZ)=1037+1=1038个时间周期*/
	P1REFRESH = nstoclk(7800); //刷新周期:(7.8us)/((1/HCLK)s)=(7.8*10^3)/(1/133*10^6)=1037.4


	/* 2.2 设置时间参数，下列设置全都是取了最小值下列设置均在sdram手册中可查询到 */
	P1CASLAT |= (3<<1); //CAS Latency:指的是内存存取数据所需的延迟时间，简单的说，就是内存接到CPU的指令后的反应速度。一般的参数值是2和3两种。K4X1G163PQ的芯片手册上CAS Latency=3 
	P1T_DQSS = 0x1 ;         				
	P1T_MRD = 0x2 ;							
	P1T_RAS =  nstoclk(42) ;					
	P1T_RC = nstoclk(60) ;									
	P1T_RCD  = nstoclk( 18 ) | (( nstoclk( 18 ) - 3 ) << 3 ) ;
	P1T_RFC = nstoclk( 72 ) | ( ( nstoclk( 72 )-3 ) << 5 ) ;   
	P1T_RP = nstoclk( 18 ) | ( ( nstoclk( 18 ) - 3 ) << 3 ) ; 
	P1T_RRD = nstoclk(12) ;
	P1T_WR = nstoclk(12) ;
    P1T_WTR = 0x1 ;
	P1T_XP =  0x1 ;
	P1T_XSR = nstoclk(120) ;
	P1T_ESR = nstoclk(120) ;

	/* 2.3 chip configuration */
	P1MEMCFG |= 2<<0 ;  // column address AXI地址位的编码数包含的列地址A0~A9
	P1MEMCFG |= 3<<3 ;  // row address AXI地址位的编码数包含的行地址A0~A13
	P1MEMCFG |= 0<<6 ;	// A10/AP 自动预充电位的编码位置在内存中的地址：0=地址位 10
	P1MEMCFG &=~(7<<15);//BurstLength[15:17]清零
	P1MEMCFG |= 2<<15; 	// BurstLength：脉冲4，执行数据访问的数量编码到 SDRAM，用于每个读和写命令	  					  					
					
						//set_nbit( P1MEMCFG2, 0, 4, 0x5 ) ???
						
	P1MEMCFG2 |= 1<<6 ;		   // 32 bit 
	P1MEMCFG2 &=~(7<<8);
	P1MEMCFG2 |= 3<<8 ;		// Mobile DDR SDRAM 	
	P1MEMCFG2 &=~(3<<11);
	P1MEMCFG2 |= (1<<11);	//读延迟 1 周期
	P1_chip_0_cfg |= 1<<16 ;	// Bank-Row-Column organization 

	/* 3. 初始化sdram */
	P1DIRECTCMD = 0xc0000 ; 	//使得 DRAM 控制器产生‘NOP’存储器命令
	P1DIRECTCMD = 0x0 ;			//使得 DRAM 控制器产生‘Prechargeall’存储器命令
	P1DIRECTCMD = 0x40000 ;		//使得 DRAM 控制器产生‘Autorefresh’存储器命令
	P1DIRECTCMD = 0x40000 ;		//使得 DRAM 控制器产生‘Autorefresh’存储器命令
	P1DIRECTCMD = 0xa0000 ; 	//使得 DRAM 控制器产生‘MRS’存储器命令.MRS 块地址必须被设置。
	P1DIRECTCMD = 0x80032 ; 	//使得 DRAM 控制器产生‘MRS’存储器命令.EMRS 块地址必须被设置。

	MEM_SYS_CFG = 0x0 ;			//内存子系统配置寄存器
					
	/* 4. 使sdramc进入"ready"状态	*/
	P1MEMCCMD = 0x000 ;			//DRAM控制器状态改变为运行
	while( !((  P1MEMSTAT & 0x3 ) == 0x1)); 	// 等待dramc进入"ready"状态	
	
	
}

/* sdram_test */
int sdram_test(void)
{
	volatile unsigned char *p=(volatile unsigned char *)0x50000000;
	int i;
	for(i=0;i<10000;i++)	p[i]=0x55;
	for(i=0;i<10000;i++)	
		{
			if(p[i] != 0x55) return -1;
		}
	return 0;
}

/* 把nand拷贝到sdram */
void copy2sdram(void)
{
	/*要从lds文件中获取__code_start,__bss_start
	 *然后从0地址把数据复制到__code_start
	 */
	extern int __code_start,__bss_start;
	volatile unsigned int *dest = (volatile unsigned int *)&__code_start;
	volatile unsigned int *end = (volatile unsigned int *)&__bss_start;
	volatile unsigned int *src = (volatile unsigned int *)0;
	while(dest < end )
		{
			*dest++ = *src++;
		}
}

/* 把清除bss段 */
void clean_bss(void)
{
	/*要从lds文件中获取__bss_start,_end
	 */
	extern int __bss_start,_end;
	volatile unsigned int *start = (volatile unsigned int *)&__bss_start;
	volatile unsigned int *end = (volatile unsigned int *)&_end;
	while(start <= end )
		{
			*start++ = 0;
		}
}



