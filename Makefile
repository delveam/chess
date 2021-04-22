EXC = chess
CC = g++
FLAGS = -std=c++17 -Wall
DEBUG_FLAGS = -g -O0
RELEASE_FLAGS = -O3
LIBRARIES = -lm $(shell pkg-config --cflags --libs allegro-5 allegro_primitives-5 allegro_font-5 allegro_audio-5 allegro_ttf-5 allegro_image-5 allegro_acodec-5 allegro_color-5)

DEBUG_OBJECTS = $(shell find src/chess src/dam -name "*.cpp" | sed "s/^src/build\/debug/g;s/.cpp$//.o/g")
RELEASE_OBJECTS = $(shell find src/chess src/dam -name "*.cpp" | sed "s/^src/build\/release/g;s/.cpp$//.o/g")

.PHONY: all clean debug dev format release dam_debug chess_debug

export CC FLAGS DEBUG_FLAGS RELEASE_FLAGS

all: clean release

build:
	mkdir $@

build/debug: | build
	mkdir $@

build/debug/dam: | build/debug
	mkdir $@

build/debug/chess : | build/debug
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

dam_debug: | build/debug/dam
	make -f ./src/dam/dam.mk debug

chess_debug: dam_debug | build/debug/chess
	make -f ./src/chess/chess.mk debug

dam_release: | build/release/dam
	make -f ./src/dam/dam.mk release

chess_release: dam_release | build/release/chess
	make -f ./src/chess/chess.mk release

build/debug/chess/$(EXC): chess_debug | build/debug/chess
	$(CC) $(FLAGS) $(DEBUG_FLAGS) -o $@ $(DEBUG_OBJECTS) $(LIBRARIES)

debug: build/debug/chess/$(EXC) | bin/debug
	rm -rf bin/debug/*
	cp -r ./content ./bin/debug
	cp $< ./bin/debug

build/release/chess/$(EXC): dam_release chess_release | build/release/chess
	$(CC) $(FLAGS) $(RELEASE_FLAGS) -o $@ $(RELEASE_OBJECTS) $(LIBRARIES)

release: build/release/chess/$(EXC) | bin/release
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
