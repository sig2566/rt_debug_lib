
/******************************************************************
 * rt_debug_main.h
 * Author: Igor Shoihet
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/

#ifndef TOOLS_TRACER_RT_DEBUG_MAIN_H_
#define TOOLS_TRACER_RT_DEBUG_MAIN_H_
#include "rt_debug_types.h"

using RT_DEBUG;
struct TraceInfo
{
	char trace_str[TRACE_STRING_SIZE*2];
	timespec	lin_time;
	GenSysTime  sys_time;
} ;

struct LogSort_Data
{
	LogEntry log_info;
	bool valid = false;
};

/******************************************************************************************//*!
 *@class CDebugHandler
 *@brief The purpose of this class is :
 *@brief Implement RT debugging features call. I serves as a bridge between the process RAM and RT
 *@brief debugging class CDebugRT, which is implemented "inside" shared memory
 *********************************************************************************************/
class CDebugHandler
{
	RTDBG_STATUS_T status_;
	char shared_file_name_[TRACE_STRING_SIZE];
	char out_file_name_[TRACE_STRING_SIZE];
	CDebugRT *rt_debugp_ = NULL;
	uint32_t alloc_debug_grp_num_;
	TraceInfo	grp_trace_res_[MAX_GROUP_NUM];
	LogSort_Data grp_logs_last[MAX_GROUP_NUM];
	int32_t     mmapFd_ =  0;
	volatile bool mem_attached_ = false;

	void InitExtractTables()
	{
		uint32_t i;
		for(i=0; i< MAX_GROUP_NUM; i++)
		{
			grp_trace_res_[i].sys_time.nf= NO_DATA_IND;
			grp_trace_res_[i].sys_time.nsf = 0;
			grp_logs_last[i].valid = false;

		}

	}

public:
	CDebugHandler()
	{
		out_file_name_[0] = 0;
		shared_file_name_[0] = 0;
		status_ = RTDBG_INIT;
		alloc_debug_grp_num_ = 0;
	}
	//Counter API
	RT_counter* RTDBG_GetCounter(HANDLER debug_grp, uint32_t num )
	{
		if(!mem_attached_)
			return NULL;
		CGroupDebugRT* grp_p= rt_debugp_->GetDebugGrp(debug_grp);;
		CRT_counter_grp* grp_cnt= grp_p->GetEventCounter();
		return grp_cnt->Get_counter(num);
	}
	volatile int64_t* RTDBG_AllocCounter(HANDLER debug_grp, char* cnt_name)
	{
		if(!mem_attached_)
			return NULL;
		CGroupDebugRT* grp_p= rt_debugp_->GetDebugGrp(debug_grp);
		CRT_counter_grp* grp_cnt= grp_p->GetEventCounter();
		uint32_t cnt_num= grp_cnt->AllocCnt(cnt_name);
		RT_counter* cnt_p = grp_cnt->Get_counter(cnt_num);
		return &cnt_p->val;
	}
	uint32_t    RTDBG_GetCountersNum(HANDLER debug_grp)
	{
		if(!mem_attached_)
			return -1;
		CGroupDebugRT* grp_p= rt_debugp_->GetDebugGrp(debug_grp);;
		CRT_counter_grp* grp_cnt= grp_p->GetEventCounter();
		return grp_cnt->Get_cnt_num();
	}
	//TRACE entry API
	void 		RTDBG_AddTrace(HANDLER debug_grp, uint32_t trace_id, uint32_t line_num, GenSysTime *sys_time, uint64_t var0 = 0, uint64_t var1 = 0, uint64_t var2 = 0, uint64_t var3 = 0)
	{
		if(!mem_attached_)
			return;
		if(rt_debugp_->IsDataCollected()==false)
			return;
		CGroupDebugRT* grp_p= rt_debugp_->GetDebugGrp(debug_grp);
		CTraceGroup* tracer_p= grp_p->GetTracer();
		tracer_p->AddTrace(trace_id, line_num, sys_time, var0, var1, var2, var3);
	}
	bool        RTDBG_GetTraceEntry(char *trace_entry_str, timespec *linux_time);
	uint32_t    RTDBG_AddTraceEntry(HANDLER grp, char* format)
	{
		if(!mem_attached_)
			return -1;
		//if(rt_debugp_->IsDataCollected()==true)
		//	return -1;
		CGroupDebugRT* grp_p= rt_debugp_->GetDebugGrp(grp);
		CTraceGroup* tracer_p= grp_p->GetTracer();
		return tracer_p->AddTraceFormat(format);
	}

	//General RT Debug API
	void        RTDBG_Init()
	{
		if(!mem_attached_)
			return;

		InitExtractTables();
		rt_debugp_->Init();
	}
	HANDLER     RTDBG_AllocRTDebugGroup(char* group_name)
	{
		if(!mem_attached_)
			return -1;
		//if(rt_debugp_->IsDataCollected()==true)
		//	return -1;
		return rt_debugp_->AllocDbgGrp(group_name);
	}

	bool	RTDBG_GetGrpName(HANDLER debug_grp, char  *grp_name)
	{
		if(!mem_attached_)
			return false;
		CGroupDebugRT* grp_p = rt_debugp_->GetDebugGrp(debug_grp);
		grp_p->GetGrpName(grp_name);
		return true;
	}
	uint32_t	RTDBG_GetGrpNum(){
		if(!mem_attached_)
			return -1;
		return rt_debugp_->GetNumGrps();
	}

	void        RTDBG_Stop()
	{
		if(!mem_attached_)
			return;
		//Prepare trace extract
		InitExtractTables();
		rt_debugp_->SetDataCollection(false);
		status_ = RTDBG_STOPPED;
	}
	void        RTDBG_Start()
	{
		if(!mem_attached_)
			return;
		rt_debugp_->SetDataCollection(true);
	}

	bool		IsDataCollectionActive()
	{
		if(!mem_attached_)
			return false;
		return rt_debugp_->IsDataCollected();
	}
	void 		RTDBG_SetSharedMemFile(char* mem_mapped_file)
	{
		sprintf(shared_file_name_,"/tmp/%s", mem_mapped_file);
	}
	bool	RTDBG_ExtractDebugData(char *out_file)
	{
		if(!mem_attached_)
			return false;
		FILE *fptr= NULL;
		timespec	lin_time;
		TraceInfo trace_tmp;
		char buf[1000];
		uint32_t i, num_grp = rt_debugp_->GetNumGrps();
		fptr = fopen(out_file,"w");
		ASSERT(fptr);
		ASSERT(status_ != RTDBG_RUNNING_PRODUCER);
		ASSERT(rt_debugp_);
		fprintf(fptr,"Event Counters\n");
		fprintf(fptr,"**************\n");
		for(i=0; i< num_grp; i++)
		{
			uint32_t j, num_ev_cnt;
			CGroupDebugRT* grp_p= rt_debugp_->GetDebugGrp(i);
			char grp_name[100];
			grp_p->GetGrpName(grp_name);
			CRT_counter_grp* grp_cntrs_p = grp_p->GetEventCounter();
			num_ev_cnt= grp_cntrs_p->Get_cnt_num();
			for(j=0; j< num_ev_cnt; j++)
			{
				RT_counter* cnt_p =grp_cntrs_p->Get_counter(j);
				fprintf(fptr,"%s, %s, %lld\n", grp_name, cnt_p->cnt_name, cnt_p->val);
			}
		}

		fprintf(fptr,"*************\n");
		fprintf(fptr,"\n\nProfilers\n");
		fprintf(fptr,"*************\n");
		for(i=0; i< num_grp; i++)
		{
			uint32_t j, num;
			CGroupDebugRT* grp_p= rt_debugp_->GetDebugGrp(i);
			char grp_name[100];
			grp_p->GetGrpName(grp_name);
			CProfilerGroup* grp_profp = grp_p->GetProfiler();
			num = grp_profp->GetGrpProfNum();
			for(j=0; j< num; j++)
			{
				ProfileData prof_tmp;
				char prof_name[1000];
				if(grp_profp->GetProfInfo(j, &prof_tmp, prof_name))
				{
					fprintf(fptr, "%s, %s, avg=, %lld, max=, %lld, last=, %lld, max time=, %lld, measurements=, %lld\n",
							grp_name, prof_name, prof_tmp.average_cnt_, prof_tmp.max_cnt_, prof_tmp.last_cnt_, prof_tmp.max_cnt_time_, prof_tmp.meas_num_);
				}
			}
		}

		fprintf(fptr,"**************\n");
		fprintf(fptr,"\n\nTraces\n");
		fprintf(fptr,"**************\n");
		while(RTDBG_GetTraceEntry(buf, &lin_time)== true)
		{
			fprintf(fptr,"%s\n",buf);
		}

		fprintf(fptr,"********\n");
		fprintf(fptr,"\n\nLogs\n");
		fprintf(fptr,"********\n");
		while(RTDBG_GetLog(buf, &lin_time)== true)
		{
			fprintf(fptr,"%s\n",buf);
		}

		fclose(fptr);
		return true;
	}
	bool  		RTDBG_ActivateDebugDataCollection();
	bool	  	RTDBG_AttachDebugData();
	void		RTDBG_Close()
	{
		mem_attached_ = false;
		usleep(1);
		munmap((void*)rt_debugp_, sizeof(CDebugRT));
		close(mmapFd_);
	}
	char*       RTDBG_GetVersion()
	{
		if(!mem_attached_)
			return NULL;
		static char version[]="v0.1";
		return version;
	}
	RTDBG_STATUS_T RTDBG_GetStatus()
	{
		if(!mem_attached_)
			return RTDBG_STOPPED;
		return status_;
	}
	//Log Support
	void RTDBG_AddLog(HANDLER debug_grp, GenSysTime *sys_time, char *log_str)
	{
		if(!mem_attached_)
			return;
		if(rt_debugp_->IsDataCollected()!= true)
			return;
		CGroupDebugRT* grp_p= rt_debugp_->GetDebugGrp(debug_grp);
		CLog_group* log_grp= grp_p->GetLogger();
		log_grp->AddLog(sys_time, log_str);
	}
	bool RTDBG_GetLog(char *log_str, timespec *linux_time)
	{
		bool res = false;
		uint32_t i;
		timespec min_time;
		clock_gettime(CLOCK_MONOTONIC, &min_time);
		uint32_t min_index= -1;
		if(!mem_attached_)
			return false;
		uint32_t num_grp= rt_debugp_->GetNumGrps();
		for(i=0; i<num_grp; i++)
		{
			if(grp_logs_last[i].valid == false)
			{
				//No Log on the table. Extract new log
				CGroupDebugRT* grp_p= rt_debugp_->GetDebugGrp(i);
				CLog_group* log_grp= grp_p->GetLogger();
				grp_logs_last[i].valid =log_grp->GetLogEntry(grp_logs_last[i].log_info.log_str,
						&grp_logs_last[i].log_info.linux_time, &grp_logs_last[i].log_info.sys_time);
				if(grp_logs_last[i].valid)
				{
					//Check time of new log entry
					if(DiffBetweenTimespec(min_time, grp_logs_last[i].log_info.linux_time) > 0)
					{
						min_time= grp_logs_last[i].log_info.linux_time;
						min_index = i;
					}
				}
			}
			else
			{
				if(DiffBetweenTimespec(min_time, grp_logs_last[i].log_info.linux_time) > 0)
				{
					min_time= grp_logs_last[i].log_info.linux_time;
					min_index = i;
				}

			}
		}

		if(min_index == -1)
			return false;

		strcpy(log_str, grp_logs_last[min_index].log_info.log_str);
		*linux_time= grp_logs_last[min_index].log_info.linux_time;
		grp_logs_last[min_index].valid = false;
		return true;
	}

	//Profiler support
	uint32_t  	RTDBG_AddProfiler(HANDLER debug_grp, char *prof_name)
	{
		if(!mem_attached_)
			return -1;
		//ASSERT(rt_debugp_->IsDataCollected()!= true);
		CGroupDebugRT* grp_p= rt_debugp_->GetDebugGrp(debug_grp);
		CProfilerGroup* grp_profp = grp_p->GetProfiler();
		return grp_profp->RegistryProfileEntry(prof_name);
	}
	//Collection profiler messages .
	CProfileCnt* GetProfilerCnt(HANDLER debug_grp, uint32_t prof_id)
	{
		CGroupDebugRT* grp_p= rt_debugp_->GetDebugGrp(debug_grp);
		CProfilerGroup* grp_profp = grp_p->GetProfiler();
		CProfileCnt *prof_ptr= grp_profp->GetProfCounter(prof_id);
		assert(prof_ptr!=NULL);
		return prof_ptr;
	}
	void		RTDBG_StartProfMeas(HANDLER debug_grp, uint32_t prof_id)
	{
		CProfileCnt *prof_ptr= GetProfilerCnt(debug_grp, prof_id);
		prof_ptr->


	}
	void		RTDBG_StopProfMeas(HANDLER debug_grp, uint32_t prof_id)
	{
		CProfileCnt *prof_ptr= GetProfilerCnt(debug_grp, prof_id);
	}
	void		RTDBG_SuspendProfMeas(HANDLER debug_grp, uint32_t prof_id)
	{
		CProfileCnt *prof_ptr= GetProfilerCnt(debug_grp, prof_id);
	}
	void		RTDBG_ResumeProfMeas(HANDLER debug_grp, uint32_t prof_id)
	{
		CProfileCnt *prof_ptr= GetProfilerCnt(debug_grp, prof_id);
	}
	void		RTDBG_PutProfVal(HANDLER debug_grp, uint32_t prof_id, uint32_t val)
	{
		CProfileCnt *prof_ptr= GetProfilerCnt(debug_grp, prof_id);
	}
	uint32_t	RTDBG_GetProfCntrs(HANDLER debug_grp)
	{
		if(!mem_attached_)
			return -1;
		CGroupDebugRT* grp_p= rt_debugp_->GetDebugGrp(debug_grp);
		CProfilerGroup* grp_profp = grp_p->GetProfiler();
		return grp_profp->GetGrpProfNum();

	}
	bool		RTDBG_GetProfInfo(HANDLER debug_grp, uint32_t prof_id, ProfileData *prof_data, char *grp_name, char* prof_name)
	{
		if(!mem_attached_)
			return false;
		CGroupDebugRT* grp_p= rt_debugp_->GetDebugGrp(debug_grp);
		grp_p->GetGrpName(grp_name);
		CProfilerGroup* grp_profp = grp_p->GetProfiler();
		return grp_profp->GetProfInfo(prof_id, prof_data, prof_name);

	}

};




#endif /* TOOLS_TRACER_RT_DEBUG_MAIN_H_ */
