EXC = chess
CC = g++

SOURCES = $(wildcard src/*.cpp)
HEADERS = $(shell find src/*.hpp)
OBJECTS = $(patsubst src/%.cpp, build/%.o, $(SOURCES))
LIBRARIES = -lm $(shell pkg-config --cflags --libs allegro-5 allegro_primitives-5 allegro_font-5 allegro_audio-5 allegro_ttf-5 allegro_image-5 allegro_acodec-5 allegro_color-5)

.PHONY: all clean debug dev

all: clean debug

build:
	mkdir $@

bin:
	mkdir $@

bin/debug: | bin
	mkdir $@

build/%.o: src/%.cpp $(HEADERS) | build
	$(CC) -c $< -o $@

build/$(EXC): $(OBJECTS) | build
	$(CC) -o $@ $^ $(LIBRARIES)

debug: build/$(EXC) | bin/debug
	rm -rf bin/debug/*
	cp -r ./content ./bin/debug
	cp ./build/$(EXC) ./bin/debug

dev: debug
	cd ./bin/debug; ./$(EXC)

clean:
	if [ -d "./build" ]; then rm -rf build; fi
	if [ -d "./bin" ]; then rm -rf bin; fi
