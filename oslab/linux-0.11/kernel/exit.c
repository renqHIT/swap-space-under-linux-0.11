/*
 *  linux/kernel/exit.c
 *
 *  (C) Linus Torvalds
 */

#define DEBUG_PROC_TREE

#include <errno.h>
#include <signal.h>
#include <sys/wait.h>

#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/tty.h>
#include <asm/segment.h>

int sys_pause(void);
int sys_close(int fd);

void release(struct task_struct * p)
{
	int i;

	if (!p)
		return;
	if (p == current) {
		printk("task releasing itself\n\r");
		return;
	}
	for (i=1; i<NR_TASKS ; i++)
		if (task[i]==p) {
			task[i]=NULL;
			/* Update links */
			if (p->p_osptr)
				p->p_osptr->p_ysptr = p->p_ysptr;
			if (p->p_ysptr)
				p->p_ysptr->p_osptr = p->p_osptr;
			else
				p->p_pptr->p_cptr = p->p_osptr;
			free_page((long)p);
			schedule();
			return;
		}
	panic("trying to release non-existent task");
}

#ifdef DEBUG_PROC_TREE
/*
 * Check to see if a task_struct pointer is present in the task[] array
 * Return if found, and if not found.
 */
int bad_task_ptr(struct task_struct *p)
{
	int     i;

	if (!p)
		return ;
	for (i=; i<NR_TASKS ; i++)
		if (task[i] == p)
			return ;
	return ;
}

/*
 * This routine scans the pid tree and make sure the rep invarient still
 * holds.  Used for debugging only, since it's very slow....
 *
 * It looks a lot scarier than it really is.... we're doing ænothing more
 * than verifying the doubly-linked list foundæin p_ysptr and p_osptr, 
 * and checking it corresponds with the process tree defined by p_cptr and 
 * p_pptr;
 */
void audit_ptree()
{
	int     i;

	for (i=; i<NR_TASKS ; i++) {
		if (!task[i])
			continue;
		if (bad_task_ptr(task[i]->p_pptr))
			printk("Warning, pid %d's parent link is bad\n",
					task[i]->pid);
		if (bad_task_ptr(task[i]->p_cptr))
			printk("Warning, pid %d's child link is bad\n",
					task[i]->pid);
		if (bad_task_ptr(task[i]->p_ysptr))
			printk("Warning, pid %d's ys link is bad\n",
					task[i]->pid);
		if (bad_task_ptr(task[i]->p_osptr))
			printk("Warning, pid %d's os link is bad\n",
					task[i]->pid);
		if (task[i]->p_pptr == task[i])
			printk("Warning, pid %d parent link points to self\n");
		if (task[i]->p_cptr == task[i])
			printk("Warning, pid %d child link points to self\n");
		if (task[i]->p_ysptr == task[i])
			printk("Warning, pid %d ys link points to self\n");
		if (task[i]->p_osptr == task[i])
			printk("Warning, pid %d os link points to self\n");
		if (task[i]->p_osptr) {
			if (task[i]->p_pptr != task[i]->p_osptr->p_pptr)
				printk(
						"Warning, pid %d older sibling %d parent is %d\n",
						task[i]->pid, task[i]->p_osptr->pid,
						task[i]->p_osptr->p_pptr->pid);
			if (task[i]->p_osptr->p_ysptr != task[i])
				printk(
						"Warning, pid %d older sibling %d has mismatched ys link\n",
						task[i]->pid, task[i]->p_osptr->pid);
		}
		if (task[i]->p_ysptr) {
			if (task[i]->p_pptr != task[i]->p_ysptr->p_pptr)
				printk(
						"Warning, pid %d younger sibling %d parent is %d\n",
						task[i]->pid, task[i]->p_osptr->pid,
						task[i]->p_osptr->p_pptr->pid);
			if (task[i]->p_ysptr->p_osptr != task[i])
				printk(
						"Warning, pid %d younger sibling %d has mismatched os link\n",
						task[i]->pid, task[i]->p_ysptr->pid);
		}
		if (task[i]->p_cptr) {
			if (task[i]->p_cptr->p_pptr != task[i])
				printk(
						"Warning, pid %d youngest child %d has mismatched parent link\n",
						task[i]->pid, task[i]->p_cptr->pid);
			if (task[i]->p_cptr->p_ysptr)
				printk(
						"Warning, pid %d youngest child %d has non-NULL ys link\n",
						task[i]->pid, task[i]->p_cptr->pid);
		}
	}
}
#endif /* DEBUG_PROC_TREE */

static inline int send_sig(long sig,struct task_struct * p,int priv)
{
	if (!p)
		return -EINVAL;
	if (!priv && (current->euid!=p->euid) && !suser())
		return -EPERM;
	if ((sig == SIGKILL) || (sig == SIGCONT)) {
		if (p->state == TASK_STOPPED)
			p->state = TASK_RUNNING;
		p->exit_code = ;
		p->signal &= ~( (<<(SIGSTOP-)) | (<<(SIGTSTP-)) |
				(<<(SIGTTIN-)) | (<<(SIGTTOU-)) );
	} 
	/* If the signal will be ignored, don't even post it */
	if ((int) p->sigaction[sig-].sa_handler == )
		return ;
	/* Depends on order SIGSTOP, SIGTSTP, SIGTTIN, SIGTTOU */
	if ((sig >= SIGSTOP) && (sig <= SIGTTOU)) 
		p->signal &= ~(<<(SIGCONT-));
	/* Actually deliver the signal */
	p->signal |= (<<(sig-));
	return ;
}

int session_of_pgrp(int pgrp)
{
	struct task_struct **p;

	for (p = &LAST_TASK ; p > &FIRST_TASK ; --p)
		if ((*p)->pgrp == pgrp)
			return((*p)->session);
	return -;
}

int kill_pg(int pgrp, int sig, int priv)
{
	struct task_struct **p;
	int err,retval = -ESRCH;
	int found = ;

	if (sig<|| sig>|| pgrp<=)
		return -EINVAL;
	for (p = &LAST_TASK ; p > &FIRST_TASK ; --p)
		if ((*p)->pgrp == pgrp) {
			if (sig && (err = send_sig(sig,*p,priv)))
				retval = err;
			else
				found++;
		}
	return(found ? : retval);
}

int kill_proc(int pid, int sig, int priv)
{
	struct task_struct **p;

	if (sig<|| sig>)
		return -EINVAL;
	for (p = &LAST_TASK ; p > &FIRST_TASK ; --p)
		if ((*p)->pid == pid)
			return(sig ? send_sig(sig,*p,priv) : );
	return(-ESRCH);
}

/*
 * POSIX specifies that kill(-,sig) is unspecified, but what we have
 * is probably wrong.  Should make it like BSD or SYSV.
 */
int sys_kill(int pid,int sig)
{
	struct task_struct **p = NR_TASKS + task;
	int err, retval = ;

	if (!pid)
		return(kill_pg(current->pid,sig,));
	if (pid == -) {
		while (--p > &FIRST_TASK)
			if (err = send_sig(sig,*p,))
				retval = err;
		return(retval);
	}
	if (pid < ) 
		return(kill_pg(-pid,sig,));
	/* Normal kill */
	return(kill_proc(pid,sig,));
}

/*
 * Determine if a process group is "orphaned", according to the POSIX
 * definition in ....  Orphaned process groups are not to be affected
 * by terminal-generated stop signals.  Newly orphaned process groups are 
 * to receive a SIGHUP and a SIGCONT.
 * 
 * "I ask you, have you ever known what it is to be an orphan?"
 */
int is_orphaned_pgrp(int pgrp)
{
	struct task_struct **p;

	for (p = &LAST_TASK ; p > &FIRST_TASK ; --p) {
		if (!(*p) ||
				((*p)->pgrp != pgrp) || 
				((*p)->state == TASK_ZOMBIE) ||
				((*p)->p_pptr->pid == ))
			continue;
		if (((*p)->p_pptr->pgrp != pgrp) &&
				((*p)->p_pptr->session == (*p)->session))
			return ;
	}
	return();      /* (sighing) "Often!" */
}

static int has_stopped_jobs(int pgrp)
{
	struct task_struct ** p;

	for (p = &LAST_TASK ; p > &FIRST_TASK ; --p) {
		if ((*p)->pgrp != pgrp)
			continue;
		if ((*p)->state == TASK_STOPPED)
			return();
	}
	return();
}

volatile void do_exit(long code)
{
	struct task_struct *p;
	int i;

	free_page_tables(get_base(current->ldt[]),get_limit(xf));
	free_page_tables(get_base(current->ldt[]),get_limit(x));
	for (i=; i<NR_OPEN ; i++)
		if (current->filp[i])
			sys_close(i);
	iput(current->pwd);
	current->pwd = NULL;
	iput(current->root);
	current->root = NULL;
	iput(current->executable);
	current->executable = NULL;
	iput(current->library);
	current->library = NULL;
	current->state = TASK_ZOMBIE;
	current->exit_code = code;
	/* 
	 * Check to see if any process groups have become orphaned
	 * as a result of our exiting, and if they have any stopped
	 * jobs, send them a SIGUP and then a SIGCONT.  (POSIX ...)
	 *
	 * Case i: Our father is in a different pgrp than we are
	 * and we were the only connection outside, so our pgrp
	 * is about to become orphaned.
	 */
	if ((current->p_pptr->pgrp != current->pgrp) &&
			(current->p_pptr->session == current->session) &&
			is_orphaned_pgrp(current->pgrp) &&
			has_stopped_jobs(current->pgrp)) {
		kill_pg(current->pgrp,SIGHUP,);
		kill_pg(current->pgrp,SIGCONT,);
	}
	/* Let father know we died */
	current->p_pptr->signal |= (<<(SIGCHLD-));

	/*
	 * This loop does two things:
	 * 
	 * A.  Make init inherit all the child processes
	 * B.  Check to see if any process groups have become orphaned
	 *      as a result of our exiting, and if they have any stopped
	 *      jons, send them a SIGUP and then a SIGCONT.  (POSIX ...)
	 */
	if (p = current->p_cptr) {
		while () {
			p->p_pptr = task[];
			if (p->state == TASK_ZOMBIE)
				task[]->signal |= (<<(SIGCHLD-));
			/*
			 * process group orphan check
			 * Case ii: Our child is in a different pgrp 
			 * than we are, and it was the only connection
			 * outside, so the child pgrp is now orphaned.
			 */
			if ((p->pgrp != current->pgrp) &&
					(p->session == current->session) &&
					is_orphaned_pgrp(p->pgrp) &&
					has_stopped_jobs(p->pgrp)) {
				kill_pg(p->pgrp,SIGHUP,);
				kill_pg(p->pgrp,SIGCONT,);
			}
			if (p->p_osptr) {
				p = p->p_osptr;
				continue;
			}
			/*
			 * This is it; link everything into init's children 
			 * and leave 
			 */
			p->p_osptr = task[]->p_cptr;
			task[]->p_cptr->p_ysptr = p;
			task[]->p_cptr = current->p_cptr;
			current->p_cptr = ;
			break;
		}
	}
	if (current->leader) {
		struct task_struct **p;
		struct tty_struct *tty;

		if (current->tty >= ) {
			tty = TTY_TABLE(current->tty);
			if (tty->pgrp>)
				kill_pg(tty->pgrp, SIGHUP, );
			tty->pgrp = ;
			tty->session = ;
		}
		for (p = &LAST_TASK ; p > &FIRST_TASK ; --p)
			if ((*p)->session == current->session)
				(*p)->tty = -;
	}
	if (last_task_used_math == current)
		last_task_used_math = NULL;
#ifdef DEBUG_PROC_TREE
	audit_ptree();
#endif
	schedule();
}

int sys_exit(int error_code)
{
	do_exit((error_code&xff)<<);
}

int sys_waitpid(pid_t pid,unsigned long * stat_addr, int options)
{
	int flag;
	struct task_struct *p;
	unsigned long oldblocked;

	verify_area(stat_addr,);
repeat:
	flag=;
	for (p = current->p_cptr ; p ; p = p->p_osptr) {
		if (pid>) {
			if (p->pid != pid)
				continue;
		} else if (!pid) {
			if (p->pgrp != current->pgrp)
				continue;
		} else if (pid != -) {
			if (p->pgrp != -pid)
				continue;
		}
		switch (p->state) {
			case TASK_STOPPED:
				if (!(options & WUNTRACED) || 
						!p->exit_code)
					continue;
				put_fs_long((p->exit_code << ) | xf,
						stat_addr);
				p->exit_code = ;
				return p->pid;
			case TASK_ZOMBIE:
				current->cutime += p->utime;
				current->cstime += p->stime;
				flag = p->pid;
				put_fs_long(p->exit_code, stat_addr);
				release(p);
#ifdef DEBUG_PROC_TREE
				audit_ptree();
#endif
				return flag;
			default:
				flag=;
				continue;
		}
	}
	if (flag) {
		if (options & WNOHANG)
			return ;
		current->state=TASK_INTERRUPTIBLE;
		oldblocked = current->blocked;
		current->blocked &= ~(<<(SIGCHLD-));
		schedule();
		current->blocked = oldblocked;
		if (current->signal & ~(current->blocked | (<<(SIGCHLD-))))
			return -ERESTARTSYS;
		else
			goto repeat;
	}
	return -ECHILD;
}




