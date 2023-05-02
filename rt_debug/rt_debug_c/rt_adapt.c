/*
 * rt_adapt.c
 *
 *  Created on: Oct 10, 2022
 *      Author: igors
 *
 */
#include <sys/stat.h>   // stat
#include <stdbool.h>    // bool type
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "rt_debug_api.h"
#include "rt_debug_adap_api.h"
//Define groups

#undef DEF_GROUP
#define DEF_GROUP(a) (char*)#a,
char *group_names[]=
{
#include "groups.inc"
"MAX_GROUPS"
};
int group_indexes[MAX_GROUPS];

//Define events

typedef struct
{
	int group_id;
	volatile uint64_t* event_cnt;
	char event_name[TRACE_STRING_SIZE];
}SGroupEvent;

SGroupEvent group_events[MAX_GROUP_EVENT];

//Define traces

typedef struct SGroupTrace
{
	enum E_GROUPS  group_id;
	int  trace_id;

}SGroupTrace;

SGroupTrace traces[MAX_TRACES] = {0};

//Profiler definition

typedef struct
{
	enum E_GROUPS  group_id;
	int  profile_id;
}SGroupProfiler;

SGroupProfiler prof_points[MAX_PROFILER];

int file_exists (char *filename) {
  struct stat   buffer;
  return (stat (filename, &buffer) == 0);
}
//Initialization of RT debug environment
int RT_debug_init(char *mem_map_file)
{
	int i= 0, count= 0;
    if (file_exists(mem_map_file))
    {
        printf("Attach to %s file\n", mem_map_file);
        RTDBG_SetSharedMemFile(mem_map_file);
    	if(RTDBG_AttachDebugData()!=true)
    		return 0;
    }
    else
    {
        printf("Create and initialize the %s file\n", mem_map_file);
        RTDBG_SetSharedMemFile(mem_map_file);
        RTDBG_ActivateDebugDataCollection();
        RTDBG_Start();
    }


    //Allocate groups
#undef DEF_GROUP
#define DEF_GROUP(a) group_indexes[a]= RTDBG_AllocRTDebugGroup(#a);
#include "groups.inc"

    //Activate events
#undef DEF_EVENT
// a- grop name, b -- event name
#define DEF_EVENT(a,b) group_events[a##_##b].group_id= group_indexes[a];\
	{char tmp[]= #b;\
	strncpy(group_events[a##_##b].event_name, tmp, TRACE_STRING_SIZE-1);\
    group_events[a##_##b].event_cnt = RTDBG_AllocCounter(group_indexes[a], #b);}
#include "events.inc"

 // Traces
#undef DEF_TRACE
#define DEF_TRACE(a, b, c)  traces[a##_##b].group_id=group_indexes[a];\
		traces[a##_##b].trace_id=RTDBG_AddTraceEntry(group_indexes[a], c);
i=0;
#include "traces.inc"

#undef DEF_PROFILE
#define DEF_PROFILE(a, b) prof_points[a##_##b].group_id=group_indexes[a];\
		prof_points[a##_##b].profile_id=RTDBG_AddProfiler(group_indexes[a], #b);
#include "profiles.inc"
	RTDBG_Start();
	return 1;

}
//Add trace data
int RT_debug_save_trace(enum EGroupTrace id, int line, uint64_t val0, uint64_t val1, uint64_t val2, uint64_t val3)
{
	RTDBG_AddTrace(traces[id].group_id, traces[id].trace_id, line, NULL, val0, val1, val2, val3);
	return 0;
}
//Save log data
void RT_debug_save_log(enum E_GROUPS id, int line, char* log_str)
{
	return RTDBG_AddLog(group_names[id], NULL, log_str);
}

volatile uint64_t* RT_debug_get_event_cnt(int group_name)
{
	return group_events[group_name].event_cnt;
}
//Init profiling measurement point
void RT_debug_prof_init(int group_prof, ProfilePoint* prof, int meas_cnt)
{
	int group_id= prof_points[group_prof].group_id;
	int prof_id= prof_points[group_prof].profile_id;
	RTDBG_ProfInit(group_id, prof_id, prof, meas_cnt);
}
//Flush the profiling data when the profiling measurement is stopped
void RT_debug_prof_flush(int group_prof, ProfilePoint* prof)
{
	int group_id= prof_points[group_prof].group_id;
	int prof_id= prof_points[group_prof].profile_id;
	RTDBG_ProfFlushMeas(group_id, prof_id, prof);
}
void RT_debug_prof_start(int group_prof, ProfilePoint* prof)
{
	int group_id= prof_points[group_prof].group_id;
	int prof_id= prof_points[group_prof].profile_id;
	return RTDBG_StartProfMeas(group_id, prof_id, prof);
}
void RT_debug_prof_stop(int group_prof, ProfilePoint* prof)
{
	int group_id= prof_points[group_prof].group_id;
	int prof_id= prof_points[group_prof].profile_id;
	return RTDBG_StopProfMeas(group_id, prof_id, prof);
}

//Extract profiler data
bool RT_debug_get_prof_data(int id, ProfileData *prof_ptr)
{
	int group_id= prof_points[id].group_id;
	int prof_id= prof_points[id].profile_id;
	char gr_name[80], prof_name[80];
	return RTDBG_GetProfInfo(group_id, prof_id, prof_ptr, gr_name, prof_name);
}


void RT_debug_start()
{
	RTDBG_Start();
}
void RT_debug_stop()
{
	RTDBG_Stop();
}

//Extract collected trace data (traces, logs, event counters, profiler measurements
bool	RT_debug_ExtractDebugData(char *out_file)
{
	RTDBG_Stop();
	return RTDBG_ExtractDebugData(out_file);
}

//Extract specific log per group
bool RT_debug_get_grp_log(int group_id, char *log_str)
{
	if(log_str==NULL)
	{
		printf("ERROR: log_str is point to NULL in the RT_debug_get_grp_log function\n");
				return false;
	}
	if(group_id>MAX_GROUP_NUM-1)
	{
		printf("ERROR: group id is more then maximal \n");
		return false;
	}
	uint64_t mask = 1<<group_id;
	return RTDBG_GetLog(log_str, NULL, mask);

}

