#define ID_LENGTH 6
#define NAME_LENGTH 20
#define DESCRIPTION_LENGTH 500
#define MAX_ROOM_OBJECTS 10
#define MAX_ROOMS 100
#define MAX_OBJECTS 100
#define COORDINATES_LENGTH 10

typedef struct {
	char id[ID_LENGTH];
	char name[NAME_LENGTH];
	char description[DESCRIPTION_LENGTH];
} objectType;

typedef struct {
	char id[ID_LENGTH]; /* Unique identifier */
	int coordinates[2]; /* Coordinates on map */
	char name[NAME_LENGTH]; /* Longer name of the room */
	char description[DESCRIPTION_LENGTH]; /* Description of the room */
	char object_ids[MAX_ROOM_OBJECTS][ID_LENGTH]; /* IDs of objects present in the room */
	int n_object_ids;
	char dependency_object_ids[MAX_ROOM_OBJECTS][ID_LENGTH]; /* Objects the player must have to enter the room */
	int n_dependency_object_ids;
} roomType;

typedef struct {
	int nrooms;
	roomType rooms[MAX_ROOMS];
} roomBase;

typedef struct {
	int nobjects;
	objectType objects[MAX_OBJECTS];
} objectBase;

enum coordinates { X_COORDINATE, Y_COORDINATE };

typedef struct {
	char starting_room_id[ID_LENGTH]; /* ID of room in which the player will start */
	enum coordinates location[2]; /* The player's x and y coordinates (determining the current room) */
	char inventory[MAX_OBJECTS][ID_LENGTH]; /* Player inventory */
	int gold; /* Amount of money player has */
} playerType;
