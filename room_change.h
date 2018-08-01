#define ROOMCHANGE

typedef struct door_t {
	int room_num;
	int player_start_x;
	int player_start_y;
} door_p;

door_p* checkForRoomChange(animation_type direction, int player_x, int player_y, room_p* room);
