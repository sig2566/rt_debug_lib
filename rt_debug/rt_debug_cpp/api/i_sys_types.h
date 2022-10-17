
 /*//******************************************************************
 * i_sys_types.h.h
 * Author: Igor Shoihet
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/


#ifndef _I_SYS_TYPES_H_
#define _I_SYS_TYPES_H_

/*
 * -----------------------------------------------------------
 * Include section
 * -----------------------------------------------------------
 */



#ifdef __cplusplus
extern "C" {
#endif

/*
 * -----------------------------------------------------------
 * MACRO (define) section
 * -----------------------------------------------------------
 */

#define ATTR_ALIGN(x)  __attribute__((aligned((x))))

/*
 * -----------------------------------------------------------
 * Type definition section
 * -----------------------------------------------------------
 */

typedef const char cchar;


#ifndef uint8_t
typedef unsigned char uint8_t;
#endif

#ifndef uint8_ptr
typedef unsigned char* uint8_ptr;
#endif

#ifndef vuint8_t
typedef volatile unsigned char vuint8_t;
#endif

#ifndef uint16_t
typedef unsigned short uint16_t;
#endif

#ifndef vuint16_t
typedef volatile unsigned short vuint16_t;
#endif

#ifndef uint32_t
typedef unsigned int uint32_t;
#endif

#ifndef vuint32_t
typedef volatile unsigned int vuint32_t;
#endif

#ifndef float32_t
typedef float float32_t;
#endif

#ifndef vfloat32_t
typedef volatile float vfloat32_t;
#endif

#ifndef uint64_t
typedef unsigned long long uint64_t;
#endif

#ifndef vuint64_t
typedef volatile unsigned long long vuint64_t;
#endif

#ifndef int8_t
typedef signed char int8_t;
#endif

#ifndef int16_t
typedef signed short int16_t;
#endif

#ifndef int32_t
typedef signed int int32_t;
#endif

#ifndef int64_t
typedef signed long long int64_t;
#endif

#ifndef vchar_t
typedef volatile char vchar_t;
#endif

#ifndef void_t
typedef void void_t;
#endif

#ifndef bool_t
typedef unsigned long bool_t;
#endif

#ifndef bool8_t
typedef unsigned char bool8_t;
#endif

#ifndef db16dot16
typedef int32_t db16dot16;
#endif

#ifndef fix16dot16
typedef int32_t fix16dot16;
#endif

#ifndef fix48dot16
typedef int64_t fix48dot16;
#endif

/*
 * printf function typedef. May be used as type of print routine
 */
typedef int (*PRINTFUNC) (cchar *sformat, ...);

/*
 * Special keywoed used for CLI function declaring.
 * Example: int CLI_FUNCTION foo (int32_t p);
 */
#define CLI_FUNCTION


/*
 * puts_fn_t - Type for pointer to the function puts()
 */
#ifndef puts_fn_t
typedef void (*puts_fn_t)(const char *);
#endif

/* Initialization pairs (address offset + desired value)
 *  for HW registers configuration	*/
typedef struct HW_reg_conf_array_s
{
	uint32_t reg_offset;	/* HW register address offset from base address  	*/
	uint32_t reg_value;	/* initialization value */
} HW_reg_conf_array_t;

//5G definitions
#define BILLION  					(1000000000L)
#define MILLION  					(1000000L)
#define SUBFRAME_USECS    			(1000)
#define FRAME_USECS                 (SUBFRAME_USECS * 10)
#define SCHED_INTERVAL_USECS		(1)
#define MAX_MODULES					(1000)
#define MAX_AREAS					(MAX_MODULES*4)
//Max profiler counters per module
#define MAX_PROFILE_CNT				(100)	
#define MAX_LOGS_STRING_SIZE		(256)
#define LOG_QUEUE_SIZE				(1000)
#define SYS_FRAMES_NUM              (0x3ff)
#define NUM_OF_SF_IN_FRAME			(10)

//Maximal number RBS in 5G
#define MAX_SLOT_RBS 				(275)
#define MAX_TIMER_EVENTS  			(100)
//Number subrarriers in the resource block (RB)
#define NUM_RB_CARRIERS   			(12)

//Number layers
#define NUM_LAYERS        			(4)

//Number symbls in slot.
#define SYMB_IN_SLOT      			(14)

//Number slots, which should be kept in memory
#define NUM_KEPT_SLOTS   			(3)
//  Numerology
#define NUMEROLOGY		 		    (1)
#define NUM_SLOTS_SF			    (1<<NUMEROLOGY)
#define SLOT_DURATION	 		    (SUBFRAME_USECS/NUM_SLOTS_SF)
#define SYMBL_DURATION   		    uint32_t((SLOT_DURATION/MAX_NUM_OFDM_PER_SLOT)*SCALE_FACTOR)
/*
struct Complex
{
    int16_t r;
    int16_t i;
};
*/
struct Complex32fix
{
    int32_t re;
    int32_t im;
};

struct Complex64
{
    int64_t re;
    int64_t im;
};

struct GenSysTime
{
	uint16_t nf;
	uint16_t nsf;
	int32_t offset;
};

struct ProfileData
{
	uint64_t max_cnt_;
	uint64_t max_cnt_time_;
	uint64_t last_cnt_;
	uint64_t average_cnt_;
	uint64_t meas_num_;
	uint64_t fill_cache_line_align[3];
};


#ifdef __cplusplus
}
#endif


#endif // _I_SYS_TYPES_H_

/*
 * -----------------------------------------------------------
 * End of file
 * -----------------------------------------------------------
 */
