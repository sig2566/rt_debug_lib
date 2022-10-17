/******************************************************************
 * I_cpp_module.h
 * Author: Igor Shoihet
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/
#include <time.h>

#ifndef API_I_CPP_MODULE_H_
#define API_I_CPP_MODULE_H_

/******************************************************************
 *@brief	This function advance timestamp with value of nano-seconds
 *@brief	in format of timespec (old format - used by build oran packets to advance time between packets/symbols)
 *@brief
 *@brief	Input:	original timespec
 *@brief					 advance time in nano-seconds
 *@brief	Output:	new timespec
 *@brief
 *******************************************************************/

static timespec  Preceeding(timespec t1,uint32_t ns_value)
{
	timespec new_t;
	uint64_t t64;
	t64= (uint64_t)t1.tv_sec * BILLION + (uint64_t)t1.tv_nsec+(uint64_t)ns_value;
	new_t.tv_nsec = t64 %BILLION;
	new_t.tv_sec=(t64-t64 %BILLION)/BILLION;
	return new_t;
}
/******************************************************************
 *@brief	This function move back timestamp with value of nano-seconds
 *@brief	in format of timespec (old format - used by build oran packets to advance time between packets/symbols)
 *@brief
 *@brief	Input:	original timespec
 *@brief					 advance time in nano-seconds
 *@brief	Output:	new timespec
 *@brief
 *******************************************************************/
static timespec  Decreasing(timespec t1,int32_t ns_value)
{
	timespec new_t;
	uint64_t t64;
	t64= (uint64_t)t1.tv_sec * BILLION + (uint64_t)t1.tv_nsec - (uint64_t)ns_value;
	new_t.tv_nsec = t64 %BILLION;
	new_t.tv_sec=(t64-t64 %BILLION)/BILLION;
	return new_t;
}
/******************************************************************
 *@brief	This function decrease time of  timeval with value of micro-seconds
 *@brief	in format of timeval (pcap format - used by pcap parser)
 *@brief
 *@brief	Input:	original timeval
 *@brief					 decrease time in micro-seconds
 *@brief	Output:	new timeval
 *@brief
 *******************************************************************/

static timeval DecreasingTimevalUsec(timeval t1,uint32_t usec_value)
{
	timeval new_t;
	uint64_t t64;
	t64= (uint64_t)t1.tv_sec * MILLION + (uint64_t)t1.tv_usec-(uint64_t)usec_value;
	new_t.tv_usec = t64 %MILLION;
	new_t.tv_sec=(t64-t64 %MILLION)/MILLION;
	return new_t;
}

/******************************************************************
 *@brief	This function decrease time of  timeval with value of seconds
 *@brief	in format of timeval (pcap format - used by pcap parser)
 *@brief
 *@brief	Input:	original timeval
 *@brief					 decrease time in seconds
 *@brief	Output:	new timeval
 *@brief
 *******************************************************************/

static timeval DecreasingTimevalSec(timeval t1,uint32_t sec_value)
{
	timeval new_t=t1;
	uint64_t t64;
	if(t1.tv_sec>=sec_value)
		{new_t.tv_sec=new_t.tv_sec-sec_value;}
	else
	{
		printf("Time can't be negative!!\n");
		ASSERT(0);
	}
	return new_t;
}

/******************************************************************
 *@brief	This function decrease time of timeval with value of another timeval
 *@brief	in format of timeval (pcap format - used by pcap parser)
 *@brief
 *@brief	Input:	original timeval
 *@brief					 decrease time in timeval
 *@brief	Output:	new timeval
 *@brief
 *******************************************************************/

static timeval DecreasingTimeval(timeval orig,timeval timeval_value)
{
	timeval timeval_result;
	timeval_result=DecreasingTimevalUsec(orig,timeval_value.tv_usec);
	timeval_result=DecreasingTimevalSec(timeval_result,timeval_value.tv_sec);
	return timeval_result;
}

/******************************************************************
 *@brief	This function increase time of timeval with value of another timeval
 *@brief	in format of timeval (pcap format - used by pcap parser)
 *@brief
 *@brief	Input:	original timeval
 *@brief					 advance time in timeval
 *@brief	Output:	new timeval
 *@brief
 *******************************************************************/

static timeval PreceedingTimevalUsec(timeval t1,uint32_t usec_value)
{
	timeval new_t;
	uint64_t t64;
	t64= (uint64_t)t1.tv_sec * MILLION + (uint64_t)t1.tv_usec+(uint64_t)usec_value;
	new_t.tv_usec = t64 %MILLION;
	new_t.tv_sec=(t64-t64 %MILLION)/MILLION;
	return new_t;
}


//This function substract the time by ns_value
static timespec  ReverseTime(timespec t1,uint32_t ns_value)
{
	timespec new_t;
	uint64_t t64;
	t64= (uint64_t)t1.tv_sec * BILLION + (uint64_t)t1.tv_nsec-(uint64_t)ns_value;
	new_t.tv_nsec = t64 %BILLION;
	new_t.tv_sec=(t64-t64 %BILLION)/BILLION;
	return new_t;
}
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

namespace RT_DEBUG
{

} //namespace 
#define LOG(severity, ... )    {\
	char buf[100];\
	sprintf(buf,__VA_ARGS__);\
	callback_ptr_->ILogData(severity, buf);\
	}
#define LOG_module(severity, ... )    {\
	char buf[100];\
	sprintf(buf,__VA_ARGS__);\
	callback_framework_class_->ILogData(severity, buf);\
	}

#define DEFINE_MEMAREA(size, inc_size, name, Attrib, area_ptr ) {\
	char tmp_name[]= name;\
	CMemArea tmp_area;\
	area_ptr = &tmp_area;\
	tmp_area.Setup(size, inc_size, tmp_name, E_READ, Attrib );\
	EResultT res=  callback_ptr_->IMemAreaDefine(&area_ptr);\
	ASSERT(res == ns_5g_phy::E_OK);\
	}

#define MEMAREA_CONNECT(name, area_ptr, access_t) {\
	char tmp_name[]= name;\
	EResultT res=  callback_ptr_->IMemAreaMount(area_ptr, tmp_name , access_t);\
	ASSERT(res == ns_5g_phy::E_OK);\
	}

#define INIT_PROF_CNT(name, call_back_ptr, prof_cnt){\
	    char tmp_name[]= name;\
	    prof_cnt.Init(tmp_name, call_back_ptr);\
	    }

#endif
