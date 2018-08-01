#define STACK
#define TILE_W 64
#define TILE_H 64

#define ACTION_REMOVE_COLLISION 0

#ifndef ROOM
#define ROOM
#include "room.h"
#endif

#ifndef ROOMCHANGE
#define ROOM
#include "room_change.h"
#endif

typedef struct tile_coords_t {
	int x;
	int y;
	int top;	
} tile_coords_p;

typedef struct tile_t {
	int sheet_index_x;
	int sheet_index_y;
	int screen_x;
	int screen_y;
	int map_x;
	int map_y;
	int colliding;
	int top;
	door_p* door;
	int action;
	tile_coords_p action_coords;
} tile_p;

void init_tile(tile_p** tile, SDL_Surface *screen, room_p* room, int sheet_index_x,
		int sheet_index_y,
		int map_x,
		int map_y,
		int colliding,
		int top,
		door_p* door);

void draw_tile(tile_p* tile, SDL_Surface *screen, room_p* room);
void delete_tile(room_p* room, int x, int y, int top);
void apply_action_tile(room_p* room, int x, int y, int top);
