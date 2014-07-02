#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "adventure.h"

#define LINE_BUFFER_SIZE 1000
#define LINE_SEPARATOR "|"

void throw_error(const char *error_str);
void load_map_file(const char *mapfile, roomBase roomdb[MAX_ROOMS]);
void tokenise_room_objects(char *str, roomType *room);
void tokenise_room_dependency_objects(char *str, roomType *room);
roomType *get_room_from_coordinates(int coordinates[2], roomBase *roomdb);

void throw_error(const char *error_str)
{
	printf("Error: %s\n", error_str);
	exit(EXIT_FAILURE);
}

void tokenise_room_objects(char *str, roomType *room)
{
	int i = 0;
	char *newstr = strtok(str, ",");
	newstr[ID_LENGTH - 1] = 0;
	strncpy(room->object_ids[i], newstr, ID_LENGTH);
	i++;
	
	while( (newstr = strtok(NULL, ",")) ) {
		newstr[ID_LENGTH - 1] = 0;
		strncpy(room->object_ids[i], newstr, ID_LENGTH);
		i++;
	}
	room->n_object_ids = i;
}

void list_room_objects(roomType *room)
{
	int i;
	for(i=0; i<room->n_object_ids; i++)
		printf("Room Object ID: %s\n", room->object_ids[i]);
}

//void list_room_dependency_objects(roomType *room)
//{
	//int i;
	//for(i=0; i<room->n_dependency_object_ids; i++)
		//printf("Room Object ID: %s\n", room->dependency_object_ids[i]);
//}

void list_room_info(roomBase *roomdb)
{
	int i;
	printf("=====ROOMS LISTING=====\n");
	for(i=0; i<roomdb->nrooms; i++)
	{
		printf("ID: \"%s\"; Name \"%s\"; Description: \"%s\"\n", roomdb->rooms[i].id, roomdb->rooms[i].name, roomdb->rooms[i].description);
		printf("Locked: \"%d\"\n", roomdb->rooms[i].locked);
		if( roomdb->rooms[i].locked )
			printf("Locked description: \"%s\"\n", roomdb->rooms[i].locked_description);
		printf("Object IDs listing---\n");
		list_room_objects(&(roomdb->rooms[i]));
		printf("--end\n");
		//printf("Dependency object IDs listing---\n");
		//list_room_dependency_objects(&(roomdb->rooms[i]));
		//printf("--end\n");
	}
	printf("=====END OF ROOMS LISTING=====\n");
}

void load_map_file(const char *mapfile, roomBase *roomdb)
{
	FILE *fp = NULL;
	if ( (fp = fopen(mapfile, "r")) == NULL )
		throw_error("could not open mapfile");
		
	char line_buffer[LINE_BUFFER_SIZE] = { 0 };
	char id_buffer[LINE_BUFFER_SIZE] = { 0 };
	int coordinates_buffer[2];
	char object_ids_buffer[LINE_BUFFER_SIZE] = { 0 };
	//char dependency_object_ids_buffer[LINE_BUFFER_SIZE] = { 0 };
	int locked_buffer;	
	
	int room_index = 0;
	
	while ( fgets(line_buffer, LINE_BUFFER_SIZE, fp) != NULL ) {
		//sscanf(line_buffer, "%50[^|]|%d,%d|%50[^|]|%50[^|]", id_buffer, &coordinates_buffer[0], &coordinates_buffer[1], object_ids_buffer, dependency_object_ids_buffer);
		sscanf(line_buffer, "%50[^|]|%d,%d|%50[^|]|%d", id_buffer, &coordinates_buffer[0], &coordinates_buffer[1], object_ids_buffer, &locked_buffer);
		tokenise_room_objects(object_ids_buffer, &(roomdb->rooms[room_index]));
		//tokenise_room_dependency_objects(dependency_object_ids_buffer, &(roomdb->rooms[room_index]));
		strncpy(roomdb->rooms[room_index].id, id_buffer, ID_LENGTH);
		roomdb->rooms[room_index].coordinates[0] = coordinates_buffer[0];
		roomdb->rooms[room_index].coordinates[1] = coordinates_buffer[1];
		roomdb->rooms[room_index].locked = locked_buffer;
		
		//printf("ID: %s\n", id_buffer);
		//printf("Coordinates: %d,%d\n", coordinates_buffer[0], coordinates_buffer[1]);
		//printf("Room objects: %s\n", object_ids_buffer);
		//printf("Room dependency objects: %s\n", dependency_object_ids_buffer);
		
		
		memset(line_buffer, LINE_BUFFER_SIZE, '\0');
		memset(id_buffer, LINE_BUFFER_SIZE, '\0');
		memset(object_ids_buffer, LINE_BUFFER_SIZE, '\0');
		coordinates_buffer[0] = 0;
		coordinates_buffer[1] = 0;
		locked_buffer = 0;
				
		room_index++;
	}
	roomdb->nrooms = room_index;
}

roomType *get_room_from_id(const char *room_id, roomBase *roomdb)
{
	int i;
	for(i=0; i<roomdb->nrooms; i++)
	{
		if( (strcmp(roomdb->rooms[i].id, room_id)) == 0 ) {
			//printf("Room \"%s\" found\n", room_id);
			return &(roomdb->rooms[i]);
		}
	}
	return NULL;
}

roomType *get_room_from_coordinates(int coordinates[2], roomBase *roomdb)
{
	int i;
	for(i=0; i<roomdb->nrooms; i++)
	{
		if( (roomdb->rooms[i].coordinates[0] == coordinates[0]) && (roomdb->rooms[i].coordinates[1] == coordinates[1]) ) {
			return &(roomdb->rooms[i]);
		}
	}
	return NULL;
}

void load_rooms_file(const char *roomfile, roomBase *roomdb)
{
	FILE *fp = NULL;
	if ( (fp = fopen(roomfile, "r")) == NULL )
		throw_error("could not open roomfile");
		
	char line_buffer[LINE_BUFFER_SIZE] = { 0 };
	char id_buffer[LINE_BUFFER_SIZE] = { 0 };
	char name_buffer[LINE_BUFFER_SIZE] = { 0 };
	char description_buffer[LINE_BUFFER_SIZE] = { 0 };
	char locked_description_buffer[LINE_BUFFER_SIZE] = { 0 };
	roomType *current_room = NULL;
		
	while ( fgets(line_buffer, LINE_BUFFER_SIZE, fp) != NULL ) {
		sscanf(line_buffer, "%50[^|]|%50[^|]|%50[^|]|%50[^|\n]", id_buffer, name_buffer, description_buffer, locked_description_buffer);
		current_room = get_room_from_id(id_buffer, roomdb);
		//description_buffer[strlen(description_buffer) - 1] = 0;
		if( current_room != NULL ) {
			strncpy(current_room->name, name_buffer, NAME_LENGTH);
			strncpy(current_room->description, description_buffer, DESCRIPTION_LENGTH);
			if( current_room->locked ) {
				strncpy(current_room->locked_description, locked_description_buffer, DESCRIPTION_LENGTH);
			}			
			else {
				current_room->locked_description[0] = 0;
			}
		}
		else {
			printf("Room \"%s\" not found in map index\n", id_buffer);
			throw_error("could not load rooms");
		}
		memset(line_buffer, '\0', LINE_BUFFER_SIZE);
		memset(id_buffer, '\0', LINE_BUFFER_SIZE);
		memset(name_buffer, '\0', LINE_BUFFER_SIZE);
		memset(description_buffer, '\0', LINE_BUFFER_SIZE);
		memset(locked_description_buffer, '\0', LINE_BUFFER_SIZE);

	}
	fclose(fp);
}

void load_objects_file(const char *objectsfile, objectBase *objectdb)
{
	FILE *fp = NULL;
	if ( (fp = fopen(objectsfile, "r")) == NULL )
		throw_error("could not open objectsfile");
		
	char line_buffer[LINE_BUFFER_SIZE] = { 0 };
	char id_buffer[LINE_BUFFER_SIZE] = { 0 };
	char name_buffer[LINE_BUFFER_SIZE] = { 0 };
	char description_buffer[LINE_BUFFER_SIZE] = { 0 };
	char action_buffer[LINE_BUFFER_SIZE] = { 0 };
	char action_parameter_buffer[LINE_BUFFER_SIZE] = { 0 };
	int removeable_buffer = 0;
	int hitpoints_buffer = 0;
	
	int object_index = 0;
	
	while ( fgets(line_buffer, LINE_BUFFER_SIZE, fp) != NULL ) {
		sscanf(line_buffer, "%50[^|]|%50[^|]|%50[^|]|%d|%d|%50[^|]|%50[^|\n]", id_buffer, name_buffer, description_buffer, &removeable_buffer, &hitpoints_buffer, action_buffer, action_parameter_buffer);
		//description_buffer[strlen(description_buffer) - 1] = 0; /* Remove newline */
		strncpy(objectdb->objects[object_index].id, id_buffer, ID_LENGTH);
		strncpy(objectdb->objects[object_index].name, name_buffer, NAME_LENGTH);
		strncpy(objectdb->objects[object_index].description, description_buffer, DESCRIPTION_LENGTH);
		strncpy(objectdb->objects[object_index].action, action_buffer, ID_LENGTH);
		strncpy(objectdb->objects[object_index].action_parameter, action_parameter_buffer, ID_LENGTH);
		objectdb->objects[object_index].removeable = removeable_buffer;		
		objectdb->objects[object_index].hitpoints = hitpoints_buffer;
		
		memset(line_buffer, '\0', LINE_BUFFER_SIZE);
		memset(id_buffer, '\0', LINE_BUFFER_SIZE);
		memset(name_buffer, '\0', LINE_BUFFER_SIZE);
		memset(description_buffer, '\0', LINE_BUFFER_SIZE);
		memset(action_buffer, '\0', LINE_BUFFER_SIZE);
		memset(action_parameter_buffer, '\0', LINE_BUFFER_SIZE);
		hitpoints_buffer = 0;
		removeable_buffer = 0;
	
		object_index++;
	}
	objectdb->nobjects = object_index;
	fclose(fp);
}

objectType *get_object_from_id(const char *object_id, objectBase *objectdb)
{
	int i;
	for(i=0; i<objectdb->nobjects; i++)
	{
		if( (strcmp(objectdb->objects[i].id, object_id)) == 0 ) {
			//printf("Room \"%s\" found\n", room_id);
			return &(objectdb->objects[i]);
		}
	}
	return NULL;
}

objectType *get_object_from_name(const char *object_name, objectBase *objectdb)
{
	int i;
	for(i=0; i<objectdb->nobjects; i++)
	{
		if( (strcmp(objectdb->objects[i].name, object_name)) == 0 ) {
			//printf("Room \"%s\" found\n", room_id);
			return &(objectdb->objects[i]);
		}
	}
	return NULL;
}

int object_in_room(objectType *object, roomType *room)
{
	int i;
	for(i=0; i<room->n_object_ids; i++)
	{
		if( strcmp(room->object_ids[i], object->id) == 0)
			return 1;
	}
	return 0;
}

int object_in_inventory(objectType *object, playerType *player)
{
	int i;
	for(i=0; i<player->n_object_ids; i++)
	{
		if( strcmp(player->inventory[i], object->id) == 0)
			return 1;
	}
	return 0;
}

void remove_object_from_room(objectType *object, roomType *room)
{
	if( object_in_room(object, room) )
	{
		int i, j;
		for(i=0; i<room->n_object_ids; i++)
		{
			if( strcmp(room->object_ids[i], object->id) == 0)
			{
				//printf("Removing object with id: %s\n", room->object_ids[i]);
				for(j=i; (j+1)<room->n_object_ids; j++) {
					//printf("ID: %s\n", room->object_ids[j]);
					strncpy(room->object_ids[j], room->object_ids[j+1], ID_LENGTH);
				}
				room->n_object_ids -= 1;
				return;
			}
		}
	}
}

void list_object_info(objectBase *objectdb)
{
	int i;
	printf("=====OBJECTS LISTING=====\n");
	for(i=0; i<objectdb->nobjects; i++)
	{
		printf("ID: \"%s\"; Name \"%s\"; Description \"%s\"; Removeable \"%d\"; Hitpoints \"%d\"\n", objectdb->objects[i].id, objectdb->objects[i].name, objectdb->objects[i].description, objectdb->objects[i].removeable, objectdb->objects[i].hitpoints); 
		printf("Object action: \"%s\"; Action parameter: \"%s\"\n", objectdb->objects[i].action, objectdb->objects[i].action_parameter);
	}
	printf("=====END OF OBJECTS LISTING=====\n");

}


void load_config_file(const char *configfile, playerType *config)
{
	extern int debug;
	FILE *fp = NULL;
	if( (fp = fopen(configfile, "r")) == NULL )
		throw_error("could not load config file");
	
	//char id_buffer[LINE_BUFFER_SIZE] = { 0 };
	//fscanf(fp, "default:%s", id_buffer);
	
	char line_buffer[LINE_BUFFER_SIZE] = { 0 };
	char param1[LINE_BUFFER_SIZE] = { 0 };
	char param2[LINE_BUFFER_SIZE] = { 0 };
	
	while( fgets(line_buffer, LINE_BUFFER_SIZE, fp) != NULL ) {
		sscanf(line_buffer, "%[^:]:%[^\n]", param1, param2);
		if( strcmp(param1, "sroom") == 0) {
			if(debug) printf("Setting default room to \"%s\"\n", param2);
			strncpy(config->starting_room_id, param2, ID_LENGTH);
		}
		if( strcmp(param1, "prompt") == 0) {
			if(debug) printf("Setting default prompt to \"%s\"\n", param2);
			strncpy(config->prompt, param2, PROMPT_LENGTH);
			if(debug) printf("Prompt: %s\n", config->prompt);
		}
		memset(param1, '\0', LINE_BUFFER_SIZE);
		memset(param2, '\0', LINE_BUFFER_SIZE);
	}
	fclose(fp);
}
