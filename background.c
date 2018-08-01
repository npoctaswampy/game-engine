#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>

#include "allDefinitions.h"


void init_background(background_p** background, SDL_Surface *screen, player_p* player, room_p* room)
{
	*background = (background_p*) malloc(sizeof(background_p));	
	
	(*background)->image = SDL_LoadBMP("sprites/Untitled.bmp");

	if ((*background)->image == NULL) {
		printf("Unable to load bitmap.\n");
		exit(1);
	}

	draw_background((*background), screen, player, room);
}

void draw_background(background_p *background, SDL_Surface *screen, player_p* player, room_p* room)
{
	SDL_Rect src, dest;
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

	setTileScreens(room, src.x, src.y);

	src.w = background->image->w;
	src.h = background->image->h;
	dest.x = 0;
	dest.y = 0;
	dest.w = background->image->w;
	dest.h = background->image->h;
	SDL_BlitSurface(background->image, &src, screen, &dest);
}


void freeBackground(background_p** background){
	SDL_FreeSurface((*background)->image);
	free(*background);
}




