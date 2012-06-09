/*
 *  linux/kernel/printk.c
 *
 *  (C) 1991  Linus Torvalds
 */

/*
 * When in kernel-mode, we cannot use printf, as fs is liable to
 * point to 'interesting' things. Make a printf with fs-saving, and
 * all is well.
 */
#include <stdarg.h>
#include <stddef.h>

#include <linux/kernel.h>

#define LOW_MEM 0x100000
#define PAGING_MEMORY (15*1024*1024)
#define PAGING_PAGES (PAGING_MEMORY>>12)
#define MAP_NR(addr) (((addr)-LOW_MEM)>>12)
#define USED 100

static char buf[1024];

extern int vsprintf(char * buf, const char * fmt, va_list args);

int printk(const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i=vsprintf(buf,fmt,args);
	va_end(args);
	__asm__("push %%fs\n\t"
			"push %%ds\n\t"
			"pop %%fs\n\t"
			"pushl %0\n\t"
			"pushl $buf\n\t"
			"pushl $0\n\t"
			"call tty_write\n\t"
			"addl $8,%%esp\n\t"
			"popl %0\n\t"
			"pop %%fs"
			::"r" (i):"ax","cx","dx");
	return i;
}

#include <linux/sched.h>
#include <sys/stat.h>

static char logbuf[1024];
int fprintk(int fd, const char *fmt, ...)
{
	va_list args;
	int count;
	struct file * file;
	struct m_inode * inode;

	va_start(args, fmt);
	count=vsprintf(logbuf, fmt, args);
	va_end(args);

	if (fd < 3) //如果输出到stdout或stderr，直接调用sys_write即可
	{
		__asm__("push %%fs\n\t"
				"push %%ds\n\t"
				"pop %%fs\n\t"
				"pushl %0\n\t"
				"pushl $logbuf\n\t" //注意对于Windows环境来说，是_logbuf,下同
				"pushl %1\n\t"
				"call sys_write\n\t" //注意对于Windows环境来说，是_sys_write,下同
				"addl $8,%%esp\n\t"
				"popl %0\n\t"
				"pop %%fs"
				::"r" (count),"r" (fd):"ax","cx","dx");
	}
	else //假定>=3的描述符都与文件关联。事实上，还存在很多其它情况，这里并没有考虑。
	{
		if (!(file=task[0]->filp[fd])) //从进程0的文件描述符表中得到文件句柄
			return 0;
		inode=file->f_inode;

		__asm__("push %%fs\n\t"
				"push %%ds\n\t"
				"pop %%fs\n\t"
				"pushl %0\n\t"
				"pushl $logbuf\n\t"
				"pushl %1\n\t"
				"pushl %2\n\t"
				"call file_write\n\t"
				"addl $12,%%esp\n\t"
				"popl %0\n\t"
				"pop %%fs"
				::"r" (count),"r" (file),"r" (inode):"ax","cx","dx");
	}
	return count;
}

int f_write_swap(int page)
{
	int fd = 7;//swap文件
	int count = 4096;//固定一次写一页
	struct file * file;
	struct m_inode * inode;
	char addr[4096];
	
        addr = (page << 12) + LOW_MEM//page页号对应的物理地址(linux0.11内核直接使用物理地址) page=((addr) - LOW_MEM)>>12
	
	if (!(file=task[0]->filp[fd])) //从进程0的文件描述符表中得到文件句柄
		return 0;
	inode=file->f_inode;

	__asm__("push %%fs\n\t"		//保存fs寄存器原值（段选择符）
			"push %%ds\n\t" //保存ds寄存器原值
			"pop %%fs\n\t"
			"pushl %0\n\t"  //输入四个参数，原型为：int file_write(struct m_inode * inode, struct file * filp, char * buf, int count)
			"pushl $logbuf\n\t"
			"pushl %1\n\t"
			"pushl %2\n\t"
			"call file_write\n\t"
			"addl $12,%%esp\n\t"
			"popl %0\n\t"
			"pop %%fs"
			::"r" (count),"r" (file),"r" (inode):"ax","cx","dx");
	return count;

}

int f_read_swap(int page)
{
        int fd = 7;//swap文件
        int count = 4096;//固定一次读一页
        struct file * file;
        struct m_inode * inode;
	char addr[4096];

        addr = (page << 12) + LOW_MEM//page页号对应的物理地址(linux0.11内核直接使用物理地址) page=((addr) - LOW_MEM)>>12

        if (!(file=task[0]->filp[fd])) //从进程0的文件描述符表中得到文件句柄
                return 0;
        inode=file->f_inode;

        __asm__("push %%fs\n\t"         //保存fs寄存器原值（段选择符）
                        "push %%ds\n\t" //保存ds寄存器原值
                        "pop %%fs\n\t"
                        "pushl %0\n\t"  //输入四个参数，原型为int file_read(struct m_inode * inode, struct file * filp, char * buf, int count)
                        "pushl $addr\n\t"
                        "pushl %1\n\t"
                        "pushl %2\n\t"
                        "call file_read\n\t"
                        "addl $12,%%esp\n\t"
                        "popl %0\n\t"
                        "pop %%fs"
                        ::"r" (count),"r" (file),"r" (inode):"ax","cx","dx");
        return count;

}
	
