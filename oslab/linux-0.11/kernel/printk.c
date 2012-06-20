/*
 *  linux/kernel/printk.c
 *
 *  (C) 2012  Qiang Ren
 */

/*
 * When in kernel-mode, we cannot use printf, as fs is liable to
 * point to 'interesting' things. Make a printf with fs-saving, and
 * all is well.
 */
#include <stdarg.h>
#include <stddef.h>

#include <linux/kernel.h>

#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <linux/sched.h>
#include <asm/segment.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

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
	//int 	fd = 7;//swap文件
	int 	count = 4096;//固定一次写一页
	struct 	file * file;
	struct 	m_inode * inode;
	char *	buf;
	off_t	pos;//文件当前读写指针位置
	int 	block,c;
	struct 	buffer_head * bh;
	char * 	p;
	int 	i = 0;
	
        buf = (page << 12) + LOW_MEM/*page页号对应的物理地址(linux0.11内核直接使用物理地址) page=((addr) - LOW_MEM)>>12*/
	
	if(!(file = task[0]->filp[7])) //从进程0的文件描述符表中得到文件句柄
		return 0;
	inode = file->f_inode;
	
	/*
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
	*/

	if (file->f_flags & O_APPEND)
		pos = inode->i_size;
	else
		pos = file->f_pos;
	while (i<count) {
		if (!(block = create_block(inode,pos/BLOCK_SIZE)))
			break;
		if (!(bh=bread(inode->i_dev,block)))
			break;
		c = pos % BLOCK_SIZE;
		p = c + bh->b_data;
		bh->b_dirt = 1;
		c = BLOCK_SIZE-c;
		if (c > count-i) c = count-i;
		pos += c;
		if (pos > inode->i_size) {
			inode->i_size = pos;
			inode->i_dirt = 1;
		}
		i += c;
		while (c-->0)
			*(p++) = get_fs_byte(buf++);
		brelse(bh);
	}
	inode->i_mtime = CURRENT_TIME;
	if (!(file->f_flags & O_APPEND)) {
		file->f_pos = pos;
		inode->i_ctime = CURRENT_TIME;
	}
	return 1;
}

int f_read_swap(int page)
{
        //int 	fd = 7;//swap文件
        int 	count = 4096;//固定一次读一页
        struct 	file * file;
        struct 	m_inode * inode;
	char *	buf;

        buf = (page << 12) + LOW_MEM/*page页号对应的物理地址(linux0.11内核直接使用物理地址) page=((addr) - LOW_MEM)>>12*/

        if(!(file=task[0]->filp[7])) //从进程0的文件描述符表中得到文件句柄
                return 0;
        inode=file->f_inode;

	/*
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
	*/
	int left,chars,nr;
	struct buffer_head * bh;

	if ((left=count)<=0)
		return 0;
	while (left) {
		if ((nr = bmap(inode,(file->f_pos)/BLOCK_SIZE))) {
			if (!(bh=bread(inode->i_dev,nr)))
				break;
		} else
			bh = NULL;
		nr = file->f_pos % BLOCK_SIZE;
		chars = MIN( BLOCK_SIZE-nr , left );
		file->f_pos += chars;
		left -= chars;
		if (bh) {
			char * p = nr + bh->b_data;
			while (chars-->0)
				put_fs_byte(*(p++),buf++);
			brelse(bh);
		} else {
			while (chars-->0)
				put_fs_byte(0,buf++);
		}
	}
	inode->i_atime = CURRENT_TIME;
	
	return 1;
}
	
