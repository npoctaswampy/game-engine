#define VAMP_W 64
#define VAMP_H 64

#define VAMP_DOWN_FRAMES 3
#define VAMP_LEFT_FRAMES 3
#define VAMP_UP_FRAMES 3
#define VAMP_RIGHT_FRAMES 3

#define VAMP_DOWN_STAND_FRAME 0
#define VAMP_LEFT_STAND_FRAME 3
#define VAMP_UP_STAND_FRAME 6
#define VAMP_RIGHT_STAND_FRAME 9

#define VAMP_SPEED 2
#define VAMP_ANIMATE_SPEED 9

#define VAMP_DAMAGE 1

#define VAMPIRE

typedef struct vampire_t {
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
} vampire_p;

void init_vampire(vampire_p** vampire, int start_x, int start_y, char* sprite_sheet);
void draw_vampire(vampire_p* vampire, SDL_Surface *screen, player_p* player, background_p* background);
void update_vampire(vampire_p* vampire, int stand_frame, int anim_frames);
void vamp_brain(vampire_p* vampire, background_p* background, Uint8 *keystate, room_p* room, player_p* player);
void vampire_hit(vampire_p* vampire);


