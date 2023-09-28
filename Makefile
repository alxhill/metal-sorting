# Copyright 2020-2021 Apple Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

OBJECTS=src/main.o

METAL_FILES := $(wildcard src/metal/*.metal)


ifdef DEBUG
DBG_OPT_FLAGS=-g
else
DBG_OPT_FLAGS=-O2
endif

ifdef ASAN
ASAN_FLAGS=-fsanitize=address
else
ASAN_FLAGS=
endif

CC=clang++
CFLAGS=-Wall -std=c++17 -I./metal-cpp -I./metal-cpp-extensions -fno-objc-arc $(DBG_OPT_FLAGS) $(ASAN_FLAGS)
LDFLAGS=-framework Metal -framework Foundation -framework Cocoa -framework CoreGraphics -framework MetalKit 

VPATH=./metal-cpp

%.o: %.cpp
	$(CC) -c $(CFLAGS) $< -o $@

all: build/main

.PHONY: all

build/shaders.metallib: $(METAL_FILES)
	xcrun -sdk macosx metal -frecord-sources=flat -o $@ $(METAL_FILES)

build/main: $(OBJECTS) build/shaders.metallib Makefile
	$(CC) $(CFLAGS) $(LDFLAGS) $(ALX_OBJECTS) -o $@

### For future reference:
#build/10-frame-debugging: $(APP_10FRAMEDEBUGGING_OBJECTS) Makefile
# project-less capture requires an Info.plist. Here it is embedded directly into the binary
# $(CC) $(CFLAGS) $(LDFLAGS) -sectcreate __TEXT __info_plist ./learn-metal/10-frame-debugging/Info.plist $(APP_10FRAMEDEBUGGING_OBJECTS) -o $@

clean:
	rm -f 
		$(ALX_OBJECTS) \
		build/*
