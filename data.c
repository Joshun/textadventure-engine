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

void tokenise_room_dependency_objects(char *str, roomType *room)
{
	int i = 0;
	char *newstr = strtok(str, ",");
	newstr[ID_LENGTH - 1] = 0;
	strncpy(room->dependency_object_ids[i], newstr, ID_LENGTH);
	i++;
	
	while( (newstr = strtok(NULL, ",")) ) {
		newstr[ID_LENGTH - 1] = 0;
		strncpy(room->dependency_object_ids[i], newstr, ID_LENGTH);
		i++;
	}
	room->n_dependency_object_ids = i;
}

void list_room_objects(roomType *room)
{
	int i;
	for(i=0; i<room->n_object_ids; i++)
		printf("Room Object ID: %s\n", room->object_ids[i]);
}

void list_room_dependency_objects(roomType *room)
{
	int i;
	for(i=0; i<room->n_dependency_object_ids; i++)
		printf("Room Object ID: %s\n", room->dependency_object_ids[i]);
}

void list_room_info(roomBase *roomdb)
{
	int i;
	printf("=====ROOMS LISTING=====\n");
	for(i=0; i<roomdb->nrooms; i++)
	{
		printf("ID: \"%s\"; Name \"%s\"; Description: \"%s\"\n", roomdb->rooms[i].id, roomdb->rooms[i].name, roomdb->rooms[i].description);
		printf("Object IDs listing---\n");
		list_room_objects(&(roomdb->rooms[i]));
		printf("--end\n");
		printf("Dependency object IDs listing---\n");
		list_room_dependency_objects(&(roomdb->rooms[i]));
		printf("--end\n");
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
	char dependency_object_ids_buffer[LINE_BUFFER_SIZE] = { 0 };
	
	
	int room_index = 0;
	
	while ( fgets(line_buffer, LINE_BUFFER_SIZE, fp) != NULL ) {
		sscanf(line_buffer, "%50[^|]|%d,%d|%50[^|]|%50[^|]", id_buffer, &coordinates_buffer[0], &coordinates_buffer[1], object_ids_buffer, dependency_object_ids_buffer);
		tokenise_room_objects(object_ids_buffer, &(roomdb->rooms[room_index]));
		tokenise_room_dependency_objects(dependency_object_ids_buffer, &(roomdb->rooms[room_index]));
		strncpy(roomdb->rooms[room_index].id, id_buffer, ID_LENGTH);
		roomdb->rooms[room_index].coordinates[0] = coordinates_buffer[0];
		roomdb->rooms[room_index].coordinates[1] = coordinates_buffer[1];
		
		//printf("ID: %s\n", id_buffer);
		//printf("Coordinates: %d,%d\n", coordinates_buffer[0], coordinates_buffer[1]);
		//printf("Room objects: %s\n", object_ids_buffer);
		//printf("Room dependency objects: %s\n", dependency_object_ids_buffer);
		
		
		memset(line_buffer, LINE_BUFFER_SIZE, '\0');
		memset(id_buffer, LINE_BUFFER_SIZE, '\0');
		memset(object_ids_buffer, LINE_BUFFER_SIZE, '\0');
		memset(dependency_object_ids_buffer, LINE_BUFFER_SIZE, '\0');
		
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

void load_rooms_file(const char *roomfile, roomBase *roomdb)
{
	FILE *fp = NULL;
	if ( (fp = fopen(roomfile, "r")) == NULL )
		throw_error("could not open roomfile");
		
	char line_buffer[LINE_BUFFER_SIZE] = { 0 };
	char id_buffer[LINE_BUFFER_SIZE] = { 0 };
	char name_buffer[LINE_BUFFER_SIZE] = { 0 };
	char description_buffer[LINE_BUFFER_SIZE] = { 0 };
	roomType *current_room = NULL;
		
	while ( fgets(line_buffer, LINE_BUFFER_SIZE, fp) != NULL ) {
		sscanf(line_buffer, "%50[^|]|%50[^|]|%50[^|]", id_buffer, name_buffer, description_buffer);
		current_room = get_room_from_id(id_buffer, roomdb);
		description_buffer[strlen(description_buffer) - 1] = 0;
		if( current_room != NULL ) {
			strncpy(current_room->name, name_buffer, NAME_LENGTH);
			strncpy(current_room->description, description_buffer, DESCRIPTION_LENGTH);
		}
		else {
			printf("Room \"%s\" not found in map index\n", id_buffer);
			throw_error("could not load rooms");
		}
		memset(line_buffer, '\0', LINE_BUFFER_SIZE);
		memset(id_buffer, '\0', LINE_BUFFER_SIZE);
		memset(name_buffer, '\0', LINE_BUFFER_SIZE);
		memset(description_buffer, '\0', LINE_BUFFER_SIZE);
	}
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
	
	int object_index = 0;
	
	while ( fgets(line_buffer, LINE_BUFFER_SIZE, fp) != NULL ) {
		sscanf(line_buffer, "%50[^|]|%50[^|]|%50[^|]", id_buffer, name_buffer, description_buffer);
		description_buffer[strlen(description_buffer) - 1] = 0; /* Remove newline */
		strncpy(objectdb->objects[object_index].id, id_buffer, ID_LENGTH);
		strncpy(objectdb->objects[object_index].name, name_buffer, NAME_LENGTH);
		strncpy(objectdb->objects[object_index].description, description_buffer, DESCRIPTION_LENGTH);
		
		memset(line_buffer, '\0', LINE_BUFFER_SIZE);
		memset(id_buffer, '\0', LINE_BUFFER_SIZE);
		memset(name_buffer, '\0', LINE_BUFFER_SIZE);
		memset(description_buffer, '\0', LINE_BUFFER_SIZE);
	
		object_index++;
	}
	objectdb->nobjects = object_index;
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
void list_object_info(objectBase *objectdb)
{
	int i;
	printf("=====OBJECTS LISTING=====\n");
	for(i=0; i<objectdb->nobjects; i++)
	{
		printf("ID: \"%s\"; Name \"%s\"; Description \"%s\"\n", objectdb->objects[i].id, objectdb->objects[i].name, objectdb->objects[i].description); 

	}
	printf("=====END OF OBJECTS LISTING=====\n");

}

int main()
{
	roomBase rooms;
	objectBase objects;
	load_map_file("datafiles/map.data", &rooms);
	//list_room_objects(&(rooms.rooms[0]));
	//list_room_dependency_objects(&(rooms.rooms[1]));
	load_rooms_file("datafiles/rooms.data", &rooms);
	list_room_info(&rooms);
	load_objects_file("datafiles/objects.data", &objects);
	list_object_info(&objects);
	
	return 0;
}
