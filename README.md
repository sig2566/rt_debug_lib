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

# RT debugging extracted data format.
The following information is presented after extraction of the collected RT debugging data:
* Event counters value:
```
vent Counters
**************
STATUS_GROUP, EV_STATUS, 2
STATUS_GROUP, EV_WATCHDOG, 99618
*************
```
* Profiler point information:
```
Profilers
*************
GEN_GROUP, UI_PROF, avg=, 1085003, max=, 3964256, last=, 1084288, max time=, 108520185534605, measurements=, 9000
**************
```
* Trace information:
```
108520172652619, 0, 0, 0, INIT_GROUP, 55104, 73, New thread was created num= 9 thread_id = 0x21F56700
108528876943931, 0, 0, 0, GEN_GROUP, 34560, 49, Passed val= 0x1F35

Logs:
```
108520172410519, 0, 0, 0, INIT_GROUP,  Start running application 10
108531080142880, 0, 0, 0, GEN_GROUP,  Stop application
```
```
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
```
* traces.inc -- Definition of the tracepoint and its format:
```
DEF_TRACE(INIT_GROUP, CREATE_THREAD, "New thread was created num= %x thread_id = 0x%X")
DEF_TRACE(GEN_GROUP, PASSED, "Passed val= 0x%X")
```

## Add the RT debug API functions to the user's code.
There are API functions and MACROs in the **rt_debug_lib/rt_debug/api/rt_debug_adap_api.h** file.There are the main API functions below
* RT_debug_init(rt_debug_data);  -- Activation the collection of the RT debg data into the rt_debug_data file.
* RT_debug_start  -- Start collection RT debug information
* RT_debug_stop  -- stop the collection debugging information
* RTDBG_SAVE_LOG(group name, printf like fromatted line)   - MACRO to collect the logging per group.
* RTDBG_SAVE_TRACE(INIT_GROUP, CREATE_THREAD, i, (uint32_t)tid[i], 0, 0); --MACRO to save the trace information.
* RTDBG_GET_EVENT_CNTR_PTR  -- get pointer to some event counter. Example: uint64_t *ex_status= RTDBG_GET_EVENT_CNTR_PTR(STATUS_GROUP,EV_STATUS);
* Support the profiler points. Since it is possible, that the same profiled code runs in parallel by different threads, the profiling should use the locally allocated data to be independed from the multithreading. See example in the **rt_lib_example** code. There are support from in the case of multi-threading:
   * ProfilePoint prof_task;  -- definition of the profiled in the local memory
   * RTDBG_INIT_PROF(GEN_GROUP, UI_PROF, &prof_task, 500); -- Initialization current instance of the profiler point. Note, the last parameter is update counter. It define how many time to measure the profiler point before the result is saved in FIFO. 
   * RTDBG_START_PROF -- start pprofiler point measurement
   * RTDBG_STOP_PROF -- Stop profiler measurement.
   * RTDBG_FLUSH_DATA_PROF -- Stop with flush all collected information into the FIFO.
* RT_debug_ExtractDebugData(output file name) -- extract profiler data in the text format into the file

## Modification of the application build script
The following changes should be added to the application build script to support interation with RT debug library:
* Add RT debugger API pathes to the compiler command line:  **H_INC_DIR:= -I../api/ -I../rt_debug_cpp/api -I../../rt_debug/cust_api**
* Add linker command to link with RT debugger libraries: **LIB= -L ../rt_debug_c/bin -l  rtdebugadap -L ../rt_debug_cpp/bin -l rtdebug  -lpthread**
* Ass support for multithreading with the following compailer flag: **-pthread**

# TODO
The following issues should be added in future to the rt debugger library:
* Add support to the monitoring of execution of he application.
* Improve gettime speed. It is possible to use the core clock couner
* Use huge pages for RT shared memory to avoid performane degradation due to the page swapping.
* Need to add interface function, supporting getting logs and profiler measurement in run time
