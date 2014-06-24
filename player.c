#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "adventure.h"
#include "data.h"

#define COMP_STR(str1, str2) ( strcmp(str1, str2) == 0 )
#define COMMANDS_LIST "look inventory exit quit"

void set_player_coordinates(int coordinates[2], playerType *player);
void show_room_content(roomType *room, objectBase *objectdb);
void show_inventory(playerType *player, objectBase *objectdb);
void go_to_room(playerType *player, roomBase *roomdb, int coordinates[2], objectBase *objectdb);
void take_object(const char *object_name, playerType *player, objectBase *objectdb);
void no_entry(void);

void start_game(playerType *player, roomBase *roomdb, objectBase *objectdb)
{
	player->quit = 0;
	player->n_object_ids = 0;
	int test_coordinates[2] = { 0, 0 };
	char input_buffer[INPUT_LENGTH] = { 0 };
	player->current_room = get_room_from_id(player->starting_room_id, roomdb);
	set_player_coordinates(player->current_room->coordinates, player);
	show_room_content(player->current_room, objectdb);
	
	while( ! player->quit )
	{
		printf("%s ", player->prompt);
		fgets(input_buffer, INPUT_LENGTH, stdin);
		input_buffer[strlen(input_buffer) - 1] = 0; /* Strip newline character */
		//take_object("Spigot", player, objectdb);
		
		if( COMP_STR(input_buffer, "exit") || COMP_STR(input_buffer, "quit") || feof(stdin) ) {
			printf("Thanks for playing, have a nice day!\n");
			player->quit = 1;
		}
		else if( COMP_STR(input_buffer, "look")	) {
			show_room_content(player->current_room, objectdb);
		}
		
		else if( COMP_STR(input_buffer, "inventory" ) ) {
			show_inventory(player, objectdb);
		}
		else if( COMP_STR(input_buffer, "north" ) ) {
			test_coordinates[0] = player->coordinates[0];
			test_coordinates[1] = player->coordinates[1] + 1;
			go_to_room(player, roomdb, test_coordinates, objectdb);
		}
		else if( COMP_STR(input_buffer, "south" ) ) {
			test_coordinates[0] = player->coordinates[0];
			test_coordinates[1] = player->coordinates[1] - 1;
			go_to_room(player, roomdb, test_coordinates, objectdb);
		}
		else if( COMP_STR(input_buffer, "east" ) ) {
			test_coordinates[0] = player->coordinates[0] + 1;
			test_coordinates[1] = player->coordinates[1];
			go_to_room(player, roomdb, test_coordinates, objectdb);
		}
		else if( COMP_STR(input_buffer, "west" ) ) {
			test_coordinates[0] = player->coordinates[0] - 1;
			test_coordinates[1] = player->coordinates[1];
			go_to_room(player, roomdb, test_coordinates, objectdb);
		}
		else if( COMP_STR(input_buffer, "help" ) ) {
			printf("Commands: %s\n", COMMANDS_LIST);
		}
		else {
			if( strlen(input_buffer) > 0 ) /* check that the user hasn't simply just pressed enter */
				printf("I don't understand that. Type \"help\" for a list of recognised commands\n");
		}
	}
		
}

void set_player_coordinates(int coordinates[2], playerType *player)
{
	player->coordinates[0] = coordinates[0];
	player->coordinates[1] = coordinates[1];
}

void show_room_content(roomType *room, objectBase *objectdb)
{
	if( room->locked ) {
		printf("You cannot enter the %s.\n", room->name);
		printf("%s\n", room->locked_description);
		return;
	}
	printf("You are in the %s.\n%s\n", room->name, room->description);
	printf("You see the following objects:\n");
	int i;
	objectType *current_object;
	for(i=0; i<room->n_object_ids; i++) {
		current_object = get_object_from_id(room->object_ids[i], objectdb);
		if( current_object ) {
			if( (i + 1) != room->n_object_ids )
				printf("%s, ", current_object->name);
			else
				printf("%s\n", current_object->name);
		}
	}
}

void show_inventory(playerType *player, objectBase *objectdb)
{
	if( player->n_object_ids == 0 )
		printf("You are carrying nothing\n");
	else {
		printf("You are carring the following objects:\n");
		int i;
		objectType *current_object;
		for(i=0; i<player->n_object_ids; i++) {
			current_object = get_object_from_id(player->inventory[i], objectdb);
			if( current_object ) {
				if( (i + 1) != player->n_object_ids )
					printf("%s, ", current_object->name);
				else
					printf("%s\n", current_object->name);
			}
		}
	}
}

void go_to_room(playerType *player, roomBase *roomdb, int coordinates[2], objectBase *objectdb)
{
	roomType *test_room = NULL;
	test_room = get_room_from_coordinates(coordinates, roomdb);
	if( test_room ) {
		if( ! test_room->locked ) {
			set_player_coordinates(coordinates, player);
			player->current_room = test_room;
		}
		show_room_content(test_room, objectdb);
	}
	else
		no_entry();
}

void no_entry(void)
{
	printf("You cannot go there.\n");
}

void take_object(const char *object_name, playerType *player, objectBase *objectdb)
{
	objectType *temp_object = get_object_from_name(object_name, objectdb);
	if( temp_object == NULL )
	{
		printf("There is no such object\n");
	}
	else if( object_in_inventory(temp_object, player) ) {
		printf("You already have the %s, stupid!\n", object_name);
	}
	else if( ! object_in_room(temp_object, player->current_room) ) {
		printf("There is no such object\n");
	}
	else if( ! temp_object->removeable ) {
		printf("You cannot pick up the %s\n", object_name);
	}
	else {
		printf("You take the %s\n", temp_object->name);
		strncpy(player->inventory[player->n_object_ids], temp_object->id, ID_LENGTH);
		player->n_object_ids += 1;
		remove_object_from_room(temp_object, player->current_room);
	}
}
