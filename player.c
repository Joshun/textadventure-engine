#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "adventure.h"
#include "data.h"
#include "colourterm.h"

void make_input_lowercase(char input_buffer[INPUT_LENGTH]);
void set_player_coordinates(int coordinates[2], playerType *player);
void show_room_content(roomType *room, objectBase *objectdb);
void show_inventory(playerType *player, objectBase *objectdb);
void go_to_room(playerType *player, roomBase *roomdb, int coordinates[2], objectBase *objectdb);
void take_object(const char *object_name, playerType *player, objectBase *objectdb);
void no_entry(void);
void examine_object(char *object_name, playerType *player, objectBase *objectdb);

void use_object(char *object_name, playerType *player, objectBase *objectdb, roomBase *roomdb);

void start_game(playerType *player, roomBase *roomdb, objectBase *objectdb)
{
	player->quit = 0;
	player->n_object_ids = 0;
	int test_coordinates[2] = { 0, 0 };
	char input_buffer[INPUT_LENGTH] = { 0 };
	char input_command[INPUT_LENGTH] = { 0 };
	char input_parameter[INPUT_LENGTH] = { 0 };
	player->current_room = get_room_from_id(player->starting_room_id, roomdb);
	set_player_coordinates(player->current_room->coordinates, player);
	show_room_content(player->current_room, objectdb);
	
	while( ! player->quit )
	{
		printf("%s ", player->prompt);
		fgets(input_buffer, INPUT_LENGTH, stdin);
		make_input_lowercase(input_buffer);
		input_buffer[strlen(input_buffer) - 1] = 0; /* Strip newline character */
		sscanf(input_buffer, "%50s%*[' ']%50[^\t\n]", input_command, input_parameter);
		//printf("command=\"%s\" parameter=\"%s\"\n", input_command, input_parameter);
		//take_object("Spigot", player, objectdb);
		
		if( COMP_STR(input_command, "exit") || COMP_STR(input_buffer, "quit") || feof(stdin) ) {
			printf("Thanks for playing, have a nice day!\n");
			player->quit = 1;
		}
		else if( COMP_STR(input_command, "look")	) {
			show_room_content(player->current_room, objectdb);
		}
		
		else if( COMP_STR(input_command, "inventory" ) ) {
			show_inventory(player, objectdb);
		}
		else if( COMP_STR(input_command, "examine" ) ) {
			examine_object(input_parameter, player, objectdb);
		}
		else if( COMP_STR(input_command, "take" ) ) {
			take_object(input_parameter, player, objectdb);
		}
		else if( COMP_STR(input_command, "use" ) ) {
			use_object(input_parameter, player, objectdb, roomdb);
		}
		else if( COMP_STR(input_command, "north" ) ) {
			test_coordinates[0] = player->coordinates[0];
			test_coordinates[1] = player->coordinates[1] + 1;
			go_to_room(player, roomdb, test_coordinates, objectdb);
		}
		else if( COMP_STR(input_command, "south" ) ) {
			test_coordinates[0] = player->coordinates[0];
			test_coordinates[1] = player->coordinates[1] - 1;
			go_to_room(player, roomdb, test_coordinates, objectdb);
		}
		else if( COMP_STR(input_command, "east" ) ) {
			test_coordinates[0] = player->coordinates[0] + 1;
			test_coordinates[1] = player->coordinates[1];
			go_to_room(player, roomdb, test_coordinates, objectdb);
		}
		else if( COMP_STR(input_command, "west" ) ) {
			test_coordinates[0] = player->coordinates[0] - 1;
			test_coordinates[1] = player->coordinates[1];
			go_to_room(player, roomdb, test_coordinates, objectdb);
		}
		else if( COMP_STR(input_command, "help" ) ) {
			printf("Commands: %s\n", COMMANDS_LIST);
		}
		else {
			if( strlen(input_buffer) > 0 ) /* check that the user hasn't simply just pressed enter */
				printf("I don't understand that. Type \"help\" for a list of recognised commands\n");
		}
		CLR_BUFF(input_buffer);
		CLR_BUFF(input_command);
		CLR_BUFF(input_parameter);
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
	if( strlen(object_name) < 1) {
		printf("Take what?\n");
		return;
	}
	
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
		printf("You take the %s\n", object_name);
		strncpy(player->inventory[player->n_object_ids], temp_object->id, ID_LENGTH);
		player->n_object_ids += 1;
		remove_object_from_room(temp_object, player->current_room);
	}
}

void make_input_lowercase(char input_buffer[INPUT_LENGTH])
{
	int i;
	for(i=0; i<INPUT_LENGTH; i++)
	{
		if( input_buffer[i] == '\0' || input_buffer[i] == '\n' )
			break;
		else {
			if( isupper(input_buffer[i]) )
				input_buffer[i] = tolower(input_buffer[i]);
		}
	}
}

void examine_object(char *object_name, playerType *player, objectBase *objectdb)
{
	if( strlen(object_name) < 1 ) {
		printf("Examine what?\n");
		return;
	}
	
	objectType *temp_object = NULL;
	temp_object = get_object_from_name(object_name, objectdb);
	if( temp_object == NULL ) {
		printf("There is no such object\n");
		return;
	}
	else if( object_in_room(temp_object, player->current_room) || object_in_inventory(temp_object, player) ) {
		printf("%s\n", temp_object->description);
	}
	else {
		printf("There is no such object\n");
	}
}

void unlock_room(char *room_id, roomBase *roomdb)
{
	roomType *temp_room = get_room_from_id(room_id, roomdb);
	if( temp_room )
		temp_room->locked = 0;
	else
		printf("Error: room with ID \"%s\" could not be found\n", room_id);
}

void use_object(char *object_name, playerType *player, objectBase *objectdb, roomBase *roomdb)
{
	if( strlen(object_name) < 1 ) {
		printf("Use what?\n");
		return;
	}
	
	objectType *temp_object = get_object_from_name(object_name, objectdb);
	
	if( temp_object == NULL ) {
		printf("There is no such object\n");
		return;
	}
	else if( object_in_inventory(temp_object, player) ) {
		if( temp_object->action[0] == '0' ) {
			printf("Nothing happens.\n");
		}
		else if( COMP_STR(temp_object->action, "ULOCK") ) {
			printf("The %s is now accessible.\n", player->current_room->name);
			unlock_room(temp_object->action_parameter, roomdb);
		}
		else {
			printf("Error: invalid object action \"%s\"\n", temp_object->action);
		}
	}
	else {
		printf("You do not have the %s.\n", object_name);
	}
	
}
	
