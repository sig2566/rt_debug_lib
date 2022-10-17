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

#include "rt_debug_api.h"
#include "rt_debug_adap_api.h"
//Define groups

#undef DEF_GROUP
#define DEF_GROUP(a) #a,
char group_names[]=
{
#include "groups.inc"
"MAX_GROUPS"
};
int group_indexes[MAX_GROUPS];

//Define events

struct SGroupEvent
{
	int group_id;
	volatile uint64_t* event_cnt;
}

struct SGroupEvent group_events[MAX_GROUP_EVENT];

//Define traces

struct SGroupTrace
{
	enum E_GROUPS  group_id;
	int  trace_id;

};
struct SGroupTrace traces[MAX_TRACES] = {0};

//Profiler definition

struct SGroupProfiler
{
	enum E_GROUPS  group_id;
	int  profile_id;
};

struct SGroupProfiler prof_points[MAX_PROFILER];

int file_exists (char *filename) {
  struct stat   buffer;
  return (stat (filename, &buffer) == 0);
}

int RT_debug_init(char *mem_map_file)
{
	int i, count= 0;
    if (file_exists(mem_map_file))
    {
        printf("Attach to %s file\n", mem_map_file);
    	if(RTDBG_AttachDebugData()!=true)
    		return 0;
    }
    else
    {
        printf("Create and initialize the %s file\n", mem_map_file);
        RTDBG_Start();
    }

    //Allocate groups
#undef DEF_GROUP
#define DEF_GROUP(a) group_indexes[i++]= RTDBG_AllocRTDebugGroup(#a);
#include "groups.inc"
    //Activate events
#undef DEF_EVENT
#define DEF_EVENT(a,b) group_events[a##_##b].group_id= group_indexes[a];\
    group_events[a##_##b].event_cnt = RTDBG_AllocCounter(group_indexes[a], #b);
#include "events.inc"

 // Traces
#undef DEF_TRACE
#define DEF_TRACE(a, b)  traces[a##_##b].group_id=group_indexes[a];\
		traces[a##_##b].trace_id=RTDBG_AddTraceEntry(group_indexes[a], b);
i=0;
#include "traces.inc"

#undef DEF_PROFILE
#define DEF_PROFILE(a, b) prof_points[a##_##b].group_id=group_indexes[a];\
		prof_points[a##_##b].profile_id=RTDBG_GetProfCntrs(group_indexes[a]);
#include "profiles.inc"
	RTDBG_Start();
	return 1;

}

int RT_debug_save_trace(enum EGroupTrace id, int line, uint64_t val0, uint64_t val1, uint64_t val2, uint64_t val3)
{
	RTDBG_AddTrace(traces[id].group_id, traces[id].trace_id, line, NULL, val0, val1, val2, val3);
	return 0;
}

int RT_debug_save_log(enum E_GROUPS id, int line, char* log_str)
{
	RTDBG_AddLog(group_indexes[id], line, NULL, char* log_str);
}

volatile uint64_t* RT_debug_get_event_cnt(enum EGroupEvent id)
{

}

void RT_debug_prof_start(enum EGroupProfile id)
{

}
void RT_debug_prof_stop(enum EGroupProfile id)
{

}

void RT_debug_start()
{

}
void RT_debug_stop()
{

}

//Extract collected trace data (traces, logs, event counters, profiler measurements
bool	RT_debug_ExtractDebugData(char *out_file)
{

	return true;
}



