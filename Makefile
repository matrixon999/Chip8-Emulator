CC = g++
CFLAGS = -std=c++20 -I./raylib/include
LDFLAGS = -L./raylib/lib -lraylib -lglfw -lGL -lm -lpthread -ldl -lrt -lX11 -lfmt -Draylib_USE_STATIC_LIBS

SRCDIR = src
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = chip-8-emu

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)