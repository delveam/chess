EXC = chess
CC = g++
FLAGS = -std=c++17 -Wall
DEBUG_FLAGS = -g -O0
RELEASE_FLAGS = -O3
LIBRARIES = -lm $(shell pkg-config --cflags --libs allegro-5 allegro_primitives-5 allegro_font-5 allegro_audio-5 allegro_ttf-5 allegro_image-5 allegro_acodec-5 allegro_color-5)

DAM_HEADERS = $(shell find ./src/dam -name "*.hpp")
CHESS_HEADERS = $(shell find ./src -name "*.hpp")
SOURCES = $(shell cd ./src; find . ./dam -name "*.cpp")
DEBUG_OBJECTS = $(patsubst ./%.cpp, ./build/debug/%.o, $(SOURCES))
RELEASE_OBJECTS = $(patsubst ./%.cpp, ./build/release/%.o, $(SOURCES))

.PHONY: all clean debug dev format release

export CC FLAGS DEBUG_FLAGS

all: clean release

build:
	mkdir $@

build/debug: | build
	mkdir $@

build/debug/dam: | build/debug
	mkdir $@

build/release: | build
	mkdir $@

build/release/dam: | build/release
	mkdir $@

bin:
	mkdir $@

bin/debug: | bin
	mkdir $@

bin/release: | bin
	mkdir $@

build/debug/dam/%.o: src/dam/%.cpp $(DAM_HEADERS) | build/debug/dam
	$(CC) $(FLAGS) $(DEBUG_FLAGS) -o $@ -c $<

build/debug/%.o: src/%.cpp $(CHESS_HEADERS) | build/debug
	$(CC) $(FLAGS) $(DEBUG_FLAGS) -o $@ -c $<

build/debug/$(EXC): $(DEBUG_OBJECTS) | build/debug
	$(CC) $(FLAGS) $(DEBUG_FLAGS) -o $@ $^ $(LIBRARIES)

debug: build/debug/$(EXC) | bin/debug
	rm -rf bin/debug/*
	cp -r ./content ./bin/debug
	cp $< ./bin/debug

build/release/dam/%.o: src/dam/%.cpp $(DAM_HEADERS) | build/release/dam
	$(CC) $(FLAGS) $(RELEASE_FLAGS) -o $@ -c $<

build/release/%.o: src/%.cpp $(CHESS_HEADERS) | build/release
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
