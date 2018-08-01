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

#define INF 9999999
#define PLANE_SIZE 40
#define COLLIDE 9999998

char* rooms[] = 
	{
	"./rooms/R0.room",
	"./rooms/R1.room",
	"./rooms/R2.room",
	"./rooms/R3.room",
	"./rooms/R4.room",
	"./rooms/R5.room",
	"./rooms/R6.room",
	"./rooms/R7.room",
	"./rooms/R8.room",
	"./rooms/R9.room",
	"./rooms/R10.room",
	"./rooms/R11.room",
	"./rooms/R12.room",
	"./rooms/R13.room",
	};

char buffer[20];

room_p* init_room(int room_number,int player_start_x,int player_start_y, SDL_Surface* screen)
{
	room_p* room = (room_p*) malloc(sizeof(room_p));
	
	room->forward_obs_stack = NULL;
	room->backward_obs_stack = NULL;

	room->num_villains=0;
	int villain_start_x,villain_start_y,villain_type;

	room->vampire_collection = NULL;
	room->vamp_count = 0;

	room->ghost_collection = NULL;
	room->ghost_count = 0;

	FILE* fp = fopen(rooms[room_number], "r");
	
	int sheet_index_x,sheet_index_y, map_x, map_y, colliding, top, i, j;

	door_p* door = (door_p*) malloc(sizeof(door_p));
	door->room_num = -1;	
	door->player_start_x = -1;
	door->player_start_y = -1;
	
			
	int size = -1;
	int* n = (int*) malloc(9 * sizeof(int));
	int* player_pos = (int*) malloc(2 * sizeof(int));
	tile_p* tile = NULL;

	room->image = SDL_LoadBMP("sprites/textures.bmp");

	room->fn = rooms[room_number];

	if (room->image == NULL) {
		printf("Unable to load bitmap.\n");
		exit(1);
	}

	room->plane = (void***) malloc(sizeof(tile_p**) * PLANE_SIZE);
	for(i=0; i < PLANE_SIZE; i++)
	{
		room->plane[i] = (void**) malloc(sizeof(tile_p*) * PLANE_SIZE);
		
		for(j=0; j < PLANE_SIZE; j++)
		{
			room->plane[i][j] = NULL;
		} 
	}
		

	if(fp != NULL)
	{
		fread(player_pos,2*sizeof(int),1,fp);
		if(player_start_x == -1 || player_start_y == -1)
		{
			room->player_start_x = player_pos[0];
			room->player_start_y = player_pos[1];
		}else{
			room->player_start_x = player_start_x;
			room->player_start_y = player_start_y;
		}

		fread(&(room->num_villains),sizeof(int),1,fp);

		for(i=0;i<room->num_villains;i++)
		{	
			fread(&villain_type,sizeof(int),1,fp);
			fread(&villain_start_x,sizeof(int),1,fp);
			fread(&villain_start_y,sizeof(int),1,fp);
			add_villain(villain_start_x,villain_start_y,room,villain_type);
		}
		
		while(size != 0)
		{
		
			size = fread(n, 9*sizeof(int), 1, fp);

			sheet_index_x = n[0];
			sheet_index_y = n[1];
			map_x = n[2];
			map_y = n[3]; 
			colliding = n[4];
			top = n[5];
			door->room_num = n[6];
			door->player_start_x = n[7];
			door->player_start_y = n[8];
			if(
				(n[6] == -1 || n[7] == -1 || n[8] == -1) &&
				((map_x/TILE_W) < PLANE_SIZE && (map_y/TILE_H) < PLANE_SIZE)
			)
			{
				init_tile(&tile, screen, room, sheet_index_x, sheet_index_y, map_x, map_y, colliding, top, NULL);
				if(room->plane[n[3]/TILE_W][n[2]/TILE_H] != NULL && ((tile_p*)(room->plane[n[3]/TILE_W][n[2]/TILE_H]))->colliding == 1)
					tile->colliding = 1;
				if(room->plane[n[3]/TILE_W][n[2]/TILE_H] != NULL && ((tile_p*)(room->plane[n[3]/TILE_W][n[2]/TILE_H]))->door != NULL)
					tile->door = ((tile_p*)(room->plane[n[3]/TILE_W][n[2]/TILE_H]))->door;
				room->plane[n[3]/TILE_W][n[2]/TILE_H] = tile;
				push_stack(&(room->backward_obs_stack), tile);
			}else if(
					((map_x/TILE_W) < PLANE_SIZE && (map_y/TILE_H) < PLANE_SIZE)
				)
			{
				init_tile(&tile, screen, room, sheet_index_x, sheet_index_y, map_x, map_y, colliding, top, door);
				if(room->plane[n[3]/TILE_W][n[2]/TILE_H] != NULL && ((tile_p*)(room->plane[n[3]/TILE_W][n[2]/TILE_H]))->colliding == 1)
					tile->colliding = 1;
				if(room->plane[n[3]/TILE_W][n[2]/TILE_H] != NULL && ((tile_p*)(room->plane[n[3]/TILE_W][n[2]/TILE_H]))->door != NULL)
					tile->door = ((tile_p*)(room->plane[n[3]/TILE_W][n[2]/TILE_H]))->door;
				room->plane[n[3]/TILE_W][n[2]/TILE_H] = tile;
				push_stack(&(room->backward_obs_stack), tile);
				door = (door_p*) malloc(sizeof(door_p));
				door->room_num = -1;	
				door->player_start_x = -1;
				door->player_start_y = -1;
			}
		}
		fclose(fp);
	}else{
		room->player_start_x = screen->w/1.2;
		room->player_start_y = screen->h/1.2;
	}
	
	free(n);
	free(player_pos);

	buildGraph(room);
	draw_room(room, screen,0);

	return room;
}
	
void setTileScreens(room_p* room, int x, int y)
{
	tile_p* tile = NULL;

	if(room->backward_obs_stack !=NULL)
	{
		while((tile = (tile_p*) pop_stack( &(room->backward_obs_stack) ) ) != NULL)
		{
			tile->screen_x = tile->map_x - x;
			tile->screen_y = tile->map_y - y;
			push_stack( &(room->forward_obs_stack), tile);
		}
	}
	if(room->forward_obs_stack !=NULL)
	{
		while((tile = (tile_p*) pop_stack( &(room->forward_obs_stack) ) ) != NULL)
		{
			tile->screen_x = tile->map_x - x;
			tile->screen_y = tile->map_y - y;
			push_stack( &(room->backward_obs_stack), tile);
		}
	}else
	{
		printf("No room loaded\n");
	}
}	
	
void draw_room(room_p* room, SDL_Surface* screen, int top)
{
	tile_p* tile = NULL;

	if(room->backward_obs_stack !=NULL)
	{
		while((tile = (tile_p*) pop_stack( &(room->backward_obs_stack) ) ) != NULL)
		{
			if(tile->top == top)
			{
				draw_tile(tile, screen, room);
			}
			push_stack( &(room->forward_obs_stack), tile);
		}
	}else if(room->forward_obs_stack !=NULL)
	{
		while((tile = (tile_p*) pop_stack( &(room->forward_obs_stack) ) ) != NULL)
		{
			if(tile->top == top)
			{
				draw_tile(tile, screen, room);
			}
			push_stack( &(room->backward_obs_stack), tile);
		}
	}else
	{
		printf("No room loaded\n");
	}
}

void save_room(room_p* room)
{
	FILE* fp = fopen(room->fn,"w");
	vampire_p* vampire = NULL;
	ghost_p* ghost = NULL;
	int villain_type,i;

	int* n = (int*) malloc(9 * sizeof(int));
	int* player_pos = (int*) malloc(2 * sizeof(int));
	tile_p* tile = NULL;

	player_pos[0] = room->player_start_x;
	player_pos[1] = room->player_start_y;
	
	fwrite(player_pos, 2*sizeof(int), 1, fp);
	fwrite(&(room->num_villains),sizeof(int), 1, fp);

	for(i=0;i<room->vamp_count;i++)
	{	
		vampire = (vampire_p*)room->vampire_collection[i];
		villain_type = 1;
		fwrite(&villain_type,sizeof(int),1,fp);
		fwrite(&(vampire->map_x),sizeof(int),1,fp);
		fwrite(&(vampire->map_y),sizeof(int),1,fp);
	}
	for(i=0;i<room->ghost_count;i++)
	{	
		ghost = (ghost_p*)room->ghost_collection[i];
		villain_type = 2;
		fwrite(&villain_type,sizeof(int),1,fp);
		fwrite(&(ghost->map_x),sizeof(int),1,fp);
		fwrite(&(ghost->map_y),sizeof(int),1,fp);
	}

	if(room->backward_obs_stack !=NULL)
	{
		while((tile = (tile_p*) pop_stack( &(room->backward_obs_stack) ) ) != NULL)
		{
			n[0] = tile->sheet_index_x;
			n[1] = tile->sheet_index_y;
			n[2] = tile->map_x;
			n[3] = tile->map_y;
			n[4] = tile->colliding;
			n[5] = tile->top;
			if(tile->door != NULL)
			{
				n[6] = tile->door->room_num;
				n[7] = tile->door->player_start_x;
				n[8] = tile->door->player_start_y;
			}else{
				n[6] = -1;
				n[7] = -1;
				n[8] = -1;
			}
			fwrite(n, 9*sizeof(int), 1, fp);
			push_stack( &(room->forward_obs_stack), tile);
		}
	}else if(room->forward_obs_stack !=NULL)
	{
		while((tile = (tile_p*) pop_stack( &(room->forward_obs_stack) ) ) != NULL)
		{
			n[0] = tile->sheet_index_x;
			n[1] = tile->sheet_index_y;
			n[2] = tile->map_x;
			n[3] = tile->map_y;
			n[4] = tile->colliding;
			n[5] = tile->top;
			if(tile->door != NULL)
			{
				n[6] = tile->door->room_num;
				n[7] = tile->door->player_start_x;
				n[8] = tile->door->player_start_y;
			}else{
				n[6] = -1;
				n[7] = -1;
				n[8] = -1;
			}
			fwrite(n, 9*sizeof(int), 1, fp);
			push_stack( &(room->backward_obs_stack), tile);
		}
	}else
	{
		printf("No room loaded\n");
	}
	
	free(n);
	free(player_pos);
	fclose(fp);
}

void buildGraph(room_p* room)
{
	int i,j,k;
	int** distance = (int**) malloc(sizeof(int*)*PLANE_SIZE*PLANE_SIZE);
	room->pred  = (int**) malloc(sizeof(int*)*PLANE_SIZE*PLANE_SIZE);\

	for(i=0; i<(PLANE_SIZE*PLANE_SIZE); i++)
	{
		room->pred[i] = (int*) malloc(sizeof(int)*PLANE_SIZE*PLANE_SIZE);
		for(j=0;j<(PLANE_SIZE*PLANE_SIZE);j++)
			room->pred[i][j] = INF;
	}

	for(i=0; i<(PLANE_SIZE*PLANE_SIZE); i++)
	{
		distance[i] = (int*) malloc(sizeof(int)*PLANE_SIZE*PLANE_SIZE);
		for(j=0;j<(PLANE_SIZE*PLANE_SIZE);j++)
			distance[i][j] = INF;
	}
	for(i=0; i<PLANE_SIZE; i++)
	{
		for(j=0; j<PLANE_SIZE; j++)
		{
			tile_p* tile = ((tile_p*)(room->plane[i][j]));

			if(tile != NULL && tile->colliding != 1)
			{
								

				if(i<PLANE_SIZE-1)
				{
					tile_p* tile_iplus1 = ((tile_p*)(room->plane[i+1][j]));
					if(tile_iplus1 != NULL && tile_iplus1->colliding != 1) 
						distance[(i*PLANE_SIZE)+j][((i*PLANE_SIZE)+j)+PLANE_SIZE] = 1;
				}

				if(j<PLANE_SIZE-1)
				{
					tile_p* tile_jplus1 = ((tile_p*)(room->plane[i][j+1]));

					if(tile_jplus1 != NULL && tile_jplus1->colliding != 1) 
						distance[(i*PLANE_SIZE)+j][((i*PLANE_SIZE)+j)+1] = 1;
				}

				if(i>0)
				{
					tile_p* tile_iminus1 = ((tile_p*)(room->plane[i-1][j]));
					
					if(tile_iminus1 != NULL && tile_iminus1->colliding != 1)
						distance[(i*PLANE_SIZE)+j][((i*PLANE_SIZE)+j)-PLANE_SIZE] = 1;
				}

				if(j>0)
				{		
					tile_p* tile_jminus1 = ((tile_p*)(room->plane[i][j-1]));
		
					 if(tile_jminus1 != NULL && tile_jminus1->colliding != 1) 
						distance[(i*PLANE_SIZE)+j][((i*PLANE_SIZE)+j)-1] = 1;
				}

				distance[(i*PLANE_SIZE)+j][(i*PLANE_SIZE)+j] = 0;
			}
			else if(tile != NULL && tile->colliding == 1)
			{
				for(k=0; k<(PLANE_SIZE*PLANE_SIZE); k++)
				{
					distance[(i*PLANE_SIZE)+j][k] = COLLIDE;
					distance[k][(i*PLANE_SIZE)+j] = COLLIDE;
				}
			}
		}
	}

	room->adj = distance;
}

animation_type getDirection(int srcx, int srcy, int destx, int desty, room_p* room)
{
	clock_t begin = clock();
	tile_p* tile = ((tile_p*)(room->plane[destx][desty]));
	int i = 0;


	int v1 = (srcy*PLANE_SIZE)+srcx;
	int rove = (desty*PLANE_SIZE)+destx;

	shortestPath(room, v1);

	while(rove < (PLANE_SIZE*PLANE_SIZE) && v1 < (PLANE_SIZE*PLANE_SIZE) && room->pred[v1][rove] != v1)
	{
		rove = room->pred[v1][rove];
	}

	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

	//printf("%lf\n",time_spent);

	if(rove == v1 + 1) return RIGHT;
	if(rove == v1 - 1) return LEFT;
	if(rove == v1 - PLANE_SIZE) return UP;
	if(rove == v1 + PLANE_SIZE) return DOWN;
	return 10; 
}

void shortestPath(room_p* room, int v1)
{
	int* distances = (int*) malloc(sizeof(int)*PLANE_SIZE*PLANE_SIZE);

	int i,j,w,min;

	int* visited = (int*) malloc(sizeof(int)*PLANE_SIZE*PLANE_SIZE);

	int all_visited = 0;

	all_visited = 0;
	for(i=0; i<(PLANE_SIZE*PLANE_SIZE); i++)
	{
		distances[i] = room->adj[v1][i];
		if(distances[i] == 1) room->pred[v1][i] = v1;
		visited[i] = 0;
		if(distances[i]!=COLLIDE)
		{
			visited[i] = 0;
		}else visited[i] = 1;
	}
	visited[v1] = 1;

	int focus_node = v1;
	int next_focus_node = v1;

	while(all_visited == 0)
	{
		min = INF+1;

		all_visited = 1;

		focus_node = next_focus_node;

		for(i=0; i<(PLANE_SIZE*PLANE_SIZE); i++)
		{
			if((room->adj[focus_node][i]+distances[focus_node]) < distances[i])
			{
				distances[i] = (room->adj[focus_node][i]+distances[focus_node]);
				room->pred[v1][i] = focus_node;
			}
			
			if(visited[i] == 0 && focus_node != i && distances[i] < min)
			{
				next_focus_node = i;
				min = distances[i];
			}
			
			if(visited[i] == 0) all_visited = 0;
		}
		visited[focus_node] = 1;
	
	}

	free(distances);
}

void add_villain(int villain_start_x,int villain_start_y, room_p* room, int villain_type)
{
	vampire_p* vampire = NULL;
	ghost_p* ghost = NULL;

	if(villain_type == 1)
	{
		init_vampire(&vampire,villain_start_x,villain_start_y, "sprites/vampire_sprite_squish.bmp");
		if(room->vampire_collection == NULL) room->vampire_collection = malloc(sizeof(vampire_p*));
		else room->vampire_collection = realloc(room->vampire_collection,((room->vamp_count)+1)*sizeof(vampire_p*));
		room->vampire_collection[room->vamp_count] = vampire;
		(room->vamp_count)++;
	}else if(villain_type == 2)
	{
		init_ghost(&ghost,villain_start_x,villain_start_y, "sprites/ghost.bmp");
		if(room->ghost_collection == NULL) room->ghost_collection = malloc(sizeof(ghost_p*));
		else room->ghost_collection = realloc(room->ghost_collection,((room->ghost_count)+1)*sizeof(ghost_p*));
		room->ghost_collection[room->ghost_count] = ghost;
		(room->ghost_count)++;
	}
}


void freeRoom(room_p** room_ptr)
{
	int i;
	for(i=0; i < PLANE_SIZE; i++)
		free((*room_ptr)->plane[i]);

	SDL_FreeSurface((*room_ptr)->image);
	freeStack((*room_ptr)->forward_obs_stack);
	freeStack((*room_ptr)->backward_obs_stack);
	free((*room_ptr)->plane);
	free((*room_ptr));
}











