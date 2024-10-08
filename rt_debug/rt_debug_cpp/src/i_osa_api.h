/******************************************************************
 * i_osa_api.h
 * Author: Igor Shoihet
 *******************************************************************/
#ifndef _OSA_API_H
#define _OSA_API_H

#define POSIX_API
/*
 * -----------------------------------------------------------
 * Include section
 * -----------------------------------------------------------
 */
#ifdef POSIX_API

#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <sys/time.h>
#include <cassert>
#include <stdint.h>

#endif

//#include "i_sys_utils.h"
#include "i_os_common.h"
/*
 * --------------------------------------------------------------------------------
 * Abstract Wrapper around specific OS features (threads, mutexes, spinlocks, etc.
 * --------------------------------------------------------------------------------
 */

#define OSA_TYPE_THX

#define OSA_OK		(0)
#define OSA_ERROR	(-1)
#define OSA_FAIL	(-1)

#define OSA_DEFAULT_STACK_SIZE	1024
#define OSA_DEFAULT_PRIORITY	10
#define ASSERT assert

// align the size to the nearest aligned value
#define ALIGN_SIZE(x, ALIGNMENT_VALUE) (((x)+ALIGNMENT_VALUE-1)&(~(ALIGNMENT_VALUE-1)))

// add to the size an additional section with alignment value size in order to be able to align the allocated memory to the alignment value
#define ADD_SIZE_FOR_MEM_ALIGN(x, ALIGNMENT_VALUE) ((x)+ALIGNMENT_VALUE-1)
// align the pointer to the nearest aligned value
#define ALIGN_POINTER(x, ALIGNMENT_VALUE) ((uint64_t)((((uint64_t)(x))+ALIGNMENT_VALUE-1)&(~(ALIGNMENT_VALUE-1))))

#define ALIGN_128BIT(n)				((n) & ~0x0000000f)
#define INC_128BIT(n)				((n) + 0x00000010)
#define CHECK_ALIGN_128BIT(n)		(!((n) & 0x0000000c))

#define ALIGN_64BIT(n)              ((n) & ~0x00000007)
#define INC_64BIT(n)                ((n) + 0x00000008)
#define CHECK_ALIGN_64BIT(n)	    (!((n) & 0x00000007))

#define ALIGN_32BIT(n)              ((n) & ~0x00000003)
#define INC_32BIT(n)                ((n) + 0x00000004)
#define CHECK_ALIGN_32BIT(n)		(!((n) & 0x00000003))

#define ALIGN_4SAMPLES(n)			((n) & ~0x00000003)
#define CHECK_ALIGN_4SAMPLES(n)		(!((n) & 0x00000003))

#define BITS_TO_BYTES(n)            ((((n) + 7)) >> 3)
#define BITS_TO_WORDS(n)            ((((n) + 31)) >> 5)
#define ALIGN_TO_4(n)				(((n) + 3) & ~3)


#define W32_TO_BYTES(n)				((n) << 2)
#define BYTES_TO_W32(n)				((n) >> 2)

#define	OSA_MEM_BL16_POOL_ID	0
#define	OSA_MEM_BL32_POOL_ID	1
#define	OSA_MEM_BL48_POOL_ID	2
#define	OSA_MEM_BL64_POOL_ID	3
#define	OSA_MEM_BL128_POOL_ID	4
#define	OSA_MEM_BL192_POOL_ID	5
#define	OSA_MEM_BL256_POOL_ID	6
#define	OSA_MEM_BL512_POOL_ID	7
#define	OSA_MEM_BYTE_POOL_ID	8	/* HEAP */
#define OSA_MEM_MAX_PUB_POOLS	(OSA_MEM_BYTE_POOL_ID+1)
#define OSA_MEM_MAX_PRIV_POOLS	5
#define OSA_MEM_MAX_POOLS		(OSA_MEM_MAX_PUB_POOLS+OSA_MEM_MAX_PRIV_POOLS)

#define OSA_TICK_LEN		      	10
#define OSA_MESSAGE_LEN_4BYTE	 	1

#define OSA_SIZEOF_THREAD_TCB		sizeof(TX_THREAD)


#define	OSA_TICK_DIVISOR_FACTOR_DEFAULT  1


/*
 * -----------------------------------------------------------
 * Type definition section
 * -----------------------------------------------------------
 */

typedef enum osa_utl_flags_e
{
	OSA_UTL_TRACEX = 0,
	OSA_UTL_STACK_CHECK,
} OSA_utl_flags_t;


typedef enum
{
	BYTE_POOL,
	BLOCK_POOL
} OSA_mem_pool_type_t;

typedef short	OSA_mem_pool_id;

extern unsigned long	*mem_pub_num_of_blocks;
extern void *tx_sys_exception_ptr;

extern "C" typedef void (*TX_THREAD_ENTRY_t)(unsigned int);
extern "C" typedef  void *(*THREAD_ROUTINE_PTR(void *));

typedef void (OSA_handler_func)();
typedef OSA_handler_func *OSA_handler;


//

typedef struct
{
	uint32_t 			priority;
	uint32_t			stack_size;
	void 			    *stack_addr;
    uint32_t			auto_start;
	uint32_t 			preempt;
	uint32_t			tid;

} OSA_thread_attr_t;


#ifdef POSIX_API
#define OSA_thread_t       pthread_t
#define OSA_spinlock_t     pthread_spinlock_t
#define OSA_mutex_t        pthread_mutex_t
#define OSA_thread_attr_t  pthread_attr_t
#endif

typedef unsigned long OSA_thread_t;
typedef unsigned long OSA_stat_t;

void OSA_threads_print();

static void OSA_thread_attr_init(OSA_thread_attr_t *attr_p)
{
#ifdef POSIX_API
    pthread_attr_init(attr_p);
#endif
}
static void  OSA_thread_attr_destroy(OSA_thread_attr_t *attr_p)
{
    uint32_t res;
#ifdef POSIX_API
    res = pthread_attr_destroy(attr_p);
   ASSERT(res==0);
#endif
}
//void OSA_thread_attr_set_thread_name(void *attr, char *name);

void OSA_thread_attr_set_autostart(void *attr, unsigned int auto_start);
static void OSA_thread_attr_set_stack_size(OSA_thread_attr_t *attr, uint32_t stack_size)
{
#ifdef POSIX_API
    int32_t s;
    s = pthread_attr_setstacksize(attr, stack_size);
    ASSERT(s ==0);
#endif
}

static void OSA_thread_attr_set_priority(OSA_thread_attr_t *attr, OSA_prio priority)
{
#ifdef POSIX_API
    int32_t s;
    struct sched_param param;
    ASSERT(priority < OSA_PRIO_MAX);
    if(priority <= OSA_PRIO_LOW)
    {
        //Set low priority
        param.__sched_priority = priority;
        s = pthread_attr_setschedpolicy(attr, SCHED_OTHER);
        ASSERT(s==0);

    }
    else
    {
      //Set thread with real time priority
        int32_t max_fifo = sched_get_priority_max(SCHED_FIFO);
        ASSERT(pthread_attr_setschedpolicy(attr, SCHED_FIFO) == 0);
        param.__sched_priority = max_fifo - (OSA_PRIO_MAX - priority);
    }
    ASSERT(pthread_attr_setschedparam(attr, &param)== 0);
#endif
}
//void OSA_thread_attr_get_priority(void *attr, unsigned int *priority_p);


static void OSA_thread_create( pthread_t *thread, const
        pthread_attr_t *thread_attr,
        void *(*start_routine) (void *),
        void *arg)
{
#ifdef POSIX_API
    ASSERT(pthread_create(thread, thread_attr, start_routine, arg) == 0);
#endif
}
//void OSA_thread_destroy(void *thread);

/*-------------- Mutex --------------*/

static void OSA_mutex_create(OSA_mutex_t *mutex)
{
#ifdef POSIX_API
    ASSERT(pthread_mutex_init(mutex, NULL) == 0 );
#endif
}
static void OSA_mutex_destroy(OSA_mutex_t *mutex)
{
#ifdef POSIX_API
    ASSERT(pthread_mutex_destroy(mutex)==0);
#endif
}
static int OSA_mutex_trylock(OSA_mutex_t *mutex)
{
#ifdef POSIX_API
    return pthread_mutex_trylock(mutex);
#endif
}
static void OSA_mutex_lock(OSA_mutex_t *mutex)
{
#ifdef POSIX_API
    ASSERT(pthread_mutex_lock(mutex) == 0);
#endif
}
static void OSA_mutex_unlock(OSA_mutex_t *mutex)
{
#ifdef POSIX_API
    ASSERT(pthread_mutex_unlock(mutex) == 0);
#endif
}

/*-------------- Semaphores --------------*/

void *OSA_sem_create(char *name, unsigned int value);
int OSA_sem_destroy(void *sem_p);
int OSA_sem_wait(void *sem_p);
int OSA_sem_trywait(void *sem_p);
int OSA_sem_post(void *sem_p);
int OSA_sem_ceiling_post(void *sem_p, unsigned int value);

/*-------------- Spinlocks --------------*/

static void OSA_spinlock_create(OSA_spinlock_t *lock)
{
#ifdef POSIX_API
    ASSERT(pthread_spin_init(lock, PTHREAD_PROCESS_SHARED) == 0);
#endif
}
static void OSA_spinlock_destroy(OSA_spinlock_t *lock)
{
#ifdef POSIX_API
    ASSERT(pthread_spin_destroy(lock)==0);
#endif
}
static void OSA_spinlock_lock(OSA_spinlock_t *lock)
{
#ifdef POSIX_API
    ASSERT(pthread_spin_lock(lock) == 0);
#endif
}
static int32_t OSA_spinlock_trylock(OSA_spinlock_t *lock)
{
#ifdef POSIX_API
    return pthread_spin_trylock(lock);
#endif
}
static void OSA_spinlock_unlock(OSA_spinlock_t *lock)
{
#ifdef POSIX_API
    ASSERT(pthread_spin_unlock(lock) == 0);
#endif
}



/*-------------- Messages Queue --------------*/
/*
 * -----------------------------------------------------------
 * Function:	OSA_mq_create_full
 * Description:	Create message queue of (size )elements, (dim)*sizeof(uint32_t) size each
 * -----------------------------------------------------------
 */
void *OSA_mq_create( const char *name, 	int	size);
/*
 * -----------------------------------------------------------
 * Function:	OSA_mq_create_full
 * Description:	Create message queue of (size )elements, (dim)*sizeof(uint32_t) size each
 * -----------------------------------------------------------
 */
void *OSA_mq_create_full( const char *name, 	int	size, int dim);
void OSA_mq_destroy(void *mqdes);


unsigned int OSA_mq_wait_receive(void *qid, void *mptr,  unsigned int tval);
unsigned int OSA_mq_try_receive(void *qid, void *mptr);
unsigned int OSA_mq_receive(void *qid, void *mptr);
int OSA_mq_wait_send(void *qid, void *mptr, unsigned int tval);
int OSA_mq_try_send(void *qid, void *mptr);

/*-------------- Memory management --------------*/

/*
 * -----------------------------------------------------------
 * Function:	OSA_malloc
 * Description:	Allocate memory from the specified pool
 * Input:		The pointer to the memory pool,
 * 			The size needed for the data
 * -----------------------------------------------------------
 */

void *OSA_malloc(void *pool, unsigned int size);
void *OSA_malloc_aligned
(
	void			*pool,		/* The pointer to the memory pool       */
	unsigned int		size,		/* The size needed for the data         */
	unsigned int		alignment	/* How memory should be aligned         */
);

void OSA_free(void	*pool, 	void *ptr);
void *OSA_mem_alloc(unsigned int mem_size);
void OSA_mem_free(void* mem_ptr);

/*
 * -----------------------------------------------------------
 * Function:	OSA_block_pool_create
 * Description:	Create block pool
 * -----------------------------------------------------------
 */
void *OSA_block_pool_create
(
		const char		*name,
		unsigned int	block_size,
		void			*pool_area_ptr,
		unsigned int	pool_size
);

/*
 * -----------------------------------------------------------
 * Function:	OSA_block_pool_delete
 * Description:	Delete block pool
 * -----------------------------------------------------------
 */
void OSA_block_pool_delete(void *block_pool_ptr);

/*
 * -----------------------------------------------------------
 * Function:	OSA_block_alloc
 * Description:	Create block pool
 * -----------------------------------------------------------
 */
void *OSA_block_alloc(void *block_pool_ptr);

/*
 * -----------------------------------------------------------
 * Function:	OSA_block_free
 * Description:	Release block
 * -----------------------------------------------------------
 */
void OSA_block_free(void *mem_ptr);


/*-------------- Misc --------------*/
static void OSA_sleep(uint32_t ticks)
{
    usleep(ticks);
}
void OSA_thread_yield(void);
unsigned OSA_thread_get_tid(void *thread);
void OSA_thread_chng_priority(void *thread_p, unsigned int new_priority);
unsigned long OSA_get_ticks( void );
void *OSA_thread_current(void);
void OSA_sleep(unsigned int ticks);

void  OSA_irq_sys_tick_on();
void  OSA_sys_tick_off(void);
void  OSA_irq_sub_tick_on(void);
void  OSA_sub_tick_off(void);
void  OSA_thread_suspend(void *thread);
void  OSA_thread_resume(void *thread);

unsigned int OSA_get_stackghost_secret(void);


/*
 * -----------------------------------------------------------
 * Static inline functions section
 * -----------------------------------------------------------
 */


/*
 * -----------------------------------------------------------
 * Global prototypes section
 * -----------------------------------------------------------
 */

void OSA_init_rtosapi(void);

/*
 * -----------------------------------------------------------
 * Function:	OSA_check_thread_stack
 * Description:	Checking stack overflow for all created threads
 * Input:		None
 * Output:		0-OK,
 * -----------------------------------------------------------
 */
unsigned int OSA_check_thread_stack_all(uint32_t mode, uint8_t node);

void  OSA_node_status(PRINTFUNC fprint);


/*
 * -----------------------------------------------------------
 * Function:	OSA_mem_init_pool
 * Description:	Initialize memory allocation stuff and assign the pool
 * 
 * Input:		mem - The pointer to the start of available memory
 *				size - size of the available memory
 *				pool - pointer to the pool object. In case pool is not 
 *				allocated previously (NULL is submitted), allocate the pool
 *				object from the mem area.
 *				allocate_from_pool - flag telling from which memory pool 
 *				should be allocated the pool object itself. If TRUE then
 *				memory pointed by mem is used. Otherwise HEAP is used.
 *				
 * Return Value: pointer to the pool object 
 * -----------------------------------------------------------
 */
void * OSA_mem_init_pool(void * pool, void *mem, uint32_t size, bool_t allocate_from_pool);


/*
 * -----------------------------------------------------------
 *
 * Function:	OSA_mem_init
 * Description:	Initialize memory allocation stuff
 * Input:		The pointer to the start of available memory
 * -----------------------------------------------------------
 */
void OSA_mem_init(void *mem, uint32_t size);

/* xtensa_init.c */
extern unsigned tx_sys_timer_hook_ptr;	/* user initialization hook */
						/* running in system tick interrupt context */
extern unsigned tx_sys_timer1_hook_ptr;/* user initialization hook */

extern unsigned OSA_sub_tick_timer_hook_ptr;
extern uint32_t	OSA_tick_divisor_factor;
extern vuint32_t  OSA_status_req;

extern uint32_t OSA_THX_medint2_tick;
extern uint32_t OSA_THX_medint3_tick;
extern uint32_t OSA_THX_medint4_tick;

 	 	 	 	 	 	 	 /* running in timer1 interrupt context */
#define OSA_sys_timer_hook_ptr		tx_sys_timer_hook_ptr
#define OSA_sys_fast_timer_hook_ptr	tx_sys_timer1_hook_ptr

extern void *OSA_start_free_mem;


void OSA_set_irq_cb(void *fnc_ptr);
void OSA_set_fiq_cb(void *fnc_ptr);


void OSA_timer_interrupt(void);


/*
 * Set timer duration of built-in TIMER #1 used as RTOS clock, when �fast� tick used by TFSH module is enabled,
 * "val" parameter defined new IRQ period, by default TFSH module set val=5, OSA_tick_divisor_1ms/ val = 200 mks.
 * With this new timer period configuration RTOS tick will be incremented as usual every 1 ms, but foreground task
 * and background task created by TFSH will be resumed every [1 msec/�val�] mks. Default value of foreground/background
 * task resuming could be changed by user, see note below.
*/
unsigned int OSA_set_tick_divisor_factor(unsigned int val);


/*
 * Set timer duration of built-in TIMER #1 used as RTOS clock,
 * RTOS tick interrupt will be generated every "val" tick�s of CPU/DSP,
 * default value provides 1 msec OSA tick period loaded during OSA initialization.
 */
unsigned int OSA_set_tick_divisor(unsigned int val);


/*
 * Set timer duration of built-in TIMER #2 dedicated for PROFILER or other IRQ driven service,
 * timer duration and enable/disable IRQ from this source controlled by TFSH_profile_start/TFSH_profile_stop routines.
 */
unsigned int OSA_set_sub_tick_divisor(unsigned int val);

void OSA_trace_irq_enter(uint32_t irq_id);
void OSA_trace_irq_exit(uint32_t irq_id);
void OSA_user_event_insert(uint32_t event_id, uint32_t data0, uint32_t data1, uint32_t data2, uint32_t data3);
void OSA_trx_trace_buf_enable(bool_t full);
uint32_t OSA_trx_trace_buf_disable(void);

void OSA_register_exceptions_handler(OSA_handler f);

extern void OSA_crash_report_print(PRINTFUNC fprint);

void OSA_init_bgr_ctrl(void);
void OSA_utl_ctrl_set_flag(uint8_t node, OSA_utl_flags_t flag, bool_t val);
bool_t OSA_bgr_ctrl();

/*
* OSA_get_system_state()
* returns the value of the ThreadX internal variable _tx_thread_system_state. If value larger then 0 then system is within an interrupt
*/
uint32_t OSA_get_system_state();

#endif /*_OSA_API_H*/

/*
 * -----------------------------------------------------------
 * End of file
 * -----------------------------------------------------------
 */
