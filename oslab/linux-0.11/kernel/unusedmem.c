#include<signal.h>
#include<asm/system.h>
#include<linux/sched.h>
#include<linux/head.h>
#include<linux/kernel.h>

#define PAGING_MEMORY (15 * 1024 * 1024) 
#define PAGING_PAGES (PAGING_MEMORY >> 12)

extern unsigned char mem_map[PAGING_PAGES] = {0,};

void unusedmem(void)
{
	int i,j,k,free=0;
	long * pg_tbl;

	for(i=0 ; i<PAGING_PAGES ; i++)
		if (!mem_map[i]) free++;
	printk("%d pages free (of %d)\n\r",free,PAGING_PAGES);
	for(i=2 ; i<1024 ; i++) {
		if (1&pg_dir[i]) {
			pg_tbl=(long *) (0xfffff000 & pg_dir[i]);
			for(j=k=0 ; j<1024 ; j++)
				if (pg_tbl[j]&1)
					k++;
			printk("Pg-dir[%d] uses %d pages\n",i,k);
		}
	}
}
