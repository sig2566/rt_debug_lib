#*****************************************************************
#*
#* File: build.mk 
#*
#*****************************************************************

SHELL	:= /bin/bash
OSTYPE	:= $(shell uname)


#Configuration of build parameters depending of DU or RU component
#Common headers
#SRC_H   := $(SRC_H) $(shell find $(ROOT)/Sys_API -type f -name "*.h")
SRC_INC :=  $(shell find $(H_INC_DIR) -type f -name "*.inc")

#Set hard coded build environment if it was not set before
CC1 :=$(CC)
ifeq ($(CC),)
	CC=gcc 
	CPP=g++ 
endif
#ARCH_DEF = -DARCH=X86
OBJ := obj
BIN := bin

#BUILD_TIME=$(shell date "+DATE:%d/%m/%y_TIME:%H:%M:%S")

CC2 :=$(CC)
#VERDEF = -DTIME_DATE=\"$(BUILD_TIME)\" -DMOD_NAME=\"$(NAME)\" 

#release: CFLAGS=-O3 -g -DDEBUG $(CFLAGS_ADD)
ifeq (release, $(mode))
DEBFLAGS:=-O3 -g -DDEBUG 
else
DEBFLAGS :=-g -O0 -DDEBUG 
endif
# All Target
all: CFLAGS:= $(CFLAGS_ADD) $(DEBFLAGS)
all: lib


SRC_C   := $(shell find . -type f -name "*.c")
SRC_CPP := $(shell find . -type f -name "*.cc")
SRC_H_MODULE   := $(shell find . -type f -name "*.h")

SRC_H := $(SRC_H) $(SRC_H_MODULE)



OBJECTS_CPP := $(notdir $(SRC_CPP:.cc=.o))
OBJECTS_C := $(notdir $(patsubst %.c,%.o,$(SRC_C)))

H_INC_DIR := $(H_INC_DIR) $(ROOT)/api

#H_INCLUDE = $(INC_FLAGS) $(patsubst %, -I%, $(H_DIR)) -I$(SYS_API)  -I$(MOD_API) $(OPT_H)

ifeq ($(H_INC_DIR),)
 H_INC_DIR := $(ROOT)/cust_api
endif
H_INCLUDE = $(OPT_H) $(INC_FLAGS) $(patsubst %, -I%, $(H_INC_DIR))
LIB_TYPE?=DYNAMIC
ifeq (STATIC, $(LIB_TYPE))
BUILD_TARGET:=
LINK_TOOL:=$(AR) -rc $(BIN)/$(NAME).a
else
LINK_TOOL:= $(CPP) -shared -Wl,--export-dynamic
BUILD_TARGET:=-o $(BIN)/$(NAME).so
endif
CC3 :=$(CC)

lib : $(OBJECTS_CPP) $(OBJECTS_C)
	@echo $(CPATH)
	@echo $(OBJECTS_CPP) $(OBJECTS_C)
	@test -d $(BIN) || mkdir -p $(BIN) 
	$(LINK_TOOL) $(OBJECTS_CPP) $(OBJECTS_C) $(LIB_SO)  $(BUILD_TARGET)
 
$(OBJECTS_CPP): $(SRC_CPP) $(SRC_H) $(SRC_INC)
	@echo $(SRC_CPP) $(H_INCLUDE)
	@echo $(OBJECTS_CPP)
	@echo "CPP $(CPP)"
	@echo "CC1 $(CC1)"
	@echo "CC2 $(CC2)"
	@echo "CC3 $(CC3)"		
	@test -d $(OBJ) || mkdir -p $(OBJ) 
	$(CC) $(H_INCLUDE) $(VERDEF) $(ARCH_DEF) $(CFLAGS)  -fPIC -c $(SRC_CPP)

$(OBJECTS_C): $(SRC_C) $(SRC_H) $(SRC_INC)
	@echo $(SRC_C) $(SRC_H)
	@echo $(OBJECTS_C)
	@test -d $(OBJ) || mkdir -p $(OBJ) 
	$(CC)  $(H_INCLUDE) $(VERDEF) $(ARCH_DEF) $(CFLAGS)  -fPIC -c $(SRC_C)

.PHONY: clean
clean:
	rm *o || true
	rm $(BIN)/$(NAME).so || true
	rm $(BIN)/$(NAME).a || true
