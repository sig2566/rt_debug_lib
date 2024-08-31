
/******************************************************************
 * rt_debug_main.cc
 * Author: Igor Shoihet
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/

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
#include "rt_debug_main.h"

using namespace RT_DEBUG;
bool  	CDebugHandler::RTDBG_ActivateDebugDataCollection()
{
	ASSERT(mem_attached_ == false);
	void * retv = NULL;
	//Allocate alignment to 64 bytes address
	size_t size= sizeof(CDebugRT) ;
	mode_t origMask = umask(0);
	ASSERT(strlen(shared_file_name_)>0);
	mmapFd_ = open(shared_file_name_, O_CREAT|O_RDWR, 00666);
	umask(origMask);
	if (mmapFd_ < 0)
	{
	  perror("open mmapFd_ failed");
	  return false;
	}
	if ((ftruncate(mmapFd_, size) == 0))
	{
	  int result = lseek(mmapFd_, size - 1, SEEK_SET);
	  if (result == -1)
	  {
		perror("lseek mmapFd_ failed");
		close(mmapFd_);
		return false;
	  }

	  /* Something needs to be written at the end of the file to
	   * have the file actually have the new size.
	   * Just writing an empty string at the current file position will do.
	   * Note:
	   *  - The current position in the file is at the end of the stretched
	   *    file due to the call to lseek().
			  *  - The current position in the file is at the end of the stretched
	   *    file due to the call to lseek().
	   *  - An empty string is actually a single '\0' character, so a zero-byte
	   *    will be written at the last byte of the file.
	   */
	  result = write(mmapFd_, "", 1);
	  if (result != 1)
	  {
		perror("write mmapFd_ failed");
		close(mmapFd_);
		return false;
	  }
	}
	retv  =  mmap(NULL, size,
				  PROT_READ | PROT_WRITE , MAP_SHARED, mmapFd_, 0);

	if (retv == MAP_FAILED || retv == NULL)
	{
		perror("mmap");
		close(mmapFd_);
		return false;
	}
	status_ = RTDBG_RUNNING_PRODUCER;
	rt_debugp_= (CDebugRT*)retv;
	mem_attached_ = true;
	RTDBG_Init();
	return true;

 }

bool	  	CDebugHandler::RTDBG_AttachDebugData()
{

	void* retv = NULL;
	ASSERT(mem_attached_ == false);
	ASSERT(strlen(shared_file_name_)>0);
    mmapFd_ = open(shared_file_name_, O_RDWR, 00666);
    size_t size= sizeof(CDebugRT);
    if (mmapFd_ < 0)
    {
      return NULL;
    }
    int result = lseek(mmapFd_, 0, SEEK_END);
    if (result == -1)
    {
      perror("lseek mmapFd_ failed");
      close(mmapFd_);
      return NULL;
    }
    if (result == 0)
    {
      perror("The file has 0 bytes");
      close(mmapFd_);
      return NULL;
    }
    retv  =  mmap(NULL, size,
                PROT_READ | PROT_WRITE, MAP_SHARED, mmapFd_, 0);

    if (retv == MAP_FAILED || retv == NULL)
    {
      perror("mmap");
      close(mmapFd_);
      return NULL;
    }
    status_ = RTDBG_RUNNING_CONSUMER;
    rt_debugp_= (CDebugRT*)retv;
    mem_attached_ = true;
    return true;
  }

bool        CDebugHandler::RTDBG_GetTraceEntry(char *trace_entry_str, struct timespec *linux_time)
{
	uint32_t i, ii;
	uint32_t n, n1, min_index= -1;
	uint64_t t64;
	timespec min_time;
	uint32_t num_alloc_grps= rt_debugp_->GetNumGrps();
	CTraceGroup* tracers[MAX_GROUP_NUM];
	if(!mem_attached_)
		return false;

	if(rt_debugp_->IsDataCollected()==true)
		return false;

	min_index = -1;
	clock_gettime(CLOCK_MONOTONIC, &min_time);
	for(i=0; i< num_alloc_grps; i++)
	{
		CGroupDebugRT* grp_p= rt_debugp_->GetDebugGrp(i);
		tracers[i] = grp_p->GetTracer();

		if(grp_trace_res_[i].status == TRACE_EMPTY_DATA)
		{
			continue;
		}
		TraceInfo *trc_datap = &grp_trace_res_[i];

		if(grp_trace_res_[i].status == NO_DATA_IND)
		{
			if(tracers[i]->GetTraceEntry(trc_datap->trace_str, &trc_datap->lin_time)==false)
			{
				grp_trace_res_[i].status = TRACE_EMPTY_DATA; //NO new trace data for group i
				continue;
			}
			grp_trace_res_[i].status = DATA_IND;

		}
		if(DiffBetweenTimespec(trc_datap->lin_time, min_time) < 0)
		{
			min_time= trc_datap->lin_time;
			min_index = i;
		}

	}


	if(min_index == -1)
	{
		return false;
	}
	else
	{
		strcpy(trace_entry_str, grp_trace_res_[min_index].trace_str);
		*linux_time = grp_trace_res_[min_index].lin_time;
		grp_trace_res_[min_index].status= NO_DATA_IND;
		return true;
	}

}

CDebugHandler debug_handler;
#ifdef __cplusplus
#define EXTERN_C extern "C"
#endif
EXTERN_C RT_counter* RTDBG_GetCounter(HANDLER debug_grp, uint32_t num )
{
	return debug_handler.RTDBG_GetCounter(debug_grp, num);
}
EXTERN_C volatile int64_t* RTDBG_AllocCounter(HANDLER debug_grp, char* cnt_name)
{
	return debug_handler.RTDBG_AllocCounter(debug_grp, cnt_name);
}
EXTERN_C uint32_t    RTDBG_GetCountersNum(HANDLER debug_grp)
{
	return debug_handler.RTDBG_GetCountersNum(debug_grp);
}
//TRACE entry API
EXTERN_C void 		RTDBG_AddTrace(HANDLER debug_grp, uint32_t trace_id, uint32_t line_num,
		uint64_t var0, uint64_t var1, uint64_t var2, uint64_t var3)
{
	debug_handler.RTDBG_AddTrace(debug_grp, trace_id, line_num, var0, var1, var2, var3);
}
EXTERN_C bool        RTDBG_GetTraceEntry(char *trace_entry_str, timespec *linux_time)
{
	return debug_handler.RTDBG_GetTraceEntry(trace_entry_str, linux_time);
}
EXTERN_C uint32_t    RTDBG_AddTraceEntry(HANDLER grp, char* format)
{
	return debug_handler.RTDBG_AddTraceEntry(grp, format);
}

//Log Support
EXTERN_C void RTDBG_AddLog(HANDLER debug_grp, char *log_str)
{
	return debug_handler.RTDBG_AddLog(debug_grp, log_str);
}
EXTERN_C  bool RTDBG_GetLog(char *log_str, struct timespec *linux_time, uint64_t grp_mask)
{
	return debug_handler.RTDBG_GetLog(log_str, linux_time, grp_mask);
}

//Profiler support
EXTERN_C uint32_t  	RTDBG_AddProfiler(HANDLER debug_grp, const char *prof_name)
{
	return debug_handler.RTDBG_AddProfiler(debug_grp, prof_name);
}

EXTERN_C void	RTDBG_ProfInit(HANDLER debug_grp, uint32_t prof_id, ProfilePoint *prof_point, int meas_cnt)
{
	debug_handler.RTDBG_ProfInit(debug_grp, prof_id, prof_point, meas_cnt);
}

EXTERN_C void	RTDBG_ProfFlushMeas(HANDLER debug_grp, uint32_t prof_id, ProfilePoint *prof_point)
{
	debug_handler.RTDBG_ProfFlushMeas(debug_grp, prof_id, prof_point);
}

EXTERN_C void		RTDBG_StartProfMeas(HANDLER debug_grp, uint32_t prof_id, ProfilePoint *prof_point)
{
	debug_handler.RTDBG_StartProfMeas(debug_grp, prof_id, prof_point);

}
EXTERN_C void	RTDBG_StopProfMeas(HANDLER debug_grp, uint32_t prof_id, ProfilePoint *prof_point)
{
	return debug_handler.RTDBG_StopProfMeas(debug_grp, prof_id, prof_point);
}

EXTERN_C bool		RTDBG_GetProfInfo(HANDLER debug_grp, uint32_t prof_id, ProfileData *prof_data, char *grp_name, char* prof_name)
{
	return debug_handler.RTDBG_GetProfInfo(debug_grp, prof_id, prof_data, grp_name, prof_name);
}

EXTERN_C void		RTDBG_StopStartProfMeas(HANDLER debug_grp, uint32_t prof_id, ProfilePoint *prof_point)
{
	return debug_handler.RTDBG_StopStartProfMeas(debug_grp, prof_id, prof_point);
}
//General RT Debug API
EXTERN_C void        RTDBG_Init()
{
	debug_handler.RTDBG_Init();
}
EXTERN_C HANDLER     RTDBG_AllocRTDebugGroup(const char* group_name)
{
	return debug_handler.RTDBG_AllocRTDebugGroup(group_name);
}

EXTERN_C void RTDBG_PutProfVal(HANDLER debug_grp, uint32_t prof_id, uint64_t *val, ProfilePoint *prof_point)
{
	return debug_handler.RTDBG_PutProfVal(debug_grp, prof_id, val, prof_point);
}
//Return the name of the group debug_grp
EXTERN_C bool	RTDBG_GetGrpName(HANDLER debug_grp, char  *grp_name)
{
	return debug_handler.RTDBG_GetGrpName(debug_grp, grp_name);
}

EXTERN_C uint32_t	RTDBG_GetGrpNum()
{
	return debug_handler.RTDBG_GetGrpNum();
}
EXTERN_C void        RTDBG_Stop()
{
	debug_handler.RTDBG_Stop();
}
EXTERN_C void        RTDBG_Start()
{
	debug_handler.RTDBG_Start();
}
EXTERN_C void RTDBG_SetSharedMemFile(char* mem_mapped_file)
{
	debug_handler.RTDBG_SetSharedMemFile(mem_mapped_file);
}
EXTERN_C bool  	RTDBG_ActivateDebugDataCollection()
{
	return debug_handler.RTDBG_ActivateDebugDataCollection();
}
EXTERN_C bool  	RTDBG_AttachDebugData()
{
	return debug_handler.RTDBG_AttachDebugData();
}
EXTERN_C bool	RTDBG_ExtractDebugData(char *out_file)
{
	return debug_handler.RTDBG_ExtractDebugData(out_file);
}
EXTERN_C char*       RTDBG_GetVersion()
{
	return debug_handler.RTDBG_GetVersion();
}

EXTERN_C RTDBG_STATUS_T RTDBG_GetStatus()
{
	return debug_handler.RTDBG_GetStatus();
}
EXTERN_C void	RTDBG_Close()
{
	debug_handler.RTDBG_Close();
}
