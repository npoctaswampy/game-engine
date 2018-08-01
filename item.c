#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>

#include "allDefinitions.h"

void useItem(player_p* player,room_p* room)
{
	if(player->item_equipped == GUN)
		fire_gun(player->animation,player->map_x,player->map_y,room);
}

void fire_gun(int direction,int x,int y,room_p* room)
{
	int loc_x, loc_y, i;
	x=(x+32)/64;
	y=(y+32)/64;
	for(i=0;i<room->vamp_count;i++)
	{
		vampire_p* vampire = (vampire_p*)room->vampire_collection[i];
		loc_x = (vampire->map_x+32)/64;
		loc_y = (vampire->map_y+32)/64;

		if(direction == LEFT && loc_x < x && loc_y == y) vampire_hit(vampire);
		if(direction == RIGHT && loc_x > x && loc_y == y) vampire_hit(vampire);
		if(direction == UP && loc_x == x && loc_y < y) vampire_hit(vampire);
		if(direction == DOWN && loc_x == x && loc_y > y) vampire_hit(vampire);
	}
}
