# rt_debug_lib
Real Time Debugging Library

## Introduction
This document describes the open source project, called rt_debug_lib. The rt_debug_lib provides debugging tools, intended for debugging real time applications with minimal influence to performance. 

## Main features
* Support number tools with different purposes: Traces, Profilers points, Event counters, Logs.
* Saving data in the shared memory, that allow saving the debugging data after application was crushed.
* Support debugging multi-core and multi-processes applications.
* The tools was tested for Linux, but it may be easily ported to other operation systems.
* Tested with x86 and ARM processors.
* Integration of the RT debugging tools with C/C++ projects is supported with examples..  

## The real time debugging tools
* Real time tracer. The tracer is set of traceponts, which are added to the code. Once the tracepoint is called during the program execution, some information is collected in the cyclic buffer. When the traces may be extracted when the debugged application is stopped or the trace collection was stopped.There are numbertypes of tracepoints. They may be differ by collected information and its presentation. The format of the information presentation 
* Real time profiler. The profiler points measure timing interval between start and stop points.
* Event counter Event counter is 64 bit variable, which address is associated with its name. The application may get this address from the tracer and read any values. Event counters read/write operation are very fast and may be used for debugging critical real time code and to save the system status.
* Real time logs allow logging the application data. It works slower than traces, but allows saving any information.

## Organization of the collected debugging tool data.
The debugging information, collected during the test is saved in the shared memory. So it survise the application crush and may be restored and checked after it.
### Data Hierarchy
The data hierarchy is following:
* General debugger class **CDebugRT**. It contains array of the Debugging groups of type ##CGroupDebugRT##. The array size is set by the **MAX_GROUP_NUM** variable.
  * The class **CGroupDebugRT** is group class. The every debugging group is indipendent object. It may run indipendently by different applications or different modules of the application. It includes objects for the debugging tools.
    * Logger is implemented with the class **CLog_group**. The logger is responsible to keep logs in the internal FIFO. The special monitor should extract the logs in run time or it may be extracted when the application failed.
    * The profiler groups **CProfilerGroup** class contains **PROF_GROUP_MAX** profiler points. Every profiler point contains is associated with its FIFO, which keeps result of the profiler point measurement.
      * The profiler measurement is defined in the structure **ProfileData**. The following data are measured:
        * uint64_t max_cnt_;        -Maximal value during the measurement perios
	      * uint64_t max_cnt_time_;   -Time when the maximal measurement happen.
	      * uint64_t last_cnt_;       -The last time measurement. If the measurement was not finished the value will be set  to default.
	      * uint64_t average_cnt_;    -Average value during te measurement period.
	      * uint64_t meas_num_;       -Number measurements
      * Traces group ##CTraceGroup## keepstrace buffer of size **TRACE_ENTRIES_NUM**. Also it contains **MAX_TRACE_FORMATS** trace formats.
      * The event counter group class ##CRT_counter_grp## contains **NUM_COUNTERS** event counter objects of type **RT_counter**.

# Integration with user's code.
## Use case example
The project t_debug/example_c contain an example of integration of the C project with the RT debug library. This project include examples of usaage of all tools from the RT debugging tool library. This example was tested in Linux whith gcc4.5.8 installed. The more advanced GCC versions also should work.
### Use case compile and run
* Compilation
  * cd rt_debug_lib/rt_debug
  * ./build.sh
* Running
  * cd example_c/bin
  * rm /tmp/rt_debug.dat  -- Remove the file, containing collected debugging data from the previos run.
  *  ./rt_lib_example -a  -- Running application example, which use RT debuggings tool and save the debugging information in the /tmp/rt_debug.dat file
  *  ./rt_lib_example -e  -- Extraction of the debugging information and save it in the /tmp/rt_debug.txt file
  *  Read the context of the /tmp/rt_debug.txt file  

## General integration steps:
### Definition of RT library features configuration
The features should be configured using special MACROs. All definitions are put into the specific files in the **rt_debug_lib/rt_debug/cust_api** directory. The following files are used for configuration:
* groups.inc -- this file is used for definition of the debugging groups. The MACRO **DEF_GROUP** is used. Example groups definition:
```
DEF_GROUP(INIT_GROUP)
DEF_GROUP(GEN_GROUP)
```
* profiles.inc -- Definition of profiler points. Example
```
DEF_PROFILE(GEN_GROUP, TST_PROF)
DEF_PROFILE(GEN_GROUP, UI_PROF)
```
* events.inc -- Definition of event counters. Example:
```
DEF_EVENT(STATUS_GROUP,EV_STATUS)
DEF_EVENT(STATUS_GROUP,EV_WATCHDOG)
* traces.inc -- Definition of the tracepoint and its format:
```
DEF_TRACE(INIT_GROUP, CREATE_THREAD, "New thread was created num= %x thread_id = 0x%X")
DEF_TRACE(GEN_GROUP, PASSED, "Passed val= 0x%X")
```

## Add the RT debug API functions to the user's code.


# TDDO
