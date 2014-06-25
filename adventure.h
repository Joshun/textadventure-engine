#define ID_LENGTH 6
#define NAME_LENGTH 20
#define DESCRIPTION_LENGTH 500
#define MAX_ROOM_OBJECTS 10
#define MAX_ROOMS 100
#define MAX_OBJECTS 100
#define COORDINATES_LENGTH 10
#define PROMPT_LENGTH 50
#define INPUT_LENGTH 50

#define COMP_STR(str1, str2) ( strcmp(str1, str2) == 0 )
#define CLR_BUFF(buffer) ( memset(buffer, '\0', sizeof(buffer)) )
#define COMMANDS_LIST "look inventory exit quit"

typedef struct {
	char id[ID_LENGTH];
	char name[NAME_LENGTH];
	char description[DESCRIPTION_LENGTH];
	char action[ID_LENGTH]; /* What to do when object is used */
	char action_parameter[ID_LENGTH];
	int removeable;
} objectType;

typedef struct {
	char id[ID_LENGTH]; /* Unique identifier */
	int coordinates[2]; /* Coordinates on map */
	char name[NAME_LENGTH]; /* Longer name of the room */
	char description[DESCRIPTION_LENGTH]; /* Description of the room */
	char object_ids[MAX_ROOM_OBJECTS][ID_LENGTH]; /* IDs of objects present in the room */
	int n_object_ids;
	int locked;
	char locked_description[DESCRIPTION_LENGTH];
	//char dependency_object_ids[MAX_ROOM_OBJECTS][ID_LENGTH]; /* Objects the player must have to enter the room */
	//int n_dependency_object_ids;
} roomType;

typedef struct {
	int nrooms;
	roomType rooms[MAX_ROOMS];
} roomBase;

typedef struct {
	int nobjects;
	objectType objects[MAX_OBJECTS];
} objectBase;

typedef struct {
	int quit; /* Whether the game should exit */
	char prompt[PROMPT_LENGTH];
	char starting_room_id[ID_LENGTH]; /* ID of room in which the player will start */
	roomType *current_room;
	int coordinates[2]; /* The player's x and y coordinates (determining the current room) */
	char inventory[MAX_OBJECTS][ID_LENGTH]; /* Player inventory */
	int n_object_ids;
	int gold; /* Amount of money player has */
} playerType;

int debug;
