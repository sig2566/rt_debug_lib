/*
 * rt_debug_types.h
 *
 *  Created on: Aug 7, 2024
 *      Author: igors
 */

#ifndef RT_DEBUG_RT_DEBUG_CPP_API_RT_DEBUG_DEFS_H_
#define RT_DEBUG_RT_DEBUG_CPP_API_RT_DEBUG_DEFS_H_
#include <stdint.h>

#define ATTR_ALIGN(x)  __attribute__((aligned((x))))
#define CACHE_ALIGNMENT			(64)
#define alignas(X) __attribute__((aligned(X)))

#define NUM_COUNTERS 			(20)
#define MAX_GROUP_NUM 			(10)
#define TRACE_ENTRIES_NUM 		(1000)
#define PROF_FIFO_SIZE 			(10)
#define PROF_GROUP_MAX 			(50)
#define MAX_TRACE_FORMATS 		(50)
#define LOG_GROUP_BUFSIZE 		(128)
#define BILLION  				(1000000000L)

typedef const char cchar;
typedef unsigned long bool_t;
typedef volatile unsigned int vuint32_t;

/*
 * printf function typedef. May be used as type of print routine
 */
typedef int (*PRINTFUNC) (cchar *sformat, ...);

typedef enum
{
	TIME_EVAL,
	CYCLE_EVAL
}EProfileEval_type;

#ifndef __cplusplus
typedef struct
{
#else
struct ProfileData {
#endif
	alignas(CACHE_ALIGNMENT) uint64_t max_cnt_;
	uint64_t max_cnt_time_;
	uint64_t last_cnt_;
	uint64_t average_cnt_;
	uint64_t meas_num_;

#ifdef __cplusplus
	ProfileData()
	{
		Reset();
	}
	void Reset()
	{
		max_cnt_ = last_cnt_ = average_cnt_ = max_cnt_time_ = 0;
		meas_num_ = 0;
	}
};
#else
} ProfileData;
#endif

typedef struct
{
	ProfileData prof_data_;

	uint64_t acc_cnt_; //Support multiple start stop during one measurement
	uint32_t cnt_id_;
	uint32_t max_calls_;
	uint64_t start_val_;
	uint64_t delta_;
	uint32_t	prof_id_;
	EProfileEval_type 	evalType_;
	void *mem_ptr_;

}ProfilePoint;



#endif /* RT_DEBUG_RT_DEBUG_CPP_API_RT_DEBUG_DEFS_H_ */
