CFLAGS=-Wall -lSDL -lSDL_ttf

all:
	g++ main.cpp snake.cpp -o snake.o $(CFLAGS)

clean:
	rm -rf *.o

