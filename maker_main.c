#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#define CHARACTER
#include "char_make.h"
#include "allDefinitions.h"

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Surface *screen;
SDL_Texture* texture;

void draw_menu(player_p* player, SDL_Surface *screen)
{
	SDL_Rect src, dest;
	SDL_Surface *image = SDL_LoadBMP("sprites/menu_maker.bmp");

	Uint32 colorkey = SDL_MapRGB(image->format, 0, 0, 255);

	SDL_SetColorKey(image, SDL_TRUE, colorkey);
	src.x = 0;
	src.y = 0;
	src.w = 256;
	src.h = 960;
	dest.x = 2;
	dest.y = 0;
	dest.w = image->w;
	dest.h = image->h;
	SDL_BlitSurface(image, &src, screen, &dest);
}

void draw_routine(background_p* background, player_p* player, room_p* room, tile_p* tile)
{
	/* Redraw everything. */
	draw_background(background, screen, player, room);
	draw_room(room, screen,0);
	draw_player(player, screen);
	draw_tile(tile, screen, room);
	draw_room(room, screen,1);
	draw_menu(room, screen);
	
	/* Flip the page. */
	SDL_UpdateTexture(texture, NULL, screen->pixels, screen->pitch);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

void update_by_keystate(player_p* player, background_p* background, Uint8 *keystate)
{

	if(background == NULL) printf("WTF\n");	
	int direction = -1;
	int stand, frames;
	/* Left and right arrow keys control turning. */
	if (keystate[SDL_SCANCODE_LEFT] && !keystate[SDL_SCANCODE_RIGHT]
             && !keystate[SDL_SCANCODE_UP]   && !keystate[SDL_SCANCODE_DOWN])
	{
		direction = LEFT;
		stand = LEFT_STAND_FRAME;
		frames = LEFT_FRAMES;
	}
	else if(!keystate[SDL_SCANCODE_LEFT] && keystate[SDL_SCANCODE_RIGHT]
	     && !keystate[SDL_SCANCODE_UP]   && !keystate[SDL_SCANCODE_DOWN])
	{
		direction = RIGHT;
		stand = RIGHT_STAND_FRAME;
		frames = RIGHT_FRAMES;
	}
	else if(!keystate[SDL_SCANCODE_LEFT] && !keystate[SDL_SCANCODE_RIGHT]
	     && keystate[SDL_SCANCODE_UP]    && !keystate[SDL_SCANCODE_DOWN])
	{
		direction = UP;
		stand = UP_STAND_FRAME;
		frames = UP_FRAMES;
	}
	else if (!keystate[SDL_SCANCODE_LEFT] && !keystate[SDL_SCANCODE_RIGHT]
	     && !keystate[SDL_SCANCODE_UP]  && keystate[SDL_SCANCODE_DOWN]) 	
	{
		direction = DOWN;
		stand = DOWN_STAND_FRAME;
		frames = DOWN_FRAMES;
	}
	else
	{
		player->frame = player->stand_frame;
		frames = 1;
	}
	
	if(direction != -1)
		update_char(player, direction, stand, frames);
}

void create_new_tile(tile_p** tile, player_p* player, room_p* room)
{
	if(player->map_x >= 0 && player->map_y >= 0)
	{
		(*tile)->map_x = player->map_x;
		(*tile)->map_y = player->map_y;
		delete_tile(room, (*tile)->map_x, (*tile)->map_y, (*tile)->top);
		push_stack(&(room->backward_obs_stack), (*tile));
		init_tile(tile, screen, room, (*tile)->sheet_index_x, (*tile)->sheet_index_y, 607, 1, 0, 0, NULL);	
		draw_room(room, screen,0);
		draw_room(room, screen,1);
	}
}

void create_new_npc(player_p* player, room_p* room)
{
	int villain_type = -1;
	printf("What villain: \n\t1=Vampire\n2=Ghost");
	scanf("%d",&villain_type);
	add_villain(player->map_x, player->map_y,room,villain_type);
	(room->num_villains)++;
}

void coordinateSelector(int room_num,int* x_coord, int* y_coord){
	Uint8 *keystate;
	SDL_Event event;

	player_p* temp_player = NULL;
	background_p* temp_back = NULL;

	room_p* temp_room = init_room(room_num,-1, -1, screen);
	init_player(&temp_player, screen, "sprites/textures.bmp");
	init_background(&temp_back, screen, temp_player, temp_room);

	int done = 0;

	while(done == 0){
		SDL_PollEvent(&event);
    		while(event.type != SDL_KEYDOWN) SDL_PollEvent( &event );

		/* Update SDL's internal input state information. */
		SDL_PumpEvents();

		/* Grab a snapshot of the keyboard. */
		keystate = SDL_GetKeyboardState(NULL);
		
		update_by_keystate(temp_player, temp_back, keystate);
		
		if (keystate[SDL_SCANCODE_D] || keystate[SDLK_ESCAPE]){
			*x_coord = temp_player->map_x;
			*y_coord = temp_player->map_y;
			done = 1;
		}
		
		/* Redraw everything. */
		draw_background(temp_back, screen, temp_player, temp_room);
		draw_room(temp_room, screen,0);
		draw_player(temp_player, screen);
	
		/* Flip the page. */
		SDL_UpdateTexture(texture, NULL, screen->pixels, screen->pitch);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);

		usleep(150000);
	}

	
	freeRoom(&temp_room);
	freePlayer(&temp_player);
	freeBackground(&temp_back);
}

void PlayGame(player_p* player, background_p* background, room_p* room)
{
	Uint8 *keystate;
	int quit = 0;
	SDL_Event event;
	door_p* door = NULL;
	int room_number = 0;
	
	int mouse_x,mouse_y;
	int menu_option;

	tile_p* tile = NULL;
	init_tile(&tile, screen, room, 0, 0, 607, 1, 0, 0, NULL);

    	while (quit == 0) {

		SDL_PollEvent(&event);
    		while(event.type != SDL_KEYDOWN && event.type != SDL_MOUSEBUTTONDOWN) SDL_PollEvent( &event );
		/* Update SDL's internal input state information. */
		SDL_PumpEvents();

		/* Grab a snapshot of the keyboard. */
		keystate = SDL_GetKeyboardState(NULL);

		if (SDL_GetMouseState(&mouse_x, &mouse_y) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
			if(mouse_x < 256){
				menu_option = mouse_y/64;
				switch(menu_option)
				{
					case 0: 
						save_room(room);
						printf("Room saved\n");
						break;
					case 1: 
						quit = 1;
						break;
					case 2: 
						tile->colliding = 1;
						printf("Collision turned on\n");
						break;
					case 3: 
						tile->top = 1;
						printf("Top turned on\n");
						break;
					case 4: 
						door = (door_p*) malloc(sizeof(door_p));
						tile->door = door;
						printf("What room does this door lead to: ");
						scanf("%d",&(tile->door->room_num));

						coordinateSelector(tile->door->room_num,&(tile->door->player_start_x),&(tile->door->player_start_y));

						printf("Tile settings saved. Go ahead and place the tile.\n");
						door = NULL;
						break;
					case 5: 
						room->player_start_x = player->map_x;
						room->player_start_y = player->map_y;
						printf("Player location saved\n");
						break;
					case 6: 
						create_new_tile(&tile, player, room);
						printf("Tile placed.\n");
						break;
					case 7: 
						printf("Add Tile Action\n");
						printf("What room is this action applied to: ");
						scanf("%d",&room_number);
						coordinateSelector(room_number,&(tile->action_coords.x),&(tile->action_coords.y));
						
						break;
				}
			}
		}


		/* Respond to input. */
		if (keystate[SDL_SCANCODE_Q] || keystate[SDLK_ESCAPE]) quit = 1;
		if (keystate[SDL_SCANCODE_F])
			SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
		if (keystate[SDL_SCANCODE_X])
			SDL_SetWindowFullscreen(window, 0);

		if (keystate[SDL_SCANCODE_D]) tile->sheet_index_x++;
		if (keystate[SDL_SCANCODE_A]) tile->sheet_index_x--;
		if (keystate[SDL_SCANCODE_S]) tile->sheet_index_y++;
		if (keystate[SDL_SCANCODE_W]) tile->sheet_index_y--;

		if (keystate[SDL_SCANCODE_C])
		{
			tile->colliding = 1;
			printf("Collision turned on\n");
		} 
		if (keystate[SDL_SCANCODE_T])
		{
			tile->top = 1;
			printf("Top turned on\n");
		} 
		if (keystate[SDL_SCANCODE_V])
		{
			door = (door_p*) malloc(sizeof(door_p));
			tile->door = door;
			printf("What room does this door lead to: ");
			scanf("%d",&(tile->door->room_num));

			coordinateSelector(tile->door->room_num,&(tile->door->player_start_x),&(tile->door->player_start_y));

			printf("Tile settings saved. Go ahead and place the tile.\n");
			door = NULL;
		} 

		if (keystate[SDL_SCANCODE_P])
		{
			room->player_start_x = player->map_x;
			room->player_start_y = player->map_y;
			printf("Player location saved\n");
		}
		
		if(keystate[SDL_SCANCODE_Z])
		{
			delete_tile(room, player->map_x, player->map_y, tile->top);
			init_tile(&tile, screen, room, tile->sheet_index_x, tile->sheet_index_y, 607, 1, 0, 0, NULL);
		}

		if(keystate[SDL_SCANCODE_O])
		{ 
			save_room(room);
			printf("Room saved\n");
		}

		update_by_keystate(player, background, keystate);

		if(keystate[SDL_SCANCODE_E])
		{
			create_new_tile(&tile, player, room);
			printf("Tile placed.\n");
		}

		if(keystate[SDL_SCANCODE_N])
		{
			create_new_npc(player, room);
		}

		draw_routine(background,player,room,tile);

		usleep(150000);
	}
}

int main()
{
	player_p* player = NULL;
	background_p* background = NULL;
	room_p* room = NULL;
	int room_number = 0;	

	printf("What room number do you want to edit: ");
	scanf("%d",&room_number);

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Unable to initialize SDL: %s\n", SDL_GetError());
		return 1;
    	}

	atexit(SDL_Quit);

	screen = SDL_CreateRGBSurface(0, 1280, 960, 32,
                                        0x00FF0000,
                                        0x0000FF00,
                                        0x000000FF,
                                        0xFF000000);

	SDL_CreateWindowAndRenderer(1280, 960, 0, &window, &renderer);

	texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_ARGB8888,
                                            SDL_TEXTUREACCESS_STREAMING,
                                            1280, 960);

	if (screen == NULL) {
		printf("Unable to set video mode: %s\n", SDL_GetError());
		return 1;
	}
	
	room = init_room(room_number,-1, -1, screen);
	init_player(&player, screen, "sprites/textures.bmp");
	init_background(&background, screen, player, room);
		


	PlayGame(player, background, room);

	/* Free the memory that was allocated to the bitmaps. */
	SDL_FreeSurface(screen);
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	freeRoom(&room);
	freePlayer(&player);
	freeBackground(&background);

	return 0;
}
