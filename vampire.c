#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>

#include "allDefinitions.h"


void init_vampire(vampire_p** vampire, int start_x, int start_y, char* sprite_sheet)
{
	*vampire = (vampire_p*) malloc(sizeof(vampire_p));

	(*vampire)->stand_frame = VAMP_DOWN_STAND_FRAME;
	(*vampire)->animation = DOWN;
	(*vampire)->frame = VAMP_DOWN_STAND_FRAME; 
	(*vampire)->map_x = start_x;
	(*vampire)->map_y = start_y;
	(*vampire)->screen_x = start_x;
	(*vampire)->screen_y = start_y;
	(*vampire)->updates = 0;
	(*vampire)->last_attack = 0;
	(*vampire)->direction = DOWN;
	
	(*vampire)->image = SDL_LoadBMP(sprite_sheet);

	if ((*vampire)->image == NULL) {
		printf("Unable to load character bitmap.\n");
		exit(1);
	}

	//draw_vampire((*vampire), screen, screen->w/2, screen->h/2);
}

SDL_Rect vampire_calcX_Y(player_p* player, SDL_Surface *screen, background_p* background)
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

void draw_vampire(vampire_p* vampire, SDL_Surface *screen, player_p* player, background_p* background)
{
	SDL_Rect src, dest;

	Uint32 colorkey = SDL_MapRGB(vampire->image->format, 0, 0, 255);

	SDL_Rect coord = vampire_calcX_Y(player, screen, background);

	vampire->screen_x = vampire->map_x - coord.x;
	vampire->screen_y = vampire->map_y - coord.y;

	SDL_SetColorKey(vampire->image, SDL_TRUE, colorkey);
	src.x = vampire->frame * VAMP_W;
	if(vampire->last_attack > 0)
	{
		src.y = VAMP_H;
	}else src.y = 0;
	src.w = VAMP_W;
	src.h = VAMP_H;
	dest.x = vampire->screen_x;
	dest.y = vampire->screen_y;
	dest.w = vampire->image->w;
	dest.h = vampire->image->h;
	SDL_BlitSurface(vampire->image, &src, screen, &dest);
}


void update_vampire(vampire_p* vampire, int stand_frame, int anim_frames)
{

	if(vampire->animation == vampire->direction){
		if((vampire->updates++) % VAMP_ANIMATE_SPEED == 0) vampire->frame = ((vampire->frame+1) % anim_frames) + stand_frame;
	}else {
		vampire->stand_frame = stand_frame;
		vampire->frame = stand_frame;
		vampire->animation = vampire->direction;
	}
	if(vampire->direction == UP)
	{
		vampire->map_y -= VAMP_SPEED;
	}
	
	if(vampire->direction == DOWN)
	{
		vampire->map_y += VAMP_SPEED;
	}	

	if(vampire->direction == LEFT)
	{
		vampire->map_x -= VAMP_SPEED;
	}
	
	if(vampire->direction == RIGHT)
	{
		vampire->map_x += VAMP_SPEED;
	}
}

void vamp_brain(vampire_p* vampire, background_p* background, Uint8 *keystate, room_p* room, player_p* player)
{

	int stand, frames;
	int standing = 0;
	int src_x = vampire->map_x/64;
	int src_y = vampire->map_y/64;

	if(vampire->last_attack > 0) vampire->last_attack--;
	else{

	if(room != NULL && vampire->map_x % 64 == 0 && vampire->map_y % 64 == 0)
		vampire->direction = getDirection(src_x,src_y,((PLAYER_W/2)+player->map_x)/64,((PLAYER_H/2)+player->map_y)/64,room); 
	if(  	(((VAMP_W/2)+vampire->map_x)/32) <= (((PLAYER_W/2)+player->map_x)/32)+1  
			&&  
		(((VAMP_W/2)+vampire->map_x)/32) >= (((PLAYER_W/2)+player->map_x)/32)-1
			&&   
		(((VAMP_H/2)+vampire->map_y)/32) <= (((PLAYER_H/2)+player->map_y)/32)+1
			&& 
		(((VAMP_H/2)+vampire->map_y)/32) >= (((PLAYER_H/2)+player->map_y)/32)-1
	)
	{	
		character_attacked(player, VAMP_DAMAGE);	
		vampire->last_attack = 50;
	}

	/* Left and right arrow keys control turning. */
	if (vampire->direction==LEFT)
	{
		stand = VAMP_LEFT_STAND_FRAME;
		frames = VAMP_LEFT_FRAMES;
	}

	else if(vampire->direction==RIGHT)
	{
		stand = VAMP_RIGHT_STAND_FRAME;
		frames = VAMP_RIGHT_FRAMES;
	}
	else if(vampire->direction==UP)
	{
		stand = VAMP_UP_STAND_FRAME;
		frames = VAMP_UP_FRAMES;
	}
	else if (vampire->direction==DOWN) 	
	{
		stand = VAMP_DOWN_STAND_FRAME;
		frames = VAMP_DOWN_FRAMES;
	}
	else
	{
		standing = 1;
		vampire->frame = vampire->stand_frame;
	}
	
	if(standing == 0)
		update_vampire(vampire, stand, frames);
	}
}

void vampire_hit(vampire_p* vampire)
{
	vampire->last_attack = 50;
}








