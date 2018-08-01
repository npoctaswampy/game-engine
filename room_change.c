#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>

#include "allDefinitions.h"


door_p* check_direction(animation_type direction, animation_type looking_for,int y_val,int x_val,door_p* room_change,room_p* room){
	if(room_change != NULL) return room_change;
	if(direction == looking_for)
	{
		if(((tile_p*)(room->plane[y_val][x_val]))->door != NULL)
		{
			room_change = ((tile_p*)(room->plane[y_val][x_val]))->door;
		}
	}
	return room_change;
}

door_p* check_room_change_tiles(room_p* room, int player_x, int player_y, animation_type direction)
{
	door_p* room_change = NULL;
	room_change = check_direction(direction,UP,(player_y-1)/TILE_H,(player_x+32)/TILE_W,room_change,room);
	room_change = check_direction(direction,UP,(player_y-1)/TILE_H,(player_x+18)/TILE_W,room_change,room);
	room_change = check_direction(direction,UP,(player_y-1)/TILE_H,(player_x+42)/TILE_W,room_change,room);

	room_change = check_direction(direction,DOWN,(player_y+TILE_H)/TILE_H,(player_x+32)/TILE_W,room_change,room);
	room_change = check_direction(direction,DOWN,(player_y+TILE_H)/TILE_H,(player_x+18)/TILE_W,room_change,room);
	room_change = check_direction(direction,DOWN,(player_y+TILE_H)/TILE_H,(player_x+42)/TILE_W,room_change,room);

	room_change = check_direction(direction,LEFT,(player_y+32)/TILE_H,(player_x-1)/TILE_W,room_change,room);
	room_change = check_direction(direction,LEFT,(player_y+18)/TILE_H,(player_x-1)/TILE_W,room_change,room);
	room_change = check_direction(direction,LEFT,(player_y+42)/TILE_H,(player_x-1)/TILE_W,room_change,room);

	room_change = check_direction(direction,RIGHT,(player_y+32)/TILE_H,(player_x+TILE_W)/TILE_W,room_change,room);
	room_change = check_direction(direction,RIGHT,(player_y+18)/TILE_H,(player_x+TILE_W)/TILE_W,room_change,room);
	room_change = check_direction(direction,RIGHT,(player_y+42)/TILE_H,(player_x+TILE_W)/TILE_W,room_change,room);

	return room_change;
}

door_p* checkForRoomChange(animation_type direction, int player_x, int player_y, room_p* room)
{
	if(room == NULL) return NULL;

	door_p* room_change = NULL;

	room_change = check_room_change_tiles(room, player_x, player_y, direction);

	return room_change;	
}



