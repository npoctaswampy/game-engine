#define PLAYER_W 64
#define PLAYER_H 64

#define DOWN_FRAMES 1
#define LEFT_FRAMES 1
#define UP_FRAMES 1
#define RIGHT_FRAMES 1

#define DOWN_STAND_FRAME 2
#define LEFT_STAND_FRAME 2
#define UP_STAND_FRAME 2
#define RIGHT_STAND_FRAME 2

#define CHAR_SPEED 64
#define ANIMATE_SPEED 1

#ifndef DEFINITIONS
#define DEFINITIONS
#include "definitions.h"
#endif

typedef struct player_t {
	animation_type animation;
	int frame;
	int stand_frame;
	int screen_x;
	int screen_y;
	int map_x;
	int map_y;
	int updates;
	SDL_Surface *image;
} player_p;

void init_player(player_p** player, SDL_Surface *screen, char* sprite_sheet);
void draw_player(player_p* player, SDL_Surface *screen);
void update_char(player_p* player, animation_type direction, int stand_frame, int anim_frames);
void freePlayer(player_p** player_ptr);
void character_attacked(player_p* player, int damage);



