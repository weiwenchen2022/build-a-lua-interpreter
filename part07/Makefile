CC = gcc
CFLAGS = -O2 \
	 -g \
	 -pedantic \
	 -Wall \
	 -W \
	 -Wmissing-prototypes \
	 -Wno-sign-compare \
	 -Wimplicit-fallthrough \
	 -Wno-unused-parameter \

COMMON = ./common
VM = ./vm
COMPILER = ./compiler
CLIB = ./clib
TEST = ./test

CLIB_SRC := \
	luaaux.c \

COMMON_SRC := \
	luabase.c \
	luainit.c \
	luamem.c \
	luaobject.c \
	luastate.c \
	luastring.c \
	luatable.c \

COMPILER_SRC := \
	luacode.c \
	lualexer.c \
	luaparser.c \
	luazio.c \

VM_SRC := \
	luado.c \
	luafunc.c \
	luagc.c \
	luaopcodes.c \
	luavm.c \

TEST_SRC := \
	p*_test.c \

all : tinylua

tinylua : main.c \
	$(foreach v, $(CLIB_SRC), $(CLIB)/$(v)) \
	$(foreach v, $(COMMON_SRC), $(COMMON)/$(v)) \
	$(foreach v, $(COMPILER_SRC), $(COMPILER)/$(v)) \
	$(foreach v, $(VM_SRC), $(VM)/$(v)) \
	$(foreach v, $(TEST_SRC), $(TEST)/$(v))
	$(CC) $(CFLAGS) -I$(CLIB) -I$(COMMON) -I$(VM) -I$(TEST) -o $@ $^ -lm

clean:
	rm tinylua
