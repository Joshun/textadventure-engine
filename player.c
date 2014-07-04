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
void go_to_room(playerType *player, roomBase *roomdb, int coordinates[2], objectBase *objectdb, enemyBase *enemydb);
void take_object(const char *object_name, playerType *player, objectBase *objectdb);
void no_entry(void);
void examine_object(char *object_name, playerType *player, objectBase *objectdb);
void use_object(char *object_name, playerType *player, objectBase *objectdb, roomBase *roomdb);
void start_enemy_battle(playerType *player, objectBase *objectdb, enemyBase *enemydb);
void heal_player(playerType *player, int hpoints);

int player_input(playerType *player, char *input_command, char *input_parameter);

void start_game(playerType *player, roomBase *roomdb, objectBase *objectdb, enemyBase *enemydb)
{
	player->quit = 0;
	player->n_object_ids = 0;
	player->health = 100;
	int test_coordinates[2] = { 0, 0 };
	int input_size = 0;
	char input_command[INPUT_LENGTH] = { 0 };
	char input_parameter[INPUT_LENGTH] = { 0 };
	player->current_room = get_room_from_id(player->starting_room_id, roomdb);
	set_player_coordinates(player->current_room->coordinates, player);
	show_room_content(player->current_room, objectdb);
		
	while( ! player->quit )
	{
		//printf("command=\"%s\" parameter=\"%s\"\n", input_command, input_parameter);
		//take_object("Spigot", player, objectdb);
		input_size = player_input(player, input_command, input_parameter);
		
		if( COMP_STR(input_command, "exit") || COMP_STR(input_command, "quit") || feof(stdin) ) {
			printf("Thanks for playing, have a nice day!\n");
			player->quit = 1;
		}
		else if( COMP_STR(input_command, "look")	) {
			show_room_content(player->current_room, objectdb);
		}
		else if( COMP_STR(input_command, "health") ) {
			printf("Player HP: %d\n", player->health);
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
			go_to_room(player, roomdb, test_coordinates, objectdb, enemydb);
		}
		else if( COMP_STR(input_command, "south" ) ) {
			test_coordinates[0] = player->coordinates[0];
			test_coordinates[1] = player->coordinates[1] - 1;
			go_to_room(player, roomdb, test_coordinates, objectdb, enemydb);
		}
		else if( COMP_STR(input_command, "east" ) ) {
			test_coordinates[0] = player->coordinates[0] + 1;
			test_coordinates[1] = player->coordinates[1];
			go_to_room(player, roomdb, test_coordinates, objectdb, enemydb);
		}
		else if( COMP_STR(input_command, "west" ) ) {
			test_coordinates[0] = player->coordinates[0] - 1;
			test_coordinates[1] = player->coordinates[1];
			go_to_room(player, roomdb, test_coordinates, objectdb, enemydb);
		}
		else if( COMP_STR(input_command, "help" ) ) {
			printf("Commands: %s\n", COMMANDS_LIST);
		}
		else {
			if( input_size > 0 ) /* check that the user hasn't simply just pressed enter */
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
	if( room->n_object_ids > 0 ) {
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

void go_to_room(playerType *player, roomBase *roomdb, int coordinates[2], objectBase *objectdb, enemyBase *enemydb)
{
	roomType *test_room = NULL;
	test_room = get_room_from_coordinates(coordinates, roomdb);
	if( test_room ) {
		if( ! test_room->locked ) {
			set_player_coordinates(coordinates, player);
			player->current_room = test_room;
		}
		show_room_content(test_room, objectdb);
		if( test_room->n_enemy_ids > 0 )
			start_enemy_battle(player, objectdb, enemydb);
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

void heal_player(playerType *player, int hpoints)
{
	int heal_amount;
	if( (heal_amount = player->health + hpoints) > HEALTH_MAX )
		player->health = HEALTH_MAX;
	else
		player->health += heal_amount;
	
	printf("The object heals you: Player HP %d\n", player->health);
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
		else if( COMP_STR(temp_object->action, "HEAL") ) {
			heal_player(player, atoi(temp_object->action_parameter));
			remove_object_from_inventory(temp_object, player);
		}
		else {
			printf("Error: invalid object action \"%s\"\n", temp_object->action);
		}
	}
	else {
		printf("You do not have the %s.\n", object_name);
	}
	
}

int enemy_dead(enemyType *enemy)
{
	if( enemy->health <= 0 )
		return 1;
	else
		return 0;
}

void start_enemy_battle(playerType *player, objectBase *objectdb, enemyBase *enemydb)
{
	roomType *fighting_ground = player->current_room;
	enemyType *current_enemy = NULL;
	objectType *temp_object = NULL;
	
	char input_command[INPUT_LENGTH] = { 0 };
	char input_parameter[INPUT_LENGTH] = { 0 };
	int input_size = 0;
	
	int i;
	for(i=0; i<fighting_ground->n_enemy_ids; i++)
	{
		current_enemy = get_enemy_from_id(fighting_ground->enemy_ids[i], enemydb);
		printf("A %s approaches you for a fight.\n", current_enemy->name);
		printf("It is %s\n", current_enemy->description);
		while( ! enemy_dead(current_enemy) ) {
			printf("Player HP: %d | Enemy HP %d\n", player->health, current_enemy->health);
			input_size = player_input(player, input_command, input_parameter);
			if( COMP_STR(input_command, "exit") || COMP_STR(input_command, "quit") || feof(stdin) ) {
				printf("Thanks for playing, have a nice day!\n");
				player->quit = 1;
				goto LEAVE_BATTLE;
			}
			if( COMP_STR(input_command, "help" ) ) {
				printf("Commands: use pass look exit\n");
				goto SKIP_DAMAGE;
			}
			if( COMP_STR(input_command, "look" ) ) {
				printf("You are in a battle with a %s\n", current_enemy->name);
				printf("It is %s\n", current_enemy->description);
				goto SKIP_DAMAGE;
			}
			else if( COMP_STR(input_command, "use" ) ) {
				temp_object = get_object_from_name(input_parameter, objectdb);
				if( temp_object != NULL && object_in_inventory(temp_object, player) ) {
					if( temp_object->hitpoints > 0 )
					{
						printf("You attack the %s with the %s, doing some damage...\n", current_enemy->name, temp_object->name);
						current_enemy->health -= temp_object->hitpoints;
						if( enemy_dead(current_enemy) )
							goto SKIP_DAMAGE;
					}
					else
						printf("It has no effect\n");
				}
				else {
					printf("You don\'t have a %s\n", input_parameter);
					goto SKIP_DAMAGE;
				}
			}
			else if( COMP_STR(input_command, "pass" ) ) {
				printf("You do nothing at your peril\n");
			}
			else {
				if( input_size > 0 ) /* check that the user hasn't simply just pressed enter */
					printf("I don't understand that. Type \"help\" for a list of recognised commands\n");
					goto SKIP_DAMAGE;
			}
			
			player->health -= current_enemy->damage;
			SKIP_DAMAGE: ;
			
			if( player->health <= 0 )
			{
				printf("You have died!\n");
				player->quit = 1;
				goto LEAVE_BATTLE;
			}
		}
		printf("You have killed the %s!\n", current_enemy->name);
	}
	
	LEAVE_BATTLE:
	return;
}

int player_input(playerType *player, char *input_command, char *input_parameter)
{
	CLR_BUFF(input_command);
	CLR_BUFF(input_parameter);
	char input_buffer[INPUT_LENGTH] = { 0 };
	
	printf("%s ", player->prompt);
	fgets(input_buffer, INPUT_LENGTH, stdin);
	make_input_lowercase(input_buffer);
	input_buffer[strlen(input_buffer) - 1] = 0; /* Strip newline character */
	sscanf(input_buffer, "%50s%*[' ']%50[^\t\n]", input_command, input_parameter);
	return strlen(input_buffer);
}
