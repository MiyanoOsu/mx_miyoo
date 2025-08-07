TARGET = mx

CROSS_COMPILE =

CC = $(CROSS_COMPILE)gcc

OBJS = font.o config.o input.o video.o menu.o mx.o main.o

VPATH = ./

CFLAGS = -I. -O2 `sdl-config --cflags`

LDFLAGS = `sdl-config --libs` -lSDL_ttf -lSDL_image

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@
	@echo Done!!!

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGET)