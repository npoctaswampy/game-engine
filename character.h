#define PLAYER_W 64
#define PLAYER_H 64

#define PLAYER_MAX_HEALTH 2

#define DOWN_FRAMES 3
#define LEFT_FRAMES 3
#define UP_FRAMES 3
#define RIGHT_FRAMES 3

#define DOWN_STAND_FRAME 0
#define LEFT_STAND_FRAME 3
#define UP_STAND_FRAME 6
#define RIGHT_STAND_FRAME 9

#define GUN 0
#define CROSS 1

#define CHAR_SPEED 12
#define ANIMATE_SPEED 3

#define CHARACTER

#ifndef ROOM
#define ROOM
#include "room.h"
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
	int health;
	int speed;
	int item_equipped;
	int item_collection_size;
	int* item_collection;
} player_p;

void init_player(player_p** player, SDL_Surface *screen, char* sprite_sheet,int player_start_x,int player_start_y);
void draw_player(player_p* player, SDL_Surface *screen);
void update_char(player_p* player, animation_type direction, int stand_frame, int anim_frames);
void character_attacked(player_p* player, int damage);
void freePlayer(player_p** player_ptr);
void change_item(player_p* player);
void interact_with_room(player_p* player,room_p* room);

