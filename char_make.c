#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>

#include "char_make.h"


void character_attacked(player_p* player, int damage)
{
}

void init_player(player_p** player, SDL_Surface *screen, char* sprite_sheet)
{
	*player = (player_p*) malloc(sizeof(player_p));

	(*player)->stand_frame = DOWN_STAND_FRAME;
	(*player)->animation = DOWN;
	(*player)->frame = DOWN_STAND_FRAME; 
	(*player)->map_x = 0;
	(*player)->map_y = 0;
	(*player)->screen_x = 0;
	(*player)->screen_y = 0;
	(*player)->updates = 0;
	
	(*player)->image = SDL_LoadBMP(sprite_sheet);

	if ((*player)->image == NULL) {
		printf("Unable to load character bitmap.\n");
		exit(1);
	}

	draw_player((*player), screen);
}

void draw_player(player_p* player, SDL_Surface *screen)
{
	SDL_Rect src, dest;

	Uint32 colorkey = SDL_MapRGB(player->image->format, 0, 0, 255);

	SDL_SetColorKey(player->image, SDL_TRUE, colorkey);
	src.x = player->frame * PLAYER_W;
	src.y = 0;
	src.w = PLAYER_W;
	src.h = PLAYER_H;
	dest.x = player->screen_x;
	dest.y = player->screen_y;
	dest.w = player->image->w;
	dest.h = player->image->h;
	SDL_BlitSurface(player->image, &src, screen, &dest);
}


void update_char(player_p* player, animation_type direction, int stand_frame, int anim_frames)
{
	if(direction == UP)
	{
		player->map_y -= CHAR_SPEED;
	}
	
	if(direction == DOWN)
	{
		player->map_y += CHAR_SPEED;
	}	

	if(direction == LEFT)
	{
		player->map_x -= CHAR_SPEED;
	}
	
	if(direction == RIGHT)
	{
		player->map_x += CHAR_SPEED;
	}
}


void freePlayer(player_p** player_ptr){
	SDL_FreeSurface((*player_ptr)->image);
	free(*player_ptr);
}


