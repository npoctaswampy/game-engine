#define ROOM

#define OBSTACLE_START 7

#ifndef CHARACTER
#define CHARACTER
#include "character.h"
#endif

typedef struct room_t {
	SDL_Surface *image;
	char* fn;
	int number;
	stack* forward_obs_stack;
	stack* backward_obs_stack;
	void*** plane;
	int** adj;
	int** pred;
	int player_start_x;	
	int player_start_y;
	int plane_size;
	int num_villains;
	int vamp_count;
	void** vampire_collection;
	int ghost_count;
	void** ghost_collection;
} room_p;


room_p* init_room(int room_number,int player_start_x,int player_start_y, SDL_Surface* screen);
void draw_room(room_p* room, SDL_Surface* screen, int top);
void setTileScreens(room_p* room, int x, int y);
void save_room(room_p* room);
void shortestPath(room_p* room, int v1);
animation_type getDirection(int srcx, int srcy, int destx, int desty, room_p* room);
void buildGraph(room_p* room);
void freeRoom(room_p** room);
void add_villain(int villain_start_x,int villain_start_y, room_p* room, int villain_type);
