 
int HP_CreateFile(char *fileName);
int HP_OpenFile(char* fileName);
int HP_CloseFile(int fileDesc);
void HP_GetAllEntries(int fileDesc, char* fieldName, void* value);
int HP_DeleteEntry(int fileDesc, char* fieldName, void* value);

