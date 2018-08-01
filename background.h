#define BACKGROUND

#ifndef CHARACTER
#define CHARACTER
#include "character.h"
#endif

#ifndef ROOM
#define ROOM
#include "room.h"
#endif

typedef struct background_t {
	char* fn;
	SDL_Surface *image;
} background_p;

void init_background(background_p** background, SDL_Surface *screen, player_p* player, room_p* room);
void draw_background(background_p *background, SDL_Surface *screen, player_p* player, room_p* room);
void freeBackground(background_p** background);
