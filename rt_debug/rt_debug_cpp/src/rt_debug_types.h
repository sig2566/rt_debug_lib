
#ifndef TOOLS_TRACER_RT_DEBUG_TYPES_H_
#define TOOLS_TRACER_RT_DEBUG_TYPES_H_
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <errno.h>

#include "rt_debug_defs.h"
#include "rt_debug_api.h"
#include "i_cpp_sys_api.h"
//Data definition section
// All strings should be align to 64 bytes


namespace RT_DEBUG
{
static __syscall_slong_t DiffBetweenTimespec  (timespec t1,timespec t2)
{
	timespec new_t;
	uint64_t t64_1,t64_2;
	__syscall_slong_t t64_3;
	t64_1= (uint64_t)t1.tv_sec * BILLION + (uint64_t)t1.tv_nsec;
	t64_2= (uint64_t)t2.tv_sec * BILLION + (uint64_t)t2.tv_nsec;
	t64_3=t64_1-t64_2;
	return t64_3;
}
class CDebugRT;
/*******************************************************************************************//*!
*@class CRT_counter_grp class
*@brief The purpose of this class is :
*@brief It keeps group event counters and allows access to these counters:
*@brief
***********************************************************************************************///
class CRT_counter_grp
{
	alignas(CACHE_ALIGNMENT)  RT_counter rt_counters_[NUM_COUNTERS];
	uint32_t num_alloc_counters_;
	char group_name_[TRACE_STRING_SIZE];

public:
	void Reset(bool attach= false)
	{
		uint32_t i;
		if(attach)
			return;
		num_alloc_counters_= 0;
		for(i=0; i< NUM_COUNTERS; i++)
		{
			rt_counters_[i].val= -1;
			rt_counters_[i].cnt_name[0]= 0;
		}
		group_name_[0]= 0;
	}
	void SetGroupName(char* group_name)
	{
		strcpy(group_name_, group_name);
	}

	CRT_counter_grp()
	{
		num_alloc_counters_= 0;
	}

	int32_t AllocCnt(char *cnt_name)
	{
		uint32_t i;
		//Check if counter is allocated
		for(i=0; i<num_alloc_counters_; i++)
		{
			if(strcmp(rt_counters_[i].cnt_name, cnt_name)==0)
			{
				return i;
			}
		}
		//Try to add new counter
		if(num_alloc_counters_ < (NUM_COUNTERS-1))
		{
			if(strlen(cnt_name)>(TRACE_STRING_SIZE-1))
			{
				printf("Error: cnt_name %s too big", cnt_name);
				ASSERT(0);
			}
			strcpy(rt_counters_[i].cnt_name, cnt_name);
			num_alloc_counters_++;
			return i;

		}
		printf("Error: trying to allocate too many counters\n");
		ASSERT(0);
		return -1;
	}

	uint32_t Get_cnt_num()
	{
		return num_alloc_counters_;
	}

	RT_counter* Get_counter(uint32_t cnt_num)
	{
		ASSERT(cnt_num <= num_alloc_counters_);
		return &rt_counters_[cnt_num];
	}

};

//Tracer definitions 64 bytes size

struct Trace_entry
{
	timespec 		linux_time;
	uint16_t 		trace_id;
	uint16_t		thread_id;
	uint32_t 		line_num;
	uint32_t		status;
	uint32_t		future_use;
	uint64_t 		vals[4];
	};
const uint16_t NO_DATA_IND= -1;
const uint16_t DATA_IND = 0;
const uint32_t TRACE_EMPTY_DATA = 1;
/*******************************************************************************************//*!
*@class CTraceGroup class
*@brief The purpose of this class is :
*@brief It keeps traces per group and allows access to these traces:
*@brief
***********************************************************************************************///
class CTraceGroup
{
	char trace_formats_[MAX_TRACE_FORMATS][TRACE_STRING_SIZE];
	StaticMemArea<Trace_entry,TRACE_ENTRIES_NUM> trace_fifo_;
	uint32_t num_alloc_formats_;
	char group_name_[TRACE_STRING_SIZE];

public:
	void Reset(bool attach= false);

	void SetGroupName(char* group_name);

	int32_t AddTraceFormat(char *format_str);
	void AddTrace(uint32_t trace_id, uint32_t line_num, uint64_t var0 = 0, uint64_t var1 = 0, uint64_t var2 = 0, uint64_t var3 = 0);

	bool GetTraceEntry(char *trace_entry_str, timespec *linux_time);

};
const uint32_t LOG_STRING_SIZE= TRACE_STRING_SIZE - sizeof(timespec);
struct LogEntry
{
	timespec 		linux_time;
	char log_str[LOG_STRING_SIZE]; //Log string length < 96
};

class CLog_group
{
	StaticMemArea<LogEntry,LOG_GROUP_BUFSIZE> logs_fifo_;
	char group_name_[TRACE_STRING_SIZE];

public:
	void SetGroupName(char* group_name)
	{
		strcpy(group_name_, group_name);
	}
	void Reset(bool attach= false)
	{
		uint32_t i;
		if(!attach)
		{
			char log_name[]= "Log_data";
			logs_fifo_.Setup(LOG_GROUP_BUFSIZE, sizeof(LogEntry), log_name);
		}
	}
	CLog_group()
	{
	}

	void AddLog(char *log_str)
	{
		LogEntry tmp_entry, dummy_entry;;
		clock_gettime(CLOCK_MONOTONIC, &tmp_entry.linux_time);

		strncpy(tmp_entry.log_str, log_str, LOG_STRING_SIZE);
		while(logs_fifo_.PushFIFO_MT(&tmp_entry, sizeof(LogEntry))==E_FAIL)
		{
			logs_fifo_.PopFIFO_MT(&dummy_entry, sizeof(LogEntry));
		}
	}

	bool GetLogEntry(char *log_entry, timespec *linux_time)
	{

		uint32_t n, n1;
		uint64_t t64;
		LogEntry tmp_entry;
		if(logs_fifo_.PopFIFO_MT(&tmp_entry, sizeof(tmp_entry))==NULL)
		{
			return false;
		}

		t64= (uint64_t)tmp_entry.linux_time.tv_sec * BILLION + (uint64_t)tmp_entry.linux_time.tv_nsec;
		n=  sprintf(log_entry, "%lu, %s,  %s", t64, group_name_, tmp_entry.log_str);
		*linux_time= tmp_entry.linux_time;
		return true;
	}
};

/******************************************************************************************//*!
	*@class CProfileCnt
	*@brief The purpose of this class is :
	*@brief This class implements the profiler counter.
	*********************************************************************************************/


class  alignas(CACHE_ALIGNMENT) CProfileCnt : public ProfilePoint
{

public:
void ResetProfData()
{
	prof_data_.max_cnt_ = prof_data_.last_cnt_ = prof_data_.average_cnt_ = prof_data_.max_cnt_time_ = 0;
	prof_data_.meas_num_ = 0;
}
void ResetInit()
{
	ResetProfData();
	cnt_id_ =0;
	max_calls_= 500;
	acc_cnt_ = 0;
	start_val_ = 0;
	delta_ = 0;
	evalType_ = TIME_EVAL;//TIME_EVAL;//CYCLE_EVAL;
	prof_id_ = -1;
	mem_ptr_ = NULL;
}

void SetMaxCalls(uint32_t val)
{
	max_calls_= val;
}
void Init(CMemArea *mem_ptr, uint32_t max_count= 500)
{
	struct timespec t1, t0;

	ResetInit();
	mem_ptr_ = (void*)mem_ptr;
	if(evalType_ == TIME_EVAL)
	{
		int8_t number_delta_calls_iterations = 10;
		delta_ = 0;
		for(int8_t i=0; i<number_delta_calls_iterations; i++)
		{
			clock_gettime(CLOCK_MONOTONIC, &t0);
			clock_gettime(CLOCK_MONOTONIC, &t1);
			delta_ += (((uint64_t)t1.tv_sec * BILLION + (uint64_t)t1.tv_nsec) - ((uint64_t)t0.tv_sec * BILLION + (uint64_t)t0.tv_nsec));
		}
		delta_ /= number_delta_calls_iterations; //average amount of clocks for calling gettime

	}
	else
	{
		// call twice in order to ensure that the tsc code is located in cash and its running time is more correct
		delta_ 		= tsc();
		start_val_	= tsc();
		delta_ 		= tsc();
		start_val_	= tsc();
		delta_ = start_val_ - delta_;
		start_val_ = 0;

	}

}
void Update()
{
	if(prof_data_.meas_num_ == 0)
		return;
	prof_data_.average_cnt_ = prof_data_.average_cnt_/prof_data_.meas_num_;
	CMemArea* mem_ptr = (CMemArea*)mem_ptr_;
	while(mem_ptr->PushFIFO_MT(&prof_data_, sizeof(prof_data_))==E_FAIL )
	{
		mem_ptr->PopFIFO_MT(&prof_data_, sizeof(prof_data_));
	}

	ResetProfData();
}
void Start()
{
	ASSERT(start_val_== 0);
	//__sync_synchronize();
	struct timespec t1, t0;
	prof_data_.last_cnt_ =1; //Informs that the Start function was called

	if(evalType_ == TIME_EVAL)
	{
		//clock_gettime(CLOCK_MONOTONIC, &t0);
		clock_gettime(CLOCK_MONOTONIC, &t1);
		//delta_ = (uint64_t)t0.tv_sec * BILLION + (uint64_t)t0.tv_nsec;
		start_val_ = (uint64_t)t1.tv_sec * BILLION + (uint64_t)t1.tv_nsec;
	}
	else
	{
		// call twice in order to ensure that the tsc code is located in cash and its running time is more correct
		start_val_	= tsc();
	}
	//__sync_synchronize();
}

#if defined(__x86_64__) || defined(__amd64__)
inline	uint64_t tsc(void)
{
	register uint32_t lo, hi;
	asm volatile ("rdtsc" : "=a" (lo), "=d" (hi));
	return ((uint64_t)hi << 32UL) | (uint32_t)lo;
}
#elif defined(__aarch64__)
inline	uint64_t tsc(void)
{

/*
// this code should work on our ARM processor since it's ver8 architecture, but it does not recognize "mrc" assembly command for some reason
#if (__ARM_ARCH >= 6)
	uint32_t pmccntr;
	uint32_t pmuseren;
	uint32_t pmcntenset;
	  // Read the user mode perf monitor counter access permissions.
	  asm volatile("mrc p15, 0, %0, c9, c14, 0" : "=r"(pmuseren));
	  if (pmuseren & 1) {  // Allows reading perfmon counters for user mode code.
		asm volatile("mrc p15, 0, %0, c9, c12, 1" : "=r"(pmcntenset));
		if (pmcntenset & 0x80000000ul) {  // Is it counting?
		  asm volatile("mrc p15, 0, %0, c9, c13, 0" : "=r"(pmccntr));
		  // The counter is set up to count every 64th cycle
		  return static_cast<uint64_t>(pmccntr) * 64;  // Should optimize to << 6
		}
	  }
#endif
*/

	  // System timer of ARMv8 runs at a different frequency than the CPU's.
	  // The frequency is fixed, typically in the range 1-50MHz.  It can be
	  // read at CNTFRQ special register.  We assume the OS has set up
	  // the virtual timer properly.

	int64_t virtual_timer_value;
	asm volatile("mrs %0, cntvct_el0" : "=r"(virtual_timer_value));
	return (uint32_t)virtual_timer_value;
}
#endif


void StopContinue()
{
	struct timespec t1;
	uint64_t t64;
	if(evalType_ == TIME_EVAL)
	{
		clock_gettime(CLOCK_MONOTONIC, &t1);
		t64= (uint64_t)t1.tv_sec * BILLION + (uint64_t)t1.tv_nsec;
	}
	else
	{
		t64 = tsc();
	}
	acc_cnt_ += t64 - start_val_;
	start_val_ = 0;

}
void ForceStop()
{
	Update();
	Reset();
}
void Stop(uint64_t *prof_valp = NULL)
{
	//__sync_synchronize();
	struct timespec t1;
	volatile uint64_t t64;
	if(prof_valp==NULL)
	{
		//Calculate time profiling between calling start and stop functions
		if(evalType_ == TIME_EVAL)
		{
			clock_gettime(CLOCK_MONOTONIC, &t1);
			t64= (uint64_t)t1.tv_sec * BILLION + (uint64_t)t1.tv_nsec;
		}
		else
		{
			t64 = tsc();
		}

		int64_t tmp_val = t64 - start_val_ + acc_cnt_ - delta_;// check if calling to both, the tested code and gettime function, took less time than delta_ time (it can happen in case of caching issues)
		if(tmp_val < 0)
		{
			prof_data_.last_cnt_ = 20; //add artificial small number of nsecs (20nsec is about 30 cycles for 1.6 Ghz CPU clock)
		}
		else
		{
			prof_data_.last_cnt_= tmp_val;
		}
	}
	else
	{
		//Calculate profiling of any values
		prof_data_.last_cnt_= *prof_valp;
	}
	//prof_cnt_.last_cnt_=  t64 - start_val_ + acc_cnt_ - delta_;
	//prof_cnt_.last_cnt_=  t64 - start_val_ + acc_cnt_; // force delta_ = 0 since there is sometime values of the counter less than delta and then we get negative measurement
	//ASSERT(delta_ < 10000L)
	//ASSERT((t64 - start_val_) < 100000000000L)
	//ASSERT(acc_cnt_ == 0)
	//ASSERT(prof_cnt_.last_cnt_ < 100000000000L)
	acc_cnt_ = 0;
	if(prof_data_.last_cnt_ > prof_data_.max_cnt_)
	{
		prof_data_.max_cnt_ = prof_data_.last_cnt_;
		prof_data_.max_cnt_time_ = start_val_;
	}
	prof_data_.average_cnt_ += prof_data_.last_cnt_;
	start_val_ = 0;
	prof_data_.meas_num_++;
	if((max_calls_ !=0) &&(  max_calls_ <= prof_data_.meas_num_))
	{
		Update();
	}
	//__sync_synchronize();
}


void Reset()
{
	ResetProfData();
	start_val_ = 0;
	acc_cnt_ = 0;
}

};
/*******************************************************************************************//*!
*@class CTraceGroup class
*@brief The purpose of this class is :
*@brief It keeps profiler counters per group and allows access to these traces:
*@brief
***********************************************************************************************///
class CProfilerGroup
{
	StaticMemArea<ProfileData,PROF_FIFO_SIZE> 	  prof_fifo_[PROF_GROUP_MAX];
	uint32_t	  num_alloc_prof_;
	char group_name_[TRACE_STRING_SIZE];
public:
	void Reset(bool attach= false)
	{
		if(!attach)
		{
			uint32_t i;
			num_alloc_prof_= 0;
			group_name_[0]= 0;
		}
	}
	CProfilerGroup()
	{
		Reset(false);
	}

	CMemArea *GetProfEntryFifo(uint32_t prof_id_)
	{
		return &prof_fifo_[prof_id_];
	}
	void SetGroupName(char* group_name)
	{
		strcpy(group_name_, group_name);
	}

	uint32_t RegistryProfileEntry(char* prof_name)
	{
		uint32_t i;
		ASSERT(num_alloc_prof_<=PROF_GROUP_MAX);
		for(i=0; i< num_alloc_prof_; i++)
		{
			if(strcmp(prof_fifo_[i].name_,prof_name)==0)
			{
				return i;
			}
		}
		num_alloc_prof_++;
		prof_fifo_[i].Setup(PROF_FIFO_SIZE, sizeof(ProfileData), prof_name);
		prof_fifo_[i].MemAlloc();
		return i;
	}

	uint32_t AddProfileEntry(int prof_entry_num, ProfilePoint *prof_entry)
	{
		CProfileCnt *prof_ptr= static_cast<CProfileCnt*>(prof_entry);
		prof_ptr->Init(static_cast<CMemArea*>(&prof_fifo_[prof_entry_num]));
		return 0;
	}
	uint32_t GetGrpProfNum()
	{
		return num_alloc_prof_;
	}
	bool GetProfInfo(uint32_t i, ProfileData *prof_data_, char *prof_name)
	{
		bool res = false;
		ASSERT(i<num_alloc_prof_);
		uint32_t measure_cnt=0;
		ProfileData tmp_cnt, cnt_val;
		char buf[200];


		while(prof_fifo_[i].PopFIFO_MT((void*)&tmp_cnt, sizeof(ProfileData))!= NULL)
		{
			if(tmp_cnt.meas_num_ == 0)
			{
				continue;
			}
			res = true;
			cnt_val.last_cnt_ = tmp_cnt.last_cnt_;
			if(cnt_val.max_cnt_ < tmp_cnt.max_cnt_)
			{
			    cnt_val.max_cnt_ = tmp_cnt.max_cnt_;
			    cnt_val.max_cnt_time_ = tmp_cnt.max_cnt_time_;
			}
			cnt_val.average_cnt_ += tmp_cnt.average_cnt_ * tmp_cnt.meas_num_;
			cnt_val.meas_num_ += tmp_cnt.meas_num_;
		}
		if(res)
		{
			cnt_val.average_cnt_ = cnt_val.average_cnt_ /cnt_val.meas_num_;
			*prof_data_ = cnt_val;
			strcpy(prof_name,prof_fifo_[i].name_);
		}
		return res;
	}

	void AddProfData(uint32_t prof_id_, ProfileData *prof_data_)
	{
		ASSERT(prof_id_<num_alloc_prof_);
		ProfileData tmp_data;
		while(prof_fifo_[prof_id_].PushFIFO_MT((void*)prof_data_, sizeof(ProfileData))==E_FAIL )
		{
			prof_fifo_[prof_id_].PopFIFO_MT((void*)&tmp_data, sizeof(ProfileData));
		}
	}

};
/*******************************************************************************************//*!
*@class CGroupDebugRT class
*@brief The purpose of this class is :
*@brief This class keeps RT debugging group components and allows access to them.
*@brief
***********************************************************************************************///
class CGroupDebugRT
{
	alignas(CACHE_ALIGNMENT)  CLog_group 		logger_;
	alignas(CACHE_ALIGNMENT)  CProfilerGroup 	profiler_;
	alignas(CACHE_ALIGNMENT)  CTraceGroup  	tracer_;
	alignas(CACHE_ALIGNMENT)  CRT_counter_grp	evemt_counters_;
	char group_name_[TRACE_STRING_SIZE];

public:
	CGroupDebugRT()
	{

	}
	void Reset(bool attach= false)
	{
		logger_.Reset(attach);
		profiler_.Reset(attach);
		tracer_.Reset(attach);
		evemt_counters_.Reset(attach);
	}

	//Group initialization
	void Init(char* group_name)
	{
		Reset();
		strncpy(group_name_, group_name, TRACE_STRING_SIZE);
		logger_.SetGroupName(group_name);
		profiler_.SetGroupName(group_name);
		tracer_.SetGroupName(group_name);
		evemt_counters_.SetGroupName(group_name);
	}

	void GetGrpName(char *grp_name)
	{
		strcpy(grp_name, group_name_);
	}
	CLog_group* GetLogger()
	{
		return &logger_;
	}
	CProfilerGroup* GetProfiler()
	{
		return &profiler_;
	}
	CTraceGroup* GetTracer()
	{
		return &tracer_;
	}
	CRT_counter_grp* GetEventCounter()
	{
		return &evemt_counters_;
	}
};

/*******************************************************************************************//*!
*@class CDebugRT class
*@brief The purpose of this class is :
*@brief This class implements RT debugging features. It contains array of
*@brief debugging groups. All class data are located in the shared memory.
***********************************************************************************************///
class CDebugRT
{
public:
	uint32_t test_val;
	uint32_t num_alloc_groups_;
	alignas(CACHE_ALIGNMENT)   volatile bool active_;
	alignas(CACHE_ALIGNMENT)   volatile bool trace_collected_;

	alignas(CACHE_ALIGNMENT)  CGroupDebugRT debug_groups_[MAX_GROUP_NUM];
	alignas(CACHE_ALIGNMENT)  char filler[64];


public:
	CDebugRT()
	{
	}

	//
	void SetDataCollection(bool start_ind)
	{
		active_ = start_ind;
	}
	bool IsDataCollected()
	{
		return active_;
	}
	void Init()
	{
		uint32_t i;
		num_alloc_groups_= 0;
		trace_collected_= false;
		active_ = false;
		for(i=0;i<MAX_GROUP_NUM; i++)
		{
			debug_groups_[i].Reset(false);
		}
	}

	void Attach()
	{
		uint32_t i;
		bool attach= true;
		for(i=0;i<MAX_GROUP_NUM; i++)
		{
			debug_groups_[i].Reset(attach);

		}
	}

	CGroupDebugRT* GetDebugGrp(uint32_t grp_num)
	{
		ASSERT(grp_num < num_alloc_groups_);
		return &debug_groups_[grp_num];
	}

	uint32_t GetNumGrps()
	{
		return num_alloc_groups_;
	}

	uint32_t AllocDbgGrp(char* group_name)
	{
		uint32_t i;
		char tmp_str[TRACE_STRING_SIZE];
		for(i=0; i<num_alloc_groups_; i++)
		{
			debug_groups_[i].GetGrpName(tmp_str);
			if(strcmp(tmp_str, group_name)== 0)
			{
				return i;
			}
		}
		num_alloc_groups_++;
		ASSERT(num_alloc_groups_<= MAX_GROUP_NUM);
		debug_groups_[i].Init(group_name);
		return i;
	}

};
}; //RT_DEBUG Namespace
#endif /* TOOLS_TRACER_RT_DEBUG_TYPES_H_ */
