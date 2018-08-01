#define COLLISION

int checkForCollisions(animation_type direction, background_p* background, player_p* player, room_p* room);
int checkPerimeter(animation_type direction, background_p* background, player_p* player);
int check_tiles(room_p* room, player_p* player, animation_type direction);
