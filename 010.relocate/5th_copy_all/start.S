
.text
.global _start

_start:
	

	/* 设置协处理器 把外设的基地址和内存大小告诉CPU*/
	ldr r0, =0x70000000   //对于6410来说,内存(0x00000000～0x60000000),外设(0x70000000-0x7fffffff)
	orr r0, r0, #0x13	 //把r0的第4，1，0位置1即0x70000013，再送回r0； 外设大小:256M
	mcr p15,0,r0,c15,c2,4  //把r0的值(包括了外设基地址+外设大小)写入协处理器
	
	/* 关闭看门狗*/
	ldr r0, =0x7E004000
	mov r1, #0
	str r1, [r0]
	
	
	/* 判断是nand启动还是nor启动，同时设置内存：sp栈 */
	mov r1, #0
	ldr r0, [r1] //备份r0的值
	str r1, [r1]	//把0存到0地址空间
	ldr r2, [r1] //把0地址空间值读到r2寄存器里
	cmp r1, r2   //比较r1，r2寄存里的值
	ldr sp, =0x40000000+4096 //假设是nor启动，此时sp为soc2440的堆栈地址
	moveq sp, #4096 //比较值相等的话就是nand启动，sp=4086
	streq r0,[r1] //恢复原来0地址的值

	
	/* 时钟设置    */
	// 1. 设置各PLL的LOCK_TIME,使用默认值  
	ldr r0, =0x7E00F000  		// APLL_LOCK，供cpu使用 
	ldr r1, =0x0000FFFF
	str r1, [r0]
	
	str r1, [r0, #4]	 		// MPLL_LOCK(0x7E00F004)，供AHB(存储/中断/lcd等控制器)/APB(看门狗,定时器,SD等)总线上的设备使用 
	str r1, [r0, #8]	 		// EPLL_LOCK(0x7E00F008)，供UART,IIS,IIC使用 
	
	// 2. 设置为异步模式(Asynchronous mode)  
	ldr r0, =0x7E00F900						// OTHERS寄存器
											// 《linux installation for u-boot》3.7中：用MPLL作为HCLK和PCLK的Source是异步(ASYNC)模式
											// 用APLL是同步(SYNC)模式
	ldr r1, [r0]
	bic r1, r1, #0xc0						// 将r1的bit[6:7]清0,即SYNCMODE=0/SYNCMUXSEL=0
	str r1, [r0]
loop:			
	ldr r0, =0x7E00F900
	ldr r1, [r0]
	and r1, r1, #0xf00					
	cmp r1, #0
	bne loop		                       // 判断OTHERS寄存器[16:19]位是不是都为0，否则等待 ？？？
	
	// 3. 设置分频系数  
	#define ARM_RATIO    0   				// ARMCLK 	= DOUTAPLL / (ARM_RATIO + 1)  	= 532/(0+1) = 532  MHz
	#define MPLL_RATIO   0   				// DOUTMPLL = MOUTMPLL / (MPLL_RATIO + 1)   = 532/(0+1) = 532  MHz
	#define HCLKX2_RATIO 1   				// HCLKX2 	= HCLKX2IN / (HCLKX2_RATIO + 1) = 532/(1+1) = 266  MHz
	#define HCLK_RATIO   1   				// HCLK 	= HCLKX2   / (HCLK_RATIO + 1)   = 266/(1+1) = 133  MHz
	#define PCLK_RATIO   3   				// PCLK   	= HCLKX2   / (PCLK_RATIO + 1)   = 266/(3+1) = 66.5 MHz

	ldr r0, =0x7E00F020  					// CLK_DIV0
	ldr r1, =(ARM_RATIO) | (MPLL_RATIO << 4) | (HCLK_RATIO << 8) | (HCLKX2_RATIO << 9) | (PCLK_RATIO << 12)
	str r1, [r0]
	
	// 4. 设置PLL,放大时钟  
	// 4.1 配置APLL  
	#define APLL_CON_VAL  ((1<<31) | (266 << 16) | (3 << 8) | (1))
	ldr r0, =0x7E00F00C						// APLL_CON
	ldr r1, =APLL_CON_VAL					// FOUT = MDIV X FIN / (PDIV X 2SDIV) = 266*12/(3*2^1) = 532MHz
	str r1, [r0]		
	
	// 4.2 配置MPLL  
	#define MPLL_CON_VAL  ((1<<31) | (266 << 16) | (3 << 8) | (1))
	ldr r0, =0x7E00F010						// MPLL_CON
	ldr r1, =MPLL_CON_VAL					// FOUT = MDIV X FIN / (PDIV X 2SDIV) = 266*12/(3*2^1) = 532MHz
	str r1, [r0]		

	#define MPLL_SEL 1
	#define APLL_SEL 1	
	// 5.选择PLL的输出作为时钟源  
	ldr r0, =0x7E00F01C				       	// CLK_SRC
	ldr r1, =(MPLL_SEL<<1) | (APLL_SEL<<0)
	str r1, [r0]
	
	/* sdram初始化 */
	bl sdram_init

	/* 重定位text,rodata,data段整个程序 */
	mov r1, #0		
	ldr r2, =_start         /* 第一条指令运行时的地址 */
	ldr r3, =__bss_start    /* bss段起始地址 */
cpy:
	ldr r4,[r1]
	str r4,[r2]
	add r1,r1,#4 
	add r2,r2,#4
	cmp r2,r3
	ble cpy

	/* 清除bss段 */
	ldr r0, =__bss_start
	ldr r1, =_end
	mov r2, #0
clean:
	str r2, [r0]
	add r0, r0, #4
	cmp r0, r1
	ble clean
	
	/* 调用main函数 */
	//bl main /* 使用BL命令相对跳转。程序任然在sram中执行 */
	ldr pc, =main  /* 绝对跳转，调到SDRAM中运行 */

halt:
    b halt
		
.end
	
	
