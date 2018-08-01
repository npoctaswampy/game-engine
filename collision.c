#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>

#include "allDefinitions.h"

int checkForCollisions(animation_type direction, background_p* background, player_p* player, room_p* room)
{
	if(room == NULL) return 0;

	int collision = 0;
	
	if(checkPerimeter(direction, background, player)) return 1;

	collision = check_tiles(room, player, direction);

	return collision;	
}

int checkPerimeter(animation_type direction, background_p* background, player_p* player)
{
	int collision = 0;
	if(direction == UP && player->map_y <= 1)
		collision = 1;
	if(direction == DOWN && player->map_y >= (background->image->h - PLAYER_H))
		collision = 1;
	if(direction == LEFT && player->map_x <= 1)
		collision = 1;
	if(direction == RIGHT && player->map_x >= (background->image->w - PLAYER_W))
		collision = 1;

	return collision;
}

int check_tiles(room_p* room, player_p* player, animation_type direction)
{
	int collision = 0;
	if(direction == UP && (
			      ((tile_p*)(room->plane[(player->map_y-10)/TILE_H][(player->map_x+32)/TILE_W]))->colliding == 1
			   || ((tile_p*)(room->plane[(player->map_y-10)/TILE_H][(player->map_x+18)/TILE_W]))->colliding == 1
			   || ((tile_p*)(room->plane[(player->map_y-10)/TILE_H][(player->map_x+42)/TILE_W]))->colliding == 1))
		collision = 1;
	if(direction == DOWN && (
			        ((tile_p*)(room->plane[(player->map_y+65)/TILE_H][(player->map_x+32)/TILE_W]))->colliding == 1
			     || ((tile_p*)(room->plane[(player->map_y+65)/TILE_H][(player->map_x+18)/TILE_W]))->colliding == 1
			     || ((tile_p*)(room->plane[(player->map_y+65)/TILE_H][(player->map_x+42)/TILE_W]))->colliding == 1))
		collision = 1;
	if(direction == LEFT && (
				((tile_p*)(room->plane[(player->map_y+32)/TILE_H][(player->map_x-10)/TILE_W]))->colliding == 1
			     || ((tile_p*)(room->plane[(player->map_y+18)/TILE_H][(player->map_x-10)/TILE_W]))->colliding == 1
			     || ((tile_p*)(room->plane[(player->map_y+42)/TILE_H][(player->map_x-10)/TILE_W]))->colliding == 1))
		collision = 1;
	if(direction == RIGHT && (
				 ((tile_p*)(room->plane[(player->map_y+32)/TILE_H][(player->map_x+65)/TILE_W]))->colliding == 1
			      || ((tile_p*)(room->plane[(player->map_y+18)/TILE_H][(player->map_x+65)/TILE_W]))->colliding == 1
			      || ((tile_p*)(room->plane[(player->map_y+42)/TILE_H][(player->map_x+65)/TILE_W]))->colliding == 1))
		collision = 1;

	return collision;
}

