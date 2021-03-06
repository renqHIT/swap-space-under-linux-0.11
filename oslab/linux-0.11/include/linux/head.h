#ifndef _HEAD_H
#define _HEAD_H

typedef struct desc_struct {
	unsigned long a,b;
} desc_table[256];

//add by renq
typedef struct swap_info{
	int valid;
	//to be continue
} swap_info;

typedef struct swap_node {
	unsigned long page; //logical address
	unsigned long frame; //physical address
	int valid;
	struct swap_node *next;	
}swap_node;
//add by renq

extern unsigned long pg_dir[1024];
extern desc_table idt,gdt;

#define GDT_NUL 0
#define GDT_CODE 1
#define GDT_DATA 2
#define GDT_TMP 3

#define LDT_NUL 0
#define LDT_CODE 1
#define LDT_DATA 2

#endif
