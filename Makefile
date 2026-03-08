UNAME_S := $(shell uname -s)

CXX = clang++
CXXFLAGS = -std=c++11 -Wall -fPIC

ifeq ($(UNAME_S),Darwin)
    EXT = dylib
    LDFLAGS = -dynamiclib
    LDFLAGS_A = $(LDFLAGS)
    LDFLAGS_B = $(LDFLAGS)
else
    EXT = so
    LDFLAGS = -shared
    LDFLAGS_A = $(LDFLAGS) -Wl,--version-script=libA.ver
    LDFLAGS_B = $(LDFLAGS) -Wl,--version-script=libB.ver
endif

.PHONY: all clean run run-poison

all: libA.$(EXT) libB.$(EXT) main

libA.$(EXT): libA.cpp proto.h
	$(CXX) $(CXXFLAGS) $(LDFLAGS_A) -o $@ $<

libB.$(EXT): libB.cpp proto.h
	$(CXX) $(CXXFLAGS) $(LDFLAGS_B) -o $@ $<

main: main.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

run: all
	./main

run-poison: all
	./main --poison

clean:
	rm -f libA.$(EXT) libB.$(EXT) main main_static main_static.cpp repro.py main_static.o static_lib_v1.o static_lib_v2.o libstatic1.a libstatic2.a libA_static.dylib libB_static.dylib
