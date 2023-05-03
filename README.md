# rt_debug_lib
Real Time Debugging Library

## Introduction
This document describes the open source project, called rt_debug_lib. The rt_debug_lib provides debugging tools, intended for debugging real time applications with minimal influence to performance. The tools are:
* Real time tracer. The tracer is set of traceponts, which are added to the code. Once the tracepoint is called during the program execution, some information is collected in the cyclic buffer. When the traces may be extracted when the debugged application is stopped or the trace collection was stopped.There are numbertypes of tracepoints. They may be differ by collected information and its presentation.
