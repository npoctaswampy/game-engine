CFLAGS=-W -Wall -ggdb `sdl2-config --cflags`
LIBS=-lm `sdl2-config --libs` -lpthread
OBJS=game_main.o particle.o background.o resources.o
CC=gcc

BIN = maker game game_main.o background.o character.o char_make.o room.o tile.o collision.o stack.o room_change.o vampire.o ghost.o item.o clean

all: $(BIN)

game: game_main.o background.o character.o collision.o room.o stack.o tile.o collision.o room_change.o vampire.o ghost.o item.o
	$(CC) $(CFLAGS) game_main.o background.o character.o room.o tile.o collision.o stack.o room_change.o vampire.o ghost.o item.o -o game $(LIBS)

game_main.o: game_main.c background.h character.h room.h tile.h collision.h stack.h room_change.h vampire.h ghost.h
	$(CC) $(CFLAGS) -c game_main.c

maker: maker_main.o background.o char_make.o collision.o room.o stack.o tile.o collision.o vampire.o ghost.o item.o
	$(CC) $(CFLAGS) maker_main.o background.o char_make.o room.o tile.o collision.o stack.o vampire.o ghost.o item.o -o maker $(LIBS)

maker_main.o: maker_main.c background.h char_make.h room.h tile.h collision.h stack.h
	$(CC) $(CFLAGS) -c maker_main.c

vampire.o: vampire.c vampire.h
	$(CC) $(CFLAGS) -c vampire.c 

ghost.o: ghost.c ghost.h
	$(CC) $(CFLAGS) -c ghost.c 

background.o: background.c background.h
	$(CC) $(CFLAGS) -c background.c 

character.o:  character.c character.h
	$(CC) $(CFLAGS) -c character.c

char_make.o:  char_make.c char_make.h
	$(CC) $(CFLAGS) -c char_make.c

room.o: room.c room.h stack.h
	$(CC) $(CFLAGS) -c room.c

tile.o:  tile.c tile.h
	$(CC) $(CFLAGS) -c tile.c

collision.o: collision.c collision.h 
	$(CC) $(CFLAGS) -c collision.c

stack.o: stack.c stack.h
	$(CC) $(CFLAGS) -c stack.c

room_change.o: room_change.c room_change.h
	$(CC) $(CFLAGS) -c room_change.c

item.o: item.c item.h
	$(CC) $(CFLAGS) -c item.c
	

clean:
	rm -f *.o  

