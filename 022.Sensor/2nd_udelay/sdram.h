
#ifndef _sdram_h
#define _sdram_h

#define vi *( volatile unsigned int * ) 

#define set_zero( addr, bit ) ( (vi addr) &= ( ~ ( 1 << (bit) ) ) )
#define set_one( addr, bit ) ( (vi addr) |= ( 1 << ( bit ) ) )

#define set_bit( addr, bit, val ) ( (vi addr) = (( vi addr)&=(~(1<<(bit))) ) | ( (val)<<(bit) ) )

#define set_2bit( addr, bit, val ) ( (vi addr) = (( vi addr)&(~(3<<(bit))) ) | ( (val)<<(bit) ) )

#define set_nbit( addr, bit, len,  val ) \
	( (vi addr) = ((( vi addr)&(~(( ((1<<(len))-1) )<<(bit))))  | ( (val)<<(bit) ) ))

#define get_bit( addr, bit ) ( (( vi addr ) & ( 1 << (bit) )) > 0  )

#define get_val( addr, val ) ( (val) = vi addr )
#define read_val( addr ) ( vi ( addr ) )
#define set_val( addr, val ) ( (vi addr) = (val) )
#define or_val( addr, val ) ( (vi addr) |= (val) ) 

///////////////////////////////

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

// function declare

void sdram_init(void);
int sdram_test(void);
void copy2sdram(void);
void clean_bss(void);


#endif

