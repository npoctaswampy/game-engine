#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>

#include "allDefinitions.h"


void init_ghost(ghost_p** ghost, int start_x, int start_y, char* sprite_sheet)
{
	*ghost = (ghost_p*) malloc(sizeof(ghost_p));

	(*ghost)->stand_frame = GHOST_DOWN_STAND_FRAME;
	(*ghost)->animation = DOWN;
	(*ghost)->frame = GHOST_DOWN_STAND_FRAME; 
	(*ghost)->map_x = start_x;
	(*ghost)->map_y = start_y;
	(*ghost)->screen_x = start_x;
	(*ghost)->screen_y = start_y;
	(*ghost)->updates = 0;
	(*ghost)->last_attack = 0;
	(*ghost)->direction = DOWN;
	
	(*ghost)->image = SDL_LoadBMP(sprite_sheet);

	if ((*ghost)->image == NULL) {
		printf("Unable to load character bitmap.\n");
		exit(1);
	}

	//draw_ghost((*ghost), screen, screen->w/2, screen->h/2);
}

SDL_Rect ghost_calcX_Y(player_p* player, SDL_Surface *screen, background_p* background)
{
	SDL_Rect src;
	int top = 0;
	int left = 0;

	//Edges of the map- left and top
	if(player->map_x - screen->w/2 > 0)
	{
		src.x = player->map_x - screen->w/2;
		player->screen_x = screen->w/2;
	}else
	{
		src.x = 0;
		player->screen_x = player->map_x;
		left = 1;
	}
	if(player->map_y - screen->h/2 > 0)
	{	
		src.y = player->map_y - screen->h/2;
		player->screen_y = screen->h/2;
	}else 
	{
		src.y = 0;	
		player->screen_y = player->map_y;
		top = 1;
	}

	//Edges of the map- right and bottom
	if(player->map_x + screen->w/2 < background->image->w)
	{
		if(left == 0)
		{
			src.x = player->map_x - screen->w/2;
			player->screen_x = screen->w/2;
		}	
	}else
	{
		src.x = background->image->w - screen->w;
		player->screen_x = screen->w - (background->image->w - player->map_x);
	}
	if(player->map_y + screen->h/2 < background->image->h)
	{	
		if(top == 0)
		{
			src.y = player->map_y - screen->h/2;
			player->screen_y = screen->h/2;
		}
	}else 
	{
		src.y = background->image->h - screen->h;	
		player->screen_y = screen->h - (background->image->h - player->map_y);
	}

	return src;

}

void draw_ghost(ghost_p* ghost, SDL_Surface *screen, player_p* player, background_p* background)
{
	SDL_Rect src, dest;

	Uint32 colorkey = SDL_MapRGB(ghost->image->format, 0, 0, 255);

	SDL_Rect coord = ghost_calcX_Y(player, screen, background);

	ghost->screen_x = ghost->map_x - coord.x;
	ghost->screen_y = ghost->map_y - coord.y;

	SDL_SetColorKey(ghost->image, SDL_TRUE, colorkey);
	src.x = ghost->frame * GHOST_W;
	if(ghost->last_attack > 0)
	{
		src.y = GHOST_H;
	}else src.y = 0;
	src.w = GHOST_W;
	src.h = GHOST_H;
	dest.x = ghost->screen_x;
	dest.y = ghost->screen_y;
	dest.w = ghost->image->w;
	dest.h = ghost->image->h;
	SDL_BlitSurface(ghost->image, &src, screen, &dest);
}


void update_ghost(ghost_p* ghost, int stand_frame, int anim_frames)
{

	if(ghost->animation == ghost->direction){
		if((ghost->updates++) % GHOST_ANIMATE_SPEED == 0) ghost->frame = ((ghost->frame+1) % anim_frames) + stand_frame;
	}else {
		ghost->stand_frame = stand_frame;
		ghost->frame = stand_frame;
		ghost->animation = ghost->direction;
	}
	if(ghost->direction == UP)
	{
		ghost->map_y -= GHOST_SPEED;
	}
	
	if(ghost->direction == DOWN)
	{
		ghost->map_y += GHOST_SPEED;
	}	

	if(ghost->direction == LEFT)
	{
		ghost->map_x -= GHOST_SPEED;
	}
	
	if(ghost->direction == RIGHT)
	{
		ghost->map_x += GHOST_SPEED;
	}
}

void ghost_brain(ghost_p* ghost, background_p* background, Uint8 *keystate, room_p* room, player_p* player)
{

	int stand, frames;
	int standing = 0;
	int src_x = ghost->map_x/64;
	int src_y = ghost->map_y/64;

	if(ghost->last_attack > 0) ghost->last_attack--;
	else{

	if(room != NULL && ghost->map_x % 64 == 0 && ghost->map_y % 64 == 0)
		ghost->direction = getDirection(src_x,src_y,((PLAYER_W/2)+player->map_x)/64,((PLAYER_H/2)+player->map_y)/64,room); 
	if(  	(((GHOST_W/2)+ghost->map_x)/32) <= (((PLAYER_W/2)+player->map_x)/32)+1  
			&&  
		(((GHOST_W/2)+ghost->map_x)/32) >= (((PLAYER_W/2)+player->map_x)/32)-1
			&&   
		(((GHOST_H/2)+ghost->map_y)/32) <= (((PLAYER_H/2)+player->map_y)/32)+1
			&& 
		(((GHOST_H/2)+ghost->map_y)/32) >= (((PLAYER_H/2)+player->map_y)/32)-1
	)
	{	
		character_attacked(player, GHOST_DAMAGE);	
		ghost->last_attack = 20;
	}

	/* Left and right arrow keys control turning. */
	if (ghost->direction==LEFT)
	{
		stand = GHOST_LEFT_STAND_FRAME;
		frames = GHOST_LEFT_FRAMES;
	}

	else if(ghost->direction==RIGHT)
	{
		stand = GHOST_RIGHT_STAND_FRAME;
		frames = GHOST_RIGHT_FRAMES;
	}
	else if(ghost->direction==UP)
	{
		stand = GHOST_UP_STAND_FRAME;
		frames = GHOST_UP_FRAMES;
	}
	else if (ghost->direction==DOWN) 	
	{
		stand = GHOST_DOWN_STAND_FRAME;
		frames = GHOST_DOWN_FRAMES;
	}
	else
	{
		standing = 1;
		ghost->frame = ghost->stand_frame;
	}
	
	if(standing == 0)
		update_ghost(ghost, stand, frames);
	}
}



