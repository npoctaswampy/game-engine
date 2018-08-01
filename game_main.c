#include <SDL2/SDL.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>
#include <pthread.h>

#include "allDefinitions.h"

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Surface *screen;
SDL_Texture* texture;

player_p* player;
background_p* background;
room_p* room;
int quit;
pthread_mutex_t lock;
int action_timer;


void update_villains(background_p* background, Uint8 *keystate, room_p* room, player_p* player)
{	
	int i;
	for(i=0;i<room->vamp_count;i++)
		vamp_brain((vampire_p*)room->vampire_collection[i], background, keystate, room, player);
	for(i=0;i<room->ghost_count;i++)
		ghost_brain((ghost_p*)room->ghost_collection[i], background, keystate, room, player);
}

void draw_villains(background_p* background, room_p* room, player_p* player)
{	
	int i;
	for(i=0;i<room->vamp_count;i++)
		draw_vampire((vampire_p*)room->vampire_collection[i], screen, player, background);
	for(i=0;i<room->ghost_count;i++)
		draw_ghost((ghost_p*)room->ghost_collection[i], screen, player, background);
}

door_p* openingScreen()
{
	door_p* door = NULL;
	int selection = 0;
	int quit = 0;
	Uint8 *keystate;

	while (quit == 0 && door == NULL) {
		
		//usleep(5000);
		/* Update SDL's internal input state information. */
		SDL_PumpEvents();

		/* Grab a snapshot of the keyboard. */
		keystate = SDL_GetKeyboardState(NULL);

		/* Respond to input. */
		if (keystate[SDL_SCANCODE_Q] || keystate[SDL_SCANCODE_ESCAPE]) quit = 1;
		if (keystate[SDL_SCANCODE_E])
		{
			door = (door_p*) malloc(sizeof(door_p));
			door->room_num = 0;
			door->player_start_x = -1;
			door->player_start_y = -1;
		}

		if(keystate[SDL_SCANCODE_UP])
			selection++;
		if(keystate[SDL_SCANCODE_DOWN])
			selection--;

		SDL_UpdateTexture(texture, NULL, screen->pixels, screen->pitch);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
	}

	return door;
	
}

void draw_routine()
{
	/* Redraw everything. */
	draw_background(background, screen, player, room);
	draw_room(room, screen,0);
	draw_player(player, screen);
	draw_villains(background,  room,  player);
	draw_room(room, screen, 1);

	/* Flip the page. */
	SDL_UpdateTexture(texture, NULL, screen->pixels, screen->pitch);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}


void update_by_keystate(Uint8 *keystate)
{
	int direction, stand, frames;
	int standing = 0;
	int collision = 0;
	door_p* door = NULL;
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
		standing = 1;
		player->frame = player->stand_frame;
	}

	if (keystate[SDL_SCANCODE_X] && action_timer <= 0)
	{
		action_timer = 10;
		useItem(player,room);
	}
	if (keystate[SDL_SCANCODE_Z] && action_timer <= 4)
	{
		action_timer = 10;
		printf("Item Switch\n");
		change_item(player);
	}

	if (keystate[SDL_SCANCODE_E] && action_timer <= 4)
	{
		action_timer = 10;
		interact_with_room(player,room);
	}

	if(action_timer >= 0) 
		action_timer--;

	if(standing == 0){
		collision = checkForCollisions(direction, background, player, room);
		door = checkForRoomChange(direction, player->map_x, player->map_y, room);
	}
	if(standing == 0 && collision == 0 && door == NULL)
		update_char(player, direction, stand, frames);
	else if(collision == 1)
	{
		player->stand_frame = stand;
		player->frame = stand;
		player->animation = direction;
	}
	else if(door != NULL)
	{
		pthread_mutex_lock(&lock);
		freeRoom(&room);
		freePlayer(&player);
		freeBackground(&background);
	
		room = init_room(door->room_num, door->player_start_x, door->player_start_y, screen);
		init_player(&player, screen, "sprites/sprite_sheet_boy.bmp",room->player_start_x,room->player_start_y);
		init_background(&background, screen, player, room);

		draw_routine();

		pthread_mutex_unlock(&lock);
	}
}

void villain_thread(void* keystate)
{
	while(quit == 0){
		usleep(10000);
		
		pthread_mutex_lock(&lock);
		update_villains(background, (Uint8 *)keystate, room, player);
		pthread_mutex_unlock(&lock);
	}
}

void PlayGame()
{
	 if (pthread_mutex_init(&lock, NULL) != 0)
	 {
		printf("\n mutex init failed\n");
		exit(0);
	 }
	
	Uint8 *keystate;
	quit = 0;
	pthread_t tid;
	 
	pthread_create(&tid, NULL, villain_thread, (void*)keystate);
	
    	while (quit == 0) {
		usleep(20000);
		/* Update SDL's internal input state information. */
		SDL_PumpEvents();

		/* Grab a snapshot of the keyboard. */
		keystate = SDL_GetKeyboardState(NULL);

		/* Respond to input. */
		if (keystate[SDL_SCANCODE_Q] || keystate[SDL_SCANCODE_ESCAPE]) quit = 1;
		if (keystate[SDL_SCANCODE_F])
			SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
		if (keystate[SDL_SCANCODE_P])
			SDL_SetWindowFullscreen(window, 0);


		update_by_keystate(keystate);
		//update_villains(background, keystate, room, player);

		draw_routine();
	}
	
	pthread_join(tid, NULL);
	
    	pthread_mutex_destroy(&lock);
}

int main()
{

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Unable to initialize SDL: %s\n", SDL_GetError());
		return 1;
    	}

	atexit(SDL_VideoQuit);
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

	if (window == NULL) {
		printf("Unable to set video mode: %s\n", SDL_GetError());
		return 1;
	}

	door_p* door = openingScreen();

	room = init_room(door->room_num, door->player_start_x, door->player_start_y, screen);
	init_player(&player, screen, "sprites/sprite_sheet_boy.bmp",room->player_start_x,room->player_start_y);
	init_background(&background, screen, player, room);


	PlayGame();

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
