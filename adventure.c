#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "adventure.h"
#include "data.h"
#include "player.h"

#define PATH_BUF_SIZE 100

void parse_args(int argc, char **argv, char *dpathbuf)
{
	debug = 0;
	
	int i;
	for(i=0; i<argc; i++) {
		if( COMP_STR(argv[i], "--debug" ) ) {
			printf("Enabling debug mode\n");
			debug = 1;
		}
		else if( COMP_STR(argv[i], "--data-path" ) ) {
			if( argc > (i+1) ) {
				printf("Setting data path to %s\n", argv[i+1]);
				strncpy(dpathbuf, argv[i+1], PATH_BUF_SIZE);
			}
			else {
				printf("Error: path location invalid\n");
				exit(EXIT_FAILURE);
			}
		}
	}
}
int main(int argc, char **argv)
{
	char datapath_buffer[PATH_BUF_SIZE] = { 0 };
	parse_args(argc, argv, datapath_buffer);
	if( strlen(datapath_buffer) > 0 ) {
		if( chdir(datapath_buffer) == -1 ) {
			printf("Error: data path does not exist\n");
			exit(EXIT_FAILURE);
		}
	}
	
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

