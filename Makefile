EXC = chess
CC = g++
FLAGS = -std=c++17
DEBUG_FLAGS = -g -Wall -O0
RELEASE_FLAGS = -Wall -O3

SOURCES = $(wildcard src/*.cpp)
HEADERS = $(shell find src/*.hpp)
DEBUG_OBJECTS = $(patsubst src/%.cpp, build/debug/%.o, $(SOURCES))
RELEASE_OBJECTS = $(patsubst src/%.cpp, build/release/%.o, $(SOURCES))
LIBRARIES = -lm $(shell pkg-config --cflags --libs allegro-5 allegro_primitives-5 allegro_font-5 allegro_audio-5 allegro_ttf-5 allegro_image-5 allegro_acodec-5 allegro_color-5)

.PHONY: all clean debug dev format release

all: clean release

build:
	mkdir $@

build/debug: | build
	mkdir $@

build/release: | build
	mkdir $@

bin:
	mkdir $@

bin/debug: | bin
	mkdir $@

bin/release: | bin
	mkdir $@

build/debug/%.o: src/%.cpp $(HEADERS) | build/debug
	$(CC) $(FLAGS) $(DEBUG_FLAGS) -o $@ -c $<

build/debug/$(EXC): $(DEBUG_OBJECTS) | build/debug
	$(CC) $(FLAGS) $(DEBUG_FLAGS) -o $@ $^ $(LIBRARIES)

debug: build/debug/$(EXC) | bin/debug
	rm -rf bin/debug/*
	cp -r ./content ./bin/debug
	cp $< ./bin/debug

build/release/%.o: src/%.cpp $(HEADERS) | build/release
	$(CC) $(FLAGS) $(RELEASE_FLAGS) -o $@ -c $<

build/release/$(EXC): $(RELEASE_OBJECTS) | build/release
	$(CC) $(FLAGS) $(RELEASE_FLAGS) -o $@ $^ $(LIBRARIES)

release: build/release/$(EXC) | bin/release
	rm -rf bin/release/*
	cp -r ./content ./bin/release
	cp $< ./bin/release

dev: debug
	cd ./bin/debug; ./$(EXC)

format: $(shell find src)
	astyle --style=stroustrup -n --recursive ./src/*.cpp,*.hpp

clean:
	if [ -d "./build" ]; then rm -rf build; fi
	if [ -d "./bin" ]; then rm -rf bin; fi
