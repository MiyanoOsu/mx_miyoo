TARGET = mx

CROSS_COMPILE =

CC = $(CROSS_COMPILE)gcc

OBJS = font.o config.o input.o video.o menu.o mx.o main.o

VPATH = ./

CFLAGS = -I. -O2 $(shell pkg-config --cflags sdl)

LDFLAGS = $(shell pkg-config --libs sdl SDL_ttf SDL_image) -lstdc++

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@echo Done!!!

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGET)
