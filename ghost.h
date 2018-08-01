#define GHOST_W 64
#define GHOST_H 64

#define GHOST_DOWN_FRAMES 3
#define GHOST_LEFT_FRAMES 3
#define GHOST_UP_FRAMES 3
#define GHOST_RIGHT_FRAMES 3

#define GHOST_DOWN_STAND_FRAME 0
#define GHOST_LEFT_STAND_FRAME 3
#define GHOST_UP_STAND_FRAME 6
#define GHOST_RIGHT_STAND_FRAME 9

#define GHOST_SPEED 2
#define GHOST_ANIMATE_SPEED 12

#define GHOST_DAMAGE 2

#define GHOST

typedef struct ghost_t {
	int type;
	animation_type animation;
	int frame;
	int stand_frame;
	int screen_x;
	int screen_y;
	int map_x;
	int map_y;
	int updates;
	SDL_Surface *image;
	int last_attack;	
	animation_type direction;
} ghost_p;

void init_ghost(ghost_p** ghost, int start_x, int start_y, char* sprite_sheet);
void draw_ghost(ghost_p* ghost, SDL_Surface *screen, player_p* player, background_p* background);
void update_ghost(ghost_p* ghost, int stand_frame, int anim_frames);
void ghost_brain(ghost_p* ghost, background_p* background, Uint8 *keystate, room_p* room, player_p* player);


