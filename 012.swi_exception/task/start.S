
.text
.global _start

_start:
	// 异常向量表
	b reset  			/* vector 0: 复位时,cpu跳到0地址  */
	ldr pc, und_addr	/* vector 4: und未定义指令异常 */
	ldr pc, swi_addr	/* vector 8: swi异常，进入svc模式 */
	b halt				/* vector c: 预取中止异常 */
	b halt     			/* vector 10: 数据访问异常 */
	b halt  			/* vector 14: 没用到 */
	b halt			   /* vector 18: irq中断异常 */
	b halt     		 	/* vector 1c: fiq快中断异常 */
	
und_addr:
	.word do_und

swi_addr:
	.word do_swi




.align 4	
reset:
	/* 系统启动默认进入svc模式 */
	/* 设置协处理器 把外设的基地址和内存大小告诉CPU*/
	ldr r0, =0x70000000   //对于6410来说,内存(0x00000000～0x60000000),外设(0x70000000-0x7fffffff)
	orr r0, r0, #0x13	 //把r0的第4，1，0位置1即0x70000013，再送回r0； 外设大小:256M
	mcr p15,0,r0,c15,c2,4  //把r0的值(包括了外设基地址+外设大小)写入协处理器
	
	/* 关闭看门狗*/
	ldr r0, =0x7E004000
	mov r1, #0
	str r1, [r0]

	/* 设置中断工作在VIC模式下 */
	mrc p15,0,r0,c1,c0,0
	orr r0,r0,#(1<<24)
	mcr p15,0,r0,c1,c0,0
	
	/* 判断是nand启动还是nor启动，同时设置内存：sp栈 */
	mov r1, #0
	ldr r0, [r1] //备份r0的值
	str r1, [r1]	//把0存到0地址空间
	ldr r2, [r1] //把0地址空间值读到r2寄存器里
	cmp r1, r2   //比较r1，r2寄存里的值
	ldr sp, =0x40000000+4096 //假设是nor启动，此时sp为soc2440的堆栈地址
	moveq sp, #4096 //比较值相等的话就是nand启动，sp=4086
	streq r0,[r1] //恢复原来0地址的值

	/* 时钟初始化 */
	bl clock_init
	
	/* sdram初始化 */
	bl sdram_init

	/* 重定位text,rodata,data段整个程序 */
	bl copy2sdram

	/* 清除bss段 */
	bl clean_bss

	/* 跳转到sdram中运行 */
	ldr pc,=sdram
sdram:

	/* 初始化串口0 */
	bl uart0_init

	/* 进入usr模式 */
	mrs r0, cpsr			//把cpsr寄存器的值读入r0
	bic r0,r0,#0x1f			//把r0的[0:4]位清零
	bic r0,r0,#0x80			//把r0的[8]位清零,即开CPU的irq总中断
	orr r0,r0,#0x10			//把r0的[4]置一，即M[4:0]=10000 进入usr模式
	msr cpsr, r0			//把r0的值读入cpsr
	ldr sp, =0x57000000		//设置usr模式的堆栈指针地址为0x57000000
	
	/* 故意添加一条未定义指令*/
und_code:
	.word 0xff000000	/* 未定义指令 */

	/* 故意进入软件中断*/
	swi    0x123
	
	/* 调用main函数 */
	//bl main /* 使用BL命令相对跳转。程序仍然在sram中执行 */
	ldr lr, =halt
	ldr pc, =main  /* 绝对跳转，调到SDRAM中运行 */

halt:
    b halt

	/* und中断服务函数 */
do_und:

	/* 执行到这里之前：
	 * 1.lr_und保存有被中断模式中的下一条即将执行的指令的地址
	 * 2.SPSR_und保存有被中断模式的CPSR
	 * 3.CPSR中的M4-M0被设置为11011，进入到und模式
	 * 4.跳到0x4的地方执行程序
	 */
	 
	/* sp_und未设置，先设置它 */
	ldr sp,=0x60000000			//把堆栈指针设置在256MRAM的栈顶，
								//注意sp地址一定要是4字节对齐的地方,否则程序无法运行
	
	/* 保存现场 */
	/* 在und异常处理函数中可能会修改r0-r12，先保存 */
	/* lr是异常处理完成后的地址,也要保存 */
	stmdb sp!, {r0-r12,lr}
	
	/* 处理und异常 */
	mrs r0,cpsr			/*  将CPSR状态寄存器读取，保存到R0中,然后传给printException */
	ldr r1,=und_string  /* 通过r1把und_string字符传给printException */
	bl printException
	
	/* 恢复现场 */
	ldmia sp!,{r0-r12,pc}^    /* ^会把spsr的值恢复到cpsr里 */
	 
und_string:
	.string "und test by sgf"

.align 4
/* swi中断服务函数 */
do_swi:

	/* 执行到这里之前：
	 * 1.lr_svc保存有被中断模式中的下一条即将执行的指令的地址
	 * 2.SPSR_svc保存有被中断模式的CPSR
	 * 3.CPSR中的M4-M0被设置为10011，进入到svc模式
	 * 4.跳到0x8的地方执行程序
	 */
	 
	/* sp_svc未设置，先设置它 */
	ldr sp,=0x5f000000			//把svc的堆栈指针设置在0x5f000000地方
								//注意sp地址一定要是4字节对齐的地方,否则程序无法运行
	
	/* 保存现场 */
	/* 在svc异常处理函数中可能会修改r0-r12，先保存 */
	/* lr是异常处理完成后的地址,也要保存 */
	stmdb sp!, {r0-r12,lr}
	
	/* 处理svc异常 */
	cmp r0 , #0x0
	bleq led1_on
	cmp r0 , #0x1
	bleq led2_on
	
	/* 恢复现场 */
	ldmia sp!,{r0-r12,pc}^    /* ^会把spsr的值恢复到cpsr里 */
	 
svc_string:
	.string "swi test by sgf"

.global led_ctrl

led_ctrl:
	stmdb sp!, {r0-r12,lr}
	swi 0x123
	ldmia sp!,{r0-r12,pc}^

.end
	
	
