#!/bin/bash
cd rt_debug_cpp/
make -f Makefile clean
make -f Makefile
cd ../rt_debug_c
make -f Makefile.static clean
make -f Makefile.static
cd ../example_c
make -f Makefile.exec clean
make -f Makefile.exec
cd ..
echo "cp rt_debug_cpp/bin/librtdebug.so ../../app_framework/bin/."
cp ./rt_debug_cpp/bin/librtdebug.so ../../app_framework/bin/.


