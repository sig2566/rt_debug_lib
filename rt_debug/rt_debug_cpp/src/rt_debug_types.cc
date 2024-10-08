/******************************************************************
 * rt_debug_types.cc
 * Author: Igor Shoihet
 *******************************************************************/
#include <unistd.h>
#include <sys/syscall.h>
#include "rt_debug_types.h"


//Tracer methods
using namespace RT_DEBUG;


void CTraceGroup::Reset(bool attach)
{
	uint32_t i;
	if(attach== false)
	{
		group_name_[0] = 0;
		num_alloc_formats_= 0;
		for(i=0; i< MAX_TRACE_FORMATS; i++)
		{
			trace_formats_[i][0]= 0;
		}

		//Add default format
		char def_format[TRACE_STRING_SIZE]= "val0= %llu, val1=%llu, val2= %llu, val3= %llu";
		AddTraceFormat(def_format);
		for(i=0; i< TRACE_ENTRIES_NUM; i++)
		{
			trace_fifo_.GetEntry(i)->status = NO_DATA_IND;  //Init trace entry indication
		}
		//Alloc Trace MemArea
		char trace_name[]= "Trace data";
		trace_fifo_.Setup(TRACE_ENTRIES_NUM, sizeof(Trace_entry), trace_name);
	}
	trace_fifo_.MemAlloc();
}

void CTraceGroup::SetGroupName(const char* group_name)
{
	strcpy(group_name_, group_name);
}

int32_t CTraceGroup::AddTraceFormat(char *format_str)
{
	uint32_t i;
	ASSERT(strlen(format_str)< TRACE_STRING_SIZE);
	for(i=0; i< num_alloc_formats_; i++)
	{
		if(strcmp(trace_formats_[i], format_str)==0)
		{
			return i;
		}
	}
	ASSERT(num_alloc_formats_<MAX_TRACE_FORMATS);
	strcpy(trace_formats_[i], format_str);
	num_alloc_formats_++;
	return i;
}
void CTraceGroup::AddTrace(uint32_t trace_id, uint32_t line_num, uint64_t var0, uint64_t var1, uint64_t var2, uint64_t var3)
{

	Trace_entry *trace_entry_ptr= (Trace_entry *)trace_fifo_.AllocPoolChunk();
	Trace_entry tmp_entry;
	ASSERT(trace_id< num_alloc_formats_);
	clock_gettime(CLOCK_MONOTONIC, &tmp_entry.linux_time);

	//tmp_entry.thread_id = syscall(SYS_gettid);
	tmp_entry.thread_id = (uint16_t)pthread_self();
	tmp_entry.trace_id= trace_id;
	tmp_entry.line_num = line_num;
	tmp_entry.vals[0] = var0;
	tmp_entry.vals[1] = var1;
	tmp_entry.vals[2] = var2;
	tmp_entry.vals[3] = var3;
	tmp_entry.status  = DATA_IND;
	//*trace_entry_ptr = tmp_entry;
	memcpy(trace_entry_ptr, &tmp_entry, sizeof(tmp_entry));
}

bool CTraceGroup::GetTraceEntry(char *trace_entry_str, timespec *linux_time)
{
	uint32_t i;
	uint32_t n, n1, min_index= -1;
	uint64_t t64;
	timespec min_time;
	clock_gettime(CLOCK_MONOTONIC, &min_time);
	//Find  entry with minimal timestamp
	for(i=0; i< TRACE_ENTRIES_NUM; i++)
	{
		if(trace_fifo_.GetEntry(i)->status==NO_DATA_IND)
		{
			continue;
		}

		if(DiffBetweenTimespec(min_time, trace_fifo_.GetEntry(i)->linux_time) > 0)
		{
			min_time= trace_fifo_.GetEntry(i)->linux_time;
			min_index = i;
		}

	}
	if(min_index== -1)
	{
		//No entry available
		return false;
	}
	//Extract the trace entry data and clear the entry
	Trace_entry *trace_entry_ptr= trace_fifo_.GetEntry(min_index);

	ASSERT(trace_entry_ptr->trace_id<num_alloc_formats_);
	char *format= trace_formats_[trace_entry_ptr->trace_id];
	t64= (uint64_t)trace_entry_ptr->linux_time.tv_sec * BILLION + (uint64_t)trace_entry_ptr->linux_time.tv_nsec;

	trace_entry_str[0] = 0;
	n=  sprintf(trace_entry_str, "%lu, %s, %d, %d, ", t64, group_name_,  trace_entry_ptr->thread_id, trace_entry_ptr->line_num);
	n1= sprintf(trace_entry_str +n, format, trace_entry_ptr->vals[0], trace_entry_ptr->vals[1], trace_entry_ptr->vals[2] , trace_entry_ptr->vals[3] );
	*linux_time= trace_entry_ptr->linux_time;

	trace_entry_ptr->status= NO_DATA_IND;
	return true;
}
