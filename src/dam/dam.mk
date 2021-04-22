LIB = dam

SOURCES = $(shell find src/$(LIB) -name "*.cpp")
HEADERS = $(shell find src/$(LIB) -name "*.hpp")
DEBUG_OBJECTS = $(patsubst src/$(LIB)/%.cpp, build/debug/$(LIB)/%.o, $(SOURCES))
RELEASE_OBJECTS = $(patsubst src/$(LIB)/%.cpp, build/release/$(LIB)/%.o, $(SOURCES))

.PHONY: all debug release

all: release

build/debug/$(LIB)/%.o: src/$(LIB)/%.cpp $(HEADERS)
	$(CC) $(FLAGS) $(DEBUG_FLAGS) -o $@ -c $<

debug: $(DEBUG_OBJECTS)

build/release/$(LIB)/%.o: src/$(LIB)/%.cpp $(HEADERS)
	$(CC) $(FLAGS) $(RELEASE_FLAGS) -o $@ -c $<

release: $(RELEASE_OBJECTS)
