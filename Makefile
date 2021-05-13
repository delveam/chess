EXC := chess
CC := g++
FLAGS := -std=c++17 -Wall
DEBUG_FLAGS := -g -O0
RELEASE_FLAGS := -O3
LIBRARIES := -lm $(shell pkg-config --cflags --libs allegro-5 allegro_primitives-5 allegro_font-5 allegro_audio-5 allegro_ttf-5 allegro_image-5 allegro_acodec-5 allegro_color-5)

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

.PHONY: all clean debug dev format release

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

$(DAM_DEBUG_OBJECTS): build/debug/dam/%.o: src/dam/%.cpp $(DAM_HEADERS) | build/debug/dam
	$(CC) $(FLAGS) $(DEBUG_FLAGS) -o $@ -c $<

$(CHESS_DEBUG_OBJECTS): build/debug/chess/%.o: src/chess/%.cpp $(CHESS_HEADERS) | build/debug/chess
	$(CC) $(FLAGS) $(DEBUG_FLAGS) -o $@ -c $<

build/debug/chess/$(EXC): $(DEBUG_OBJECTS) | build/debug/chess
	$(CC) $(FLAGS) $(DEBUG_FLAGS) -o $@ $^ $(LIBRARIES)

debug: build/debug/chess/$(EXC) | bin/debug
	rm -rf bin/debug/*
	cp -r ./content ./bin/debug
	mv ./bin/debug/content/config.cfg ./bin/debug
	cp $< ./bin/debug

$(DAM_RELEASE_OBJECTS): build/release/dam/%.o: src/dam/%.cpp $(DAM_HEADERS) | build/release/dam
	$(CC) $(FLAGS) $(RELEASE_FLAGS) -o $@ -c $<

$(CHESS_RELEASE_OBJECTS): build/release/chess/%.o: src/chess/%.cpp $(CHESS_HEADERS) | build/release/chess
	$(CC) $(FLAGS) $(RELEASE_FLAGS) -o $@ -c $<

build/release/chess/$(EXC): $(RELEASE_OBJECTS) | build/release/chess
	$(CC) $(FLAGS) $(RELEASE_FLAGS) -o $@ $^ $(LIBRARIES)

release: build/release/chess/$(EXC) | bin/release
	rm -rf bin/release/*
	cp -r ./content ./bin/release
	mv ./bin/release/content/config.cfg ./bin/release
	cp $< ./bin/release

dev: debug
	cd ./bin/debug; ./$(EXC)

format: $(shell find src)
	astyle --style=stroustrup -n --recursive ./src/*.cpp,*.hpp

clean:
	if [ -d "./build" ]; then rm -rf build; fi
	if [ -d "./bin" ]; then rm -rf bin; fi
