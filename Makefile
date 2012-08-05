CFLAGS=-Wall -lSDL -lSDL_ttf -lSDL_mixer

all:
	g++ main.cpp snake.cpp -o snake.o $(CFLAGS)

clean:
	rm -rf *.o

