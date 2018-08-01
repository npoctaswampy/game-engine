#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>

#include "allDefinitions.h"



void init_player(player_p** player, SDL_Surface *screen, char* sprite_sheet,int player_start_x,int player_start_y)
{
	*player = (player_p*) malloc(sizeof(player_p));

	(*player)->stand_frame = DOWN_STAND_FRAME;
	(*player)->animation = DOWN;
	(*player)->frame = DOWN_STAND_FRAME; 
	(*player)->map_x = player_start_x;
	(*player)->map_y = player_start_y;
	(*player)->screen_x = player_start_x;
	(*player)->screen_y = player_start_y;
	(*player)->updates = 0;
	(*player)->health = 0;
	(*player)->speed = CHAR_SPEED;
	(*player)-> item_equipped = GUN;
	(*player)->item_collection = malloc(2*sizeof(int));
	(*player)->item_collection[0]=GUN;
	(*player)->item_collection[1]=CROSS;
	(*player)->item_collection_size = 2;
		
	
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
	src.y = player->health*PLAYER_H;
	src.w = PLAYER_W;
	src.h = PLAYER_H;
	dest.x = player->screen_x;
	dest.y = player->screen_y;
	dest.w = player->image->w;
	dest.h = player->image->h;
	SDL_BlitSurface(player->image, &src, screen, &dest);
}

void character_attacked(player_p* player, int damage)
{
	if(player->health < PLAYER_MAX_HEALTH)
		player->health += damage;
	printf("Character Attacked %d\n",player->health);
	player->speed -= damage;
}

void update_char(player_p* player, animation_type direction, int stand_frame, int anim_frames)
{
	if(!(player->health >= PLAYER_MAX_HEALTH))
	{
			

		if(player->animation == direction)
		{
			if((player->updates++) % ANIMATE_SPEED == 0) player->frame = ((player->frame+1) % anim_frames) + stand_frame;
		}else 
		{
			player->stand_frame = stand_frame;
			player->frame = stand_frame;
			player->animation = direction;
		}
		if(direction == UP)
		{
			player->map_y -= player->speed;
		}
	
		if(direction == DOWN)
		{
			player->map_y += player->speed;
		}	

		if(direction == LEFT)
		{
			player->map_x -= player->speed;
		}
	
		if(direction == RIGHT)
		{
			player->map_x += player->speed;
		}
	}
}

void interact_with_room(player_p* player,room_p* room)
{
	tile_p** action_tile =  NULL;
	printf("Interacting with room. %d %d\n",(player->map_x)/TILE_H,(player->map_y)/TILE_H);
	
	int x = 0; int y = 0;

	if(player->animation == UP)
	{
		y = ((player->map_y)/TILE_H)-1;
		x = (player->map_x)/TILE_W;
	}
	if(player->animation == DOWN)
	{
		y = ((player->map_y)/TILE_H)+1;
		x = (player->map_x)/TILE_W;
	}
	if(player->animation == LEFT)
	{
		x = ((player->map_x)/TILE_H)-1;
		y = (player->map_y)/TILE_W;
	}
	if(player->animation == RIGHT)
	{
		x = ((player->map_x)/TILE_H)+1;
		y = (player->map_y)/TILE_W;
	}	

	apply_action_tile(room, x, y, 1);
}

void change_item(player_p* player)
{
	player->item_equipped = (player->item_equipped+1)%(player->item_collection_size);
}

void freePlayer(player_p** player_ptr){
	SDL_FreeSurface((*player_ptr)->image);
	free(*player_ptr);
}


