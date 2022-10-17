/*
 * rt_debug_macro.h
 *
 *  Created on: Oct 8, 2022
 *      Author: igors
 */
#define "i_sys_types.h"
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
#define DEF_TRACE(a, b,c) a##_##c.
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

int RT_debug_init(char *mem_map_file);

int RT_debug_save_trace(enum EGroupTrace id, int line, uint64_t val0, uint64_t val1, uint64_t val2, uint64_t val3);

int RT_debug_save_log(enum E_GROUPS id, int line, char* log_str);

volatile uint64_t* RT_debug_get_event_cnt(enum EGroupEvent id);

void RT_debug_prof_start(enum EGroupProfile id);
void RT_debug_prof_stop(enum EGroupProfile id);

void RT_debug_start();
void RT_debug_stop();

//Extract collected trace data (traces, logs, event counters, profiler measurements
bool	RT_debug_ExtractDebugData(char *out_file);





#ifdef __cplusplus
}
#endif
#endif /* API_RT_DEBUG_ADAP_API_H_ */
