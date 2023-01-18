/*
 * rt_debug_macro.h
 *
 *  Created on: Oct 8, 2022
 *      Author: igors
 */
#include "i_sys_types.h"
#ifndef API_RT_DEBUG_ADAP_API_H_
#define API_RT_DEBUG_ADAP_API_H_
#ifdef __cplusplus
extern "C" {
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
//Initialization of RT debug environment
int RT_debug_init(char *mem_map_file);
//Add trace data
int RT_debug_save_trace(enum EGroupTrace id, int line, uint64_t val0, uint64_t val1, uint64_t val2, uint64_t val3);
//Save log data
void RT_debug_save_log(enum E_GROUPS id, int line, char* log_str);

//Get event counter
volatile uint64_t* RT_debug_get_event_cnt(int group_name);

//Start profiling measurement
void RT_debug_prof_start(int group_prof);
//Stop profiling measurement
void RT_debug_prof_stop(int group_prof);

//Extract profiler data
bool RT_debug_get_prof_data(enum EGroupProfile id, ProfileData *prof_ptr);
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
#define RTDBG_SAVE_LOG(grp_name, l) RT_debug_save_log(grp_name, __LINE__, l);

//Start profiling
#define RTDBG_START_PROF(grp_name, prof_name) RT_debug_prof_start(grp_name##_prof_name);

//Stop profiling
#define RTDBG_STOP_PROF(grp_name, prof_name) RT_debug_prof_stop(grp_name##_prof_name);

//Get event counter
#define RTDBG_GET_EVENT_CNTR_PTR(grp_name, event_name)  \
		RT_debug_get_event_cnt(grp_name##_##event_name);



#ifdef __cplusplus
}
#endif
#endif /* API_RT_DEBUG_ADAP_API_H_ */
