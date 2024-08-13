/******************************************************************
 * i_osa_api.h
 * Author: Igor Shoihet
 * Copyright   : GNU GENERAL PUBLIC LICENSE
 *******************************************************************/
#ifndef I_OS_COMMON_H
#define I_OS_COMMON_H

#define POSIX_API
/*
 * -----------------------------------------------------------
 * Include section
 * -----------------------------------------------------------
 */
//#include "i_sys_utils.h"
/*
 * --------------------------------------------------------------------------------
 * Abstract Wrapper around specific OS features (threads, mutexes, spinlocks, etc.
 * --------------------------------------------------------------------------------
 */


/*-------------- Threads --------------*/
// Thread priorities.
typedef enum
{
    OSA_PREEMPTIVE = 0,
    OSA_NON_PREEMPTIVE,
} OSA_preemp_t;


// Thread priorities.
//Note, currently priorities until OSA_PRIO_LOW ore treated as user priorities and priority above
//      are treated as REAL TIME priorities
typedef enum
{
    OSA_PRIO_LOWEST = 0,
    OSA_PRIO_ALMOST_LOWEST,
    OSA_PRIO_VERY_LOW,
    OSA_PRIO_ALMOST_VERY_LOW,
    OSA_PRIO_LOW,
    OSA_PRIO_ALMOST_LOW,
    OSA_PRIO_MEDIUM,
    OSA_PRIO_ALMOST_HIGH,
    OSA_PRIO_HIGH,
    OSA_PRIO_ALMOST_VERY_HIGH,
    OSA_PRIO_VERY_HIGH,
    OSA_PRIO_ALMOST_HIGHEST,
    OSA_PRIO_HIGHEST,
    OSA_PRIO_MAX
} OSA_prio;



typedef enum
{
  CB_UL_TASK,
  CB_DL_TASK,
  ORAN_UL_TASK,
  ORAN_DL_TASK,
  PUSCH_CHANNEL_EST_TASK,
  UL_CONTROL_TASK,
  DL_CONTROL_TASK
} OSA_TASK_TYPE;

//Scheduler parameters
typedef struct
{
    uint32_t        offset;
    uint32_t        periodic_interval;
    uint64_t        send_val;
    uint32_t        cell_id;
    OSA_TASK_TYPE   task_type;
    OSA_prio        priority;
    void*           callback_mod_api;

} OSA_SCHED_INFO;

#endif /*I_OS_COMMON_H*/

/*
 * -----------------------------------------------------------
 * End of file
 * -----------------------------------------------------------
 */
