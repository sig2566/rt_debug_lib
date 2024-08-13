/*
 * rt_debug_macro.h
 *
 *  Created on: Oct 8, 2022
 *      Author: igors
 */

//
#ifndef API_RT_DEBUG_ADAP_API_H_
#define API_RT_DEBUG_ADAP_API_H_
#include "rt_debug_defs.h"

#ifdef __cplusplus
extern "C" {
#else
//#define bool uint32_t
#include <stdbool.h>    // bool type
#endif
#undef DEF_GROUP
#define DEF_GROUP(a)  a,
enum E_GROUPS
{
#include "groups.inc"
MAX_GROUPS
};

#undef DEF_EVENT
#define DEF_EVENT(a,b) a##_##b,
enum EGroupEvent
{
#include "events.inc"
 MAX_GROUP_EVENT
};

#undef DEF_TRACE
#define DEF_TRACE(a, b,c) a##_##b,
enum EGroupTrace{
#include "traces.inc"
	MAX_TRACES
};

#undef DEF_PROFILE
#define DEF_PROFILE(a, b) a##_##b,
enum EGroupProfile
{
#include "profiles.inc"
MAX_PROFILER
};

//Initialization of RT debug environment and prepare collection of the debug information into the file mem_map_file
int RT_debug_init(char *mem_map_file);
//Add trace data
int RT_debug_save_trace(enum EGroupTrace id, int line, uint64_t val0, uint64_t val1, uint64_t val2, uint64_t val3);
//Save log data
void RT_debug_save_log(enum E_GROUPS id, int line, char* log_str);

//Get event counter
volatile uint64_t* RT_debug_get_event_cnt(int group_name);

//Init profiling measurement point
void RT_debug_prof_init(int group_prof, ProfilePoint* prof, int update_count);
//Start profiling measurement
void RT_debug_prof_start(int group_prof, ProfilePoint* prof);
//Stop profiling measurement
void RT_debug_prof_stop(int group_prof, ProfilePoint* prof);
//Flush the profiling data when the profiling measurement is stopped
void RT_debug_prof_flush(int group_prof, ProfilePoint* prof);

//Extract profiler data
bool RT_debug_get_prof_data(int id, ProfileData *prof_ptr);
//Start RT debugging
void RT_debug_start();
//Stop RT debugging
void RT_debug_stop();



//Extract collected trace data (traces, logs, event counters, profiler measurements
bool	RT_debug_ExtractDebugData(char *out_file);

//Extract specific log per group
bool RT_debug_get_grp_log(int group_id, char *log_str);

//MACROs
#define RTDBG_SAVE_TRACE(grm_name, trace_name, v0, v1, v2, v3) \
		RT_debug_save_trace(grm_name##_##trace_name, __LINE__, v0, v1, v2, v3);

//Save log
#define RTDBG_SAVE_LOG(grp_name, ...) {\
	char buf[100];\
	sprintf(buf,__VA_ARGS__);\
	RT_debug_save_log(grp_name, __LINE__, buf);\
	};


//Define profiler point
#define RTDBG_INIT_PROF(grp_name, prof_name, prof, meas_cnt) RT_debug_prof_init(grp_name##_##prof_name, prof, meas_cnt);
//Start profiling
#define RTDBG_START_PROF(grp_name, prof_name, prof) RT_debug_prof_start(grp_name##_##prof_name, prof);

//Stop profiling
#define RTDBG_STOP_PROF(grp_name, prof_name, prof) RT_debug_prof_stop(grp_name##_##prof_name, prof);

//Flush profiler data
#define RTDBG_FLUSH_DATA_PROF(grp_name, prof_name, prof) RT_debug_prof_flush(grp_name##_##prof_name, prof);

//Get event counter
#define RTDBG_GET_EVENT_CNTR_PTR(grp_name, event_name)  \
		RT_debug_get_event_cnt(grp_name##_##event_name);



#ifdef __cplusplus
}
#endif
#endif /* API_RT_DEBUG_ADAP_API_H_ */
