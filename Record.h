typedef struct FileInformation{
        char file_name[20];
        char file_type[20];
	int number_of_all_records;		/*boi8a sthn taxyterh prospelash*/
	int number_of_blocks_in_use;
} FileInformation;

typedef struct Record{
        int id;
        char name[15];
        char surname[20];
        char city[10];
}Record;

typedef struct BlockInfo{
	int number_of_records;
	int next_block;			/*sthn ylopoihsh den mas xrisimevoun alla*/
	int previous_block;
}BlockInfo;
