#!/bin/bash
cd rt_debug_cpp/
make -f Makefile.static clean
make -f Makefile.static
cd ../rt_debug_c
make -f Makefile.static clean
make -f Makefile.static
cd ../example_c
make -f Makefile.exec clean
make -f Makefile.exec
cd ..