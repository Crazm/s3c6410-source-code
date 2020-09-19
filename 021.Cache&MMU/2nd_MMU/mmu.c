#define MMU_SECDESC_AP      (3<<10)                 /* AP */
#define MMU_SECDESC_DOMAIN  (0<<5)                  /* 域 */
#define MMU_SECDESC_NCNB    (0<<2)                  /* 不开启CACHE，不使用Write buffer */
#define MMU_SECDESC_WB      (3<<2)                  /* 开启CACHE，使用Write buffer */
#define MMU_SECDESC_TYPE    ((1<<4) | (1<<1))       /* SECDESC类型 */

#define MMU_SECDESC_FOR_IO   (MMU_SECDESC_AP | MMU_SECDESC_DOMAIN | MMU_SECDESC_NCNB | MMU_SECDESC_TYPE)
#define MMU_SECDESC_FOR_MEM   (MMU_SECDESC_AP | MMU_SECDESC_DOMAIN | MMU_SECDESC_WB | MMU_SECDESC_TYPE)


#define IO  1
#define MEM 0

void create_secdesc(unsigned int *ttb, unsigned int va, unsigned int pa, int io)
{
	int index;

	index = va / 0x100000;

	if (io)
		ttb[index] = (pa & 0xfff00000) | MMU_SECDESC_FOR_IO;
	else
		ttb[index] = (pa & 0xfff00000) | MMU_SECDESC_FOR_MEM;
}

/* 创建一级页表
 * VA           PA          CB
 * 0            0           11
 * 0x50000000   0x50000000  11
 * .....
 * 0x5ff00000   0x5ff00000  11
 * 0x70000000   0x70000000  00
 * .....
 * 0x7ff00000   0x7ff00000  00
 * Framebuffer :0x59000000
 * 0x59000000   0x59000000  00
 *
 * link address:
 * 0xB0000000   0x50000000  11
 */
void create_page_table(void)
{
    /* 1. 页表在哪? 0x5b000000(占据16KB) */
	/* ttb: translation table base */
	unsigned int *ttb = (unsigned int *)0x5b000000;

	unsigned int va, pa;

	/* 2. 根据va,pa设置页表条目 */

	/* 2.1 for sram/nor flash */
	create_secdesc(ttb, 0, 0, IO);

	/* 2.2 for 256M sdram */
	va = 0x50000000;
	pa = 0x50000000;
	for (; va <= 0x5ff00000;)
	{
		create_secdesc(ttb, va, pa, MEM);
		va += 0x100000;
		pa += 0x100000;
	}

	/* 2.3 for register: 0x70000000~0x7ff00000 */
	va = 0x70000000;
	pa = 0x70000000;
	for (; va <= 0x7ff00000; )
	{
		create_secdesc(ttb, va, pa, IO);
		va += 0x100000;
		pa += 0x100000;
	}

	/* 2.5 for Framebuffer : 0x59000000 */
	create_secdesc(ttb, 0x59000000, 0x59000000, IO);

	/* 2.6 for link address */
	create_secdesc(ttb, 0xB0000000, 0x50000000, MEM);
}

