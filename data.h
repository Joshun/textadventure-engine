void load_map_file(const char *mapfile, roomBase roomdb[MAX_ROOMS]);
void load_rooms_file(const char *roomfile, roomBase *roomdb);
void load_objects_file(const char *objectsfile, objectBase *objectdb);
void list_room_objects(roomType *room, objectBase *objectdb);
roomType *get_room_from_id(const char *room_id, roomBase *roomdb);
roomType *get_room_from_coordinates(int coordinates[2], roomBase *roomdb);
objectType *get_object_from_id(const char *object_id, objectBase *objectdb);
