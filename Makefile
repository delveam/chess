EXC := chess
CC := g++
FLAGS := -std=c++17 -Wall -Wextra -Wpedantic
DEBUG_FLAGS := -g -pg -O0
RELEASE_FLAGS := -O3
INCLUDE_FLAGS := -Ibuild/deps/allegro5/include
LIBRARY_FLAGS := -lm -Lbuild/deps/allegro5/lib -lallegro -lallegro_primitives -lallegro_font -lallegro_audio -lallegro_ttf -lallegro_image -lallegro_acodec -lallegro_color -Wl,-rpath,./deps/allegro5

DAM_SOURCES := $(shell find src/dam -name "*.cpp")
DAM_HEADERS := $(shell find src/dam -name "*.hpp")
DAM_DEBUG_OBJECTS := $(patsubst src/dam/%.cpp, build/debug/dam/%.o, $(DAM_SOURCES))
DAM_RELEASE_OBJECTS := $(patsubst src/dam/%.cpp, build/release/dam/%.o, $(DAM_SOURCES))

CHESS_SOURCES := $(shell find src/chess -name "*.cpp")
CHESS_HEADERS := $(shell find src/chess -name "*.hpp")
CHESS_DEBUG_OBJECTS := $(patsubst src/chess/%.cpp, build/debug/chess/%.o, $(CHESS_SOURCES))
CHESS_RELEASE_OBJECTS := $(patsubst src/chess/%.cpp, build/release/chess/%.o, $(CHESS_SOURCES))

DEBUG_OBJECTS := $(DAM_DEBUG_OBJECTS) $(CHESS_DEBUG_OBJECTS)
RELEASE_OBJECTS := $(DAM_RELEASE_OBJECTS) $(CHESS_RELEASE_OBJECTS)

CONTENT_SOURCES := $(shell find content)

TERM_GREEN := \033[0;32m
TERM_BLUE := \033[0;34m
TERM_PURPLE := \033[0;35m
TERM_YELLOW := \033[0;33m
TERM_NC := \033[0m

.PHONY: all clean debug dev format release

$(VERBOSE).SILENT:

all: clean release

build:
	mkdir $@

build/debug: | build
	mkdir $@

build/debug/dam: | build/debug
	mkdir $@

build/debug/chess: | build/debug
	mkdir $@

build/release: | build
	mkdir $@

build/release/dam: | build/release
	mkdir $@

build/release/chess: | build/release
	mkdir $@

bin:
	mkdir $@

bin/debug: | bin
	mkdir $@

bin/release: | bin
	mkdir $@

$(DAM_DEBUG_OBJECTS): build/debug/dam/%.o: src/dam/%.cpp $(DAM_HEADERS) build/deps/allegro5/include/allegro5/allegro5.h | build/debug/dam
	@echo -e "$(TERM_GREEN)Building$(TERM_NC) $(TERM_BLUE)$@$(TERM_NC)"
	$(CC) $(INCLUDE_FLAGS) $(FLAGS) $(DEBUG_FLAGS) -o $@ -c $<

$(CHESS_DEBUG_OBJECTS): build/debug/chess/%.o: src/chess/%.cpp $(DAM_HEADERS) $(CHESS_HEADERS) build/deps/allegro5/include/allegro5/allegro5.h | build/debug/chess
	@echo -e "$(TERM_GREEN)Building$(TERM_NC) $(TERM_BLUE)$@$(TERM_NC)"
	$(CC) $(INCLUDE_FLAGS) $(FLAGS) $(DEBUG_FLAGS) -o $@ -c $<

build/debug/chess/$(EXC): $(DEBUG_OBJECTS) build/deps/allegro5/lib/liballegro.so | build/debug/chess
	@echo -e "$(TERM_YELLOW)Creating$(TERM_NC) $(TERM_PURPLE)$(EXC) (Debug)$(TERM_NC) $(TERM_YELLOW)executable$(TERM_NC)"
	$(CC) $(FLAGS) $(DEBUG_FLAGS) -o $@ $(DEBUG_OBJECTS) $(LIBRARY_FLAGS)

bin/debug/$(EXC): build/debug/chess/$(EXC) $(CONTENT_SOURCES) | bin/debug
	@echo -e "$(TERM_YELLOW)Packaging$(TERM_NC) $(TERM_PURPLE)$(EXC) (Debug)$(TERM_NC)"
	rm -rf bin/debug/*
	cp -r content bin/debug
	cp $< bin/debug
	mkdir bin/debug/deps
	mkdir bin/debug/deps/allegro5
	find build/deps/allegro5/lib -name "liballegro*" -exec cp {} bin/debug/deps/allegro5 \;
	find bin/debug/content -maxdepth 1 -type f -exec mv {} bin/debug \;

debug: bin/debug/$(EXC)
	@echo Done

$(DAM_RELEASE_OBJECTS): build/release/dam/%.o: src/dam/%.cpp $(DAM_HEADERS) build/deps/allegro5/include/allegro5/allegro5.h | build/release/dam
	@echo -e "$(TERM_GREEN)Building$(TERM_NC) $(TERM_BLUE)$@$(TERM_NC)"
	$(CC) $(INCLUDE_FLAGS) $(FLAGS) $(RELEASE_FLAGS) -o $@ -c $<

$(CHESS_RELEASE_OBJECTS): build/release/chess/%.o: src/chess/%.cpp $(DAM_HEADERS) $(CHESS_HEADERS) build/deps/allegro5/include/allegro5/allegro5.h | build/release/chess
	@echo -e "$(TERM_GREEN)Building$(TERM_NC) $(TERM_BLUE)$@$(TERM_NC)"
	$(CC) $(INCLUDE_FLAGS) $(FLAGS) $(RELEASE_FLAGS) -o $@ -c $<

build/release/chess/$(EXC): $(RELEASE_OBJECTS) build/deps/allegro5/lib/liballegro.so | build/release/chess
	@echo -e "$(TERM_YELLOW)Creating$(TERM_NC) $(TERM_PURPLE)$(EXC) (Release)$(TERM_NC) $(TERM_YELLOW)executable$(TERM_NC)"
	$(CC) $(FLAGS) $(RELEASE_FLAGS) -o $@ $(RELEASE_OBJECTS) $(LIBRARY_FLAGS)

bin/release/$(EXC): build/release/chess/$(EXC) $(CONTENT_SOURCES) | bin/release
	@echo -e "$(TERM_YELLOW)Packaging$(TERM_NC) $(TERM_PURPLE)$(EXC) (Release)$(TERM_NC)"
	rm -rf bin/release/*
	cp -r content bin/release
	cp $< bin/release
	mkdir bin/release/deps
	mkdir bin/release/deps/allegro5
	find build/deps/allegro5/lib -name "liballegro*" -exec cp {} bin/release/deps/allegro5 \;
	find bin/release/content -maxdepth 1 -type f -exec mv {} bin/release \;

release: bin/release/$(EXC)
	@echo Done

dev: bin/debug/$(EXC)
	cd bin/debug; ./$(EXC)
	gprof bin/debug/chess bin/debug/gmon.out > bin/debug/profile

format:
	astyle --style=stroustrup -n --recursive src/*.cpp,*.hpp

clean:
	if [ -d "./build" ]; then rm -rf build; fi
	if [ -d "./bin" ]; then rm -rf bin; fi
	@echo Done

deps:
	mkdir $@

deps/allegro5: | deps
	cd deps; git clone https://github.com/liballeg/allegro5

deps/allegro5/build: | deps/allegro5
	mkdir $@

deps/allegro5/build/Makefile: | deps/allegro5/build
	cd deps/allegro5/build; cmake ..

deps/allegro5/build/lib/liballegro.so: deps/allegro5/build/Makefile
	cd deps/allegro5/build; make

deps/allegro5/build/local_install: | deps/allegro5/build
	mkdir $@

deps/allegro5/build/local_install/usr: deps/allegro5/build/lib/liballegro.so | deps/allegro5/build/local_install
	cd deps/allegro5/build; make install DESTDIR=local_install

build/deps: | build
	mkdir $@

build/deps/allegro5: | build/deps
	mkdir $@

build/deps/allegro5/include/allegro5/allegro5.h: deps/allegro5/build/local_install/usr | build/deps/allegro5
	cp -r deps/allegro5/build/local_install/usr/local/include build/deps/allegro5

build/deps/allegro5/lib/liballegro.so: deps/allegro5/build/local_install/usr | build/deps/allegro5
	cp -r deps/allegro5/build/local_install/usr/local/lib build/deps/allegro5
