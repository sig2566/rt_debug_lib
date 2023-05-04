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
* Debugging groups. There are number gebugging groups. Each debugging group contains Logging buffer, 
