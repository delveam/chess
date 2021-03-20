.PHONY: all clean debug dev

all: clean debug

build:
	mkdir $@

build/Makefile: | build
	cd build; cmake ..

bin:
	mkdir $@

bin/debug: | bin
	mkdir $@

debug: build/Makefile | bin/debug
	cd build; make && mv chess ../bin/debug

dev: debug
	cd ./bin/debug; ./chess

clean:
	if [ -d "./build" ]; then rm -rf build; fi
	if [ -d "./bin" ]; then rm -rf bin; fi
