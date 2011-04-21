/* Copyright (c) 2010 The Regents of the University of California
 * Barret Rhoden <brho@cs.berkeley.edu>
 * See LICENSE for details.
 *
 * Kernel threading.  These are for blocking within the kernel for whatever
 * reason, usually during blocking IO operations.  Check out
 * Documentation/kthreads.txt for more info than you care about. */

#ifndef ROS_KERN_KTHREAD_H
#define ROS_KERN_KTHREAD_H

#include <ros/common.h>
#include <trap.h>
#include <sys/queue.h>
#include <atomic.h>

struct proc;
struct kthread;
struct semaphore;
struct semaphore_entry;
TAILQ_HEAD(kthread_tailq, kthread);
LIST_HEAD(semaphore_list, semaphore_entry);


/* This captures the essence of a kernel context that we want to suspend.  When
 * a kthread is running, we make sure its stacktop is the default kernel stack,
 * meaning it will receive the interrupts from userspace. */
struct kthread {
	struct trapframe			context;
	uintptr_t					stacktop;
	struct proc					*proc;
	struct syscall				*sysc;
	TAILQ_ENTRY(kthread)		link;
	/* ID, other shit, etc */
};


/* Semaphore for kthreads to sleep on.  0 or less means you need to sleep */
struct semaphore {
	struct kthread_tailq		waiters;
	int 						nr_signals;
	spinlock_t 					lock;
};

struct semaphore_entry {
	struct semaphore sem;
	int fd;
	LIST_ENTRY(semaphore_entry) link;
};
/* This doesn't have to be inline, but it doesn't matter for now */
static inline void init_sem(struct semaphore *sem, int signals)
{
	TAILQ_INIT(&sem->waiters);
	sem->nr_signals = signals;
	spinlock_init(&sem->lock);
}

/* Down and up for the semaphore are a little more low-level than usual, since
 * they are meant to be called by functions that manage the sleeping of a
 * kthread.  For instance, __down_sem() always returns right away.  For now,
 * these are just examples, since the actual usage will probably need lower
 * access. */

/* Down : decrement, if it was 0 or less, we need to sleep.  Returns false if
 * the kthread did not need to sleep (the signal was already there). */
static inline bool __down_sem(struct semaphore *sem, struct kthread *kthread)
{
	bool retval = FALSE;
	spin_lock(&sem->lock);
	if (sem->nr_signals-- <= 0 && kthread != NULL) {
		/* Need to sleep */
		retval = TRUE;
		TAILQ_INSERT_TAIL(&sem->waiters, kthread, link);
	}
	spin_unlock(&sem->lock);
	return retval;
}

/* Ups the semaphore.  If it was < 0, we need to wake up someone, which is the
 * return value.  If you think there should be at most one, set exactly_one. */
static inline struct kthread *__up_sem(struct semaphore *sem, bool exactly_one)
{
	struct kthread *kthread = 0;
	spin_lock(&sem->lock);
	if (sem->nr_signals++ < 0) {
		/* could do something with 'priority' here */
		kthread = TAILQ_FIRST(&sem->waiters);
		if (kthread == NULL) warn ("kthread is null\n");
		TAILQ_REMOVE(&sem->waiters, kthread, link);
		if (exactly_one)
			assert(TAILQ_EMPTY(&sem->waiters));
	} else {
		assert(TAILQ_EMPTY(&sem->waiters));
	}
	spin_unlock(&sem->lock);
	return kthread;
}

void kthread_init(void);
void sleep_on(struct semaphore *sem);
void restart_kthread(struct kthread *kthread);
void kthread_runnable(struct kthread *kthread);
/* Kmsg handler to launch/run a kthread.  This must be a routine message, since
 * it does not return. */
void __launch_kthread(struct trapframe *tf, uint32_t srcid, long a0, long a1,
	                  long a2);

#endif /* ROS_KERN_KTHREAD_H */
