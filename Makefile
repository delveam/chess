FLAGS = `pkg-config --cflags --libs allegro-5 allegro_primitives-5 allegro_font-5 allegro_audio-5 allegro_ttf-5 allegro_image-5 allegro_acodec-5 allegro_color-5` -lm
EXC = chess

.PHONY: all clean debug dev

all: clean debug

build:
	mkdir $@

bin:
	mkdir $@

bin/debug: | bin
	mkdir $@

build/main.o: src/main.cpp | build
	gcc -c $< -o $@

bin/debug/$(EXC): build/main.o | bin/debug
	gcc $^ $(FLAGS) -o $@

debug: bin/debug/$(EXC)

dev: debug
	cd ./bin/debug; ./$(EXC)

clean:
	if [ -d "./build" ]; then rm -rf build; fi
	if [ -d "./bin" ]; then rm -rf bin; fi
