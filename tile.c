#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>

#include "allDefinitions.h"

void init_tile(tile_p** tile, SDL_Surface *screen, room_p* room, 
		int sheet_index_x,
		int sheet_index_y,
		int map_x,
		int map_y,
		int colliding,
		int top,
		door_p* door)
{
	if(!(map_x < 0 || map_y < 0))
	{

		*tile = (tile_p*) malloc(sizeof(tile_p));

		(*tile)->sheet_index_x = sheet_index_x;
		(*tile)->sheet_index_y = sheet_index_y;
	
		(*tile)->map_x = map_x;
		(*tile)->map_y = map_y;

		(*tile)->screen_x = map_x;
		(*tile)->screen_y = map_y;
	
		(*tile)->colliding = colliding;

		(*tile)->top = top;

		(*tile)->door = door;

		
		(*tile)->action = ACTION_REMOVE_COLLISION;
		(*tile)->action_coords.x=map_x;
		(*tile)->action_coords.y=map_y;
		(*tile)->action_coords.top=top;

		draw_tile((*tile), screen, room);

	}
}

tile_p* get_tile(room_p* room, int x, int y, int top)
{
	tile_p* tile = NULL;
	
	if(room->backward_obs_stack !=NULL)
	{
		while((tile = (tile_p*) pop_stack( &(room->backward_obs_stack) ) ) != NULL)
		{
			if(x == tile->map_x && y == tile->map_y && tile->top == top)
				return tile;
			else push_stack( &(room->forward_obs_stack), tile);
		}
	}
	if(room->forward_obs_stack !=NULL)
	{
		while((tile = (tile_p*) pop_stack( &(room->forward_obs_stack) ) ) != NULL)
		{
			if(x == tile->map_x && y == tile->map_y && tile->top == top)
				return tile;			
			else push_stack( &(room->backward_obs_stack), tile);
		}
	}else
	{
		printf("No room loaded\n");
	}
	
	return tile;
}

void add_tile(tile_p* tile, room_p* room)
{
	push_stack( &(room->forward_obs_stack), tile);
	printf("%d,%d\n",tile->action_coords.x,tile->action_coords.y);
	room->plane[tile->action_coords.y/TILE_H][tile->action_coords.x/TILE_W] = tile;
}

void delete_tile(room_p* room, int x, int y, int top)
{
	tile_p* tile = get_tile(room, x, y, top);
	if(tile != NULL)
	{
		printf("deleting %d, %d, %d\n",x,y,top);
		free(tile);
	}
}

void draw_tile(tile_p* tile, SDL_Surface *screen, room_p* room)
{
	SDL_Rect src, dest;

	Uint32 colorkey = SDL_MapRGB(room->image->format, 0, 0, 255);

	SDL_SetColorKey(room->image, SDL_TRUE, colorkey);
	src.x = tile->sheet_index_x * TILE_W;
	src.y = tile->sheet_index_y * TILE_H;
	src.w = TILE_W;
	src.h = TILE_H;
	dest.x = tile->screen_x;
	dest.y = tile->screen_y;
	dest.w = room->image->w;
	dest.h = room->image->h;
	SDL_BlitSurface(room->image, &src, screen, &dest);
}
//8,2
//Maybe should be called open door action or something
void remove_collision_action(tile_p* tile, room_p* room)
{
	tile_p* tile_acted = get_tile(room,tile->action_coords.x,tile->action_coords.y,1);
	if(tile_acted->colliding == 1)
	{
		printf("Collision detected on tile\n");
		tile_acted->colliding=0;
		tile_acted->sheet_index_y=2;
		tile_acted->sheet_index_x=8;
	}
	add_tile(tile_acted,room);
}

void apply_action_tile(room_p* room, int x, int y, int top)
{
	printf("Getting tile %d %d %d\n",x,y,top);
	tile_p* tile = get_tile(room,x*TILE_H,y*TILE_W,top);

	if(tile != NULL && tile->action == ACTION_REMOVE_COLLISION)
	{
		add_tile(tile,room);
		printf("Found it!\n");
		remove_collision_action(tile, room);
	}
}
