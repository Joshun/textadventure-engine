#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "adventure.h"
#include "data.h"
#include "player.h"

void parse_args(int argc, char **argv)
{
	debug = 0;
	
	int i;
	for(i=0; i<argc; i++) {
		if( COMP_STR(argv[i], "--debug" ) ) {
			printf("Enabling debug mode\n");
			debug = 1;
		}
	}
}
int main(int argc, char **argv)
{
	parse_args(argc, argv);
	
	roomBase rooms;
	objectBase objects;
	playerType player;
	load_map_file("datafiles/map.data", &rooms);
	//list_room_objects(&(rooms.rooms[0]));
	//list_room_dependency_objects(&(rooms.rooms[1]));
	load_rooms_file("datafiles/rooms.data", &rooms);
	load_objects_file("datafiles/objects.data", &objects);
	load_config_file("datafiles/config.data", &player);
	
	if( debug ) {
		list_room_info(&rooms);
		list_object_info(&objects);
	}
	
	start_game(&player, &rooms, &objects);
	
	return 0;
}

