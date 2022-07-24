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

BUILD_PATH ?= build

COMMON = ./common
VM = ./vm
CLIB = ./clib
TEST = ./test

OBJECTS = \
	  main.o \
	  luaaux.o \
	  luastate.o \
	  luaobject.o \
	  luamem.o \
	  luado.o \
	  luagc.o \
	  luastring.o \
	  p1_test.o \
	  p2_test.o \
	  p3_test.o \

all : \
	$(BUILD_PATH)/tinylua

$(BUILD_PATH)/tinylua: $(foreach v, $(OBJECTS), $(BUILD_PATH)/$(v)) | $(BUILD_PATH)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_PATH)/main.o: main.c $(CLIB)/luaaux.h $(TEST)/p1_test.h $(TEST)/p2_test.h $(TEST)/p3_test.h | $(BUILD_PATH)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_PATH)/luaaux.o: $(CLIB)/luaaux.c $(CLIB)/luaaux.h $(COMMON)/luastate.h $(VM)/luado.h | $(BUILD_PATH)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_PATH)/luastate.o: $(COMMON)/luastate.c $(COMMON)/luastate.h $(COMMON)/luamem.h  $(COMMON)/luaobject.h $(COMMON)/luastring.h | $(BUILD_PATH)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_PATH)/luaobject.o: $(COMMON)/luaobject.c $(COMMON)/luaobject.h $(COMMON)/lua.h | $(BUILD_PATH)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_PATH)/luamem.o: $(COMMON)/luamem.c $(COMMON)/luastate.h $(VM)/luado.h | $(BUILD_PATH)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_PATH)/luado.o: $(VM)/luado.c $(COMMON)/luastate.h $(COMMON)/luamem.h | $(BUILD_PATH)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_PATH)/luagc.o: $(VM)/luagc.c $(COMMON)/luastate.h $(COMMON)/luamem.h
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_PATH)/luastring.o: $(COMMON)/luastring.c $(COMMON)/luastate.h $(VM)/luagc.h $(COMMON)/luamem.h
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_PATH)/p1_test.o: $(TEST)/p1_test.c $(CLIB)/luaaux.h | $(BUILD_PATH)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_PATH)/p2_test.o: $(TEST)/p2_test.c $(CLIB)/luaaux.h $(VM)/luagc.h | $(BUILD_PATH)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_PATH)/p3_test.o: $(TEST)/p3_test.c $(CLIB)/luaaux.h $(VM)/luagc.h | $(BUILD_PATH)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_PATH) :
	mkdir $@

clean:
	rm $(BUILD_PATH)/*
