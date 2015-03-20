#include <string.h>
#include "BF.h"
#include <stdio.h>
#include <stdlib.h>
#include "Record.h"
#include "HP.h"



int HP_CreateFile(char *fileName)

{
	int temp,filenumber;
	FileInformation file_info;
	void *for_insert;

	temp = BF_CreateFile(fileName);
	if(temp == 0){
		filenumber = BF_OpenFile(fileName);
		if(filenumber >= 0){
			temp = BF_AllocateBlock(filenumber);
			if(temp == 0){
				temp = BF_ReadBlock(filenumber,0,&for_insert);
				if(temp == 0){

					strcpy(file_info.file_name,fileName);
					strcpy(file_info.file_type,"Heap_File");

					file_info.number_of_blocks_in_use = 1;
					file_info.number_of_all_records = 0;

					memcpy(for_insert,&file_info,sizeof(FileInformation));
					temp = BF_WriteBlock(filenumber,0);
					if(temp != 0){
						BF_PrintError("Writing Block Error! ");
						 return -1;
					}
					temp = BF_CloseFile(filenumber);
					if(temp != 0){
						BF_PrintError("Closing File Error! ");
						return -1;
					}
					return 0;   //**success exit**//
				}
				else{
					BF_PrintError("Reading Block Error! ");
					return -1;
				}
			}
			else{
				BF_PrintError("Allocating  Block Error! ");
				return -1;
			}
		}
		else{
			BF_PrintError("Opening File Error! ");
			return -1;
		}
	}
	else{
		BF_PrintError("Creating File Error! ");
		return -1;
	}
}

int HP_OpenFile(char* fileName)

{
        int temp,filenumber;
        FileInformation file_info;
	void *for_read;

	filenumber = BF_OpenFile(fileName);

	if(filenumber >= 0){
		temp = BF_ReadBlock(filenumber,0,&for_read);
		if(temp == 0){
			if( strcmp( ((FileInformation*)for_read )->file_type , "Heap_File" ) == 0 ){

				return filenumber;	//**success exit**//
			}
			else{
				BF_CloseFile(filenumber);
				printf("The File you are trying to open is Not a Heap File!\n");
				return -1;
			}
		}
		else{
			BF_CloseFile(filenumber);
			BF_PrintError("Reading Block Error! ");
			return -1;
		}
	}
	else{

		BF_PrintError("Opening File Error! ");
		return -1;
	}

}

int HP_CloseFile(int fileDesc)

{
	int temp;

	temp = BF_CloseFile(fileDesc);
	if(temp == 0)
		return 0;
	else{
		BF_PrintError("Closing File Error! ");
		return -1;
	}
}


int HP_InsertEntry(int fileDesc,Record record)

{
	void *rec_for_in,*info_for_in;
	void *for_info,*for_search;
	BlockInfo blockinfo;
	int temp,temp2;
	int entered = 0;
	int all_records,all_blocks,sizeofsum;
	int i,j,size,recs;
	int error=0;


	temp = BF_ReadBlock(fileDesc,0,&for_info);
	if(temp == 0){
		all_records = ( (FileInformation*)for_info )->number_of_all_records;
		all_blocks =  ( (FileInformation*)for_info )->number_of_blocks_in_use;
		for(i=1; i<all_blocks; i++){   //if record exist ^_-

			temp = BF_ReadBlock(fileDesc,i,&info_for_in);
			recs = ( (BlockInfo*)info_for_in )->number_of_records;

			for(j=0;j<recs; j++){
			size = sizeof(BlockInfo);
			error=0;
			if(  ( (Record*)(info_for_in + size + (j*sizeof(Record)))  )->id == record.id)
				error++;
			if( strcmp( ( (Record*)(info_for_in + size + (j*sizeof(Record))))->name,
				(char*)record.name) == 0)
				error++;
			if( strcmp((char*) ( (Record*)(info_for_in + size + (j*sizeof(Record))))->surname,
                                (char*)record.surname) == 0)
                                error++;
			if( strcmp((char*) ( (Record*)(info_for_in + size + (j*sizeof(Record))))->city,
                                (char*)record.city) == 0)
                                error++;

			if(error == 4) return 0; /*return -1*/  //Error Record already exist!
										//!!Kanonika 8elei return -1 alla bazw 0 dioti h main kamia fora
										//bazi idies egrafes kai termatizei
										//ektos ama ine epi8imito na klini olo to programa tote..thelei -1!
			}
		}

		if(all_records%9 == 0){					//an einai 9 sola ta block
			if(all_blocks == BF_GetBlockCounter(fileDesc)){	//an prepei na ftiaso alo
				entered = 1;
				temp = BF_AllocateBlock(fileDesc);
				if(temp < 0){
					BF_PrintError("Allocating Block Error! ");
					return -1;
				}
				(( (FileInformation*)for_info )->number_of_blocks_in_use )++;


				all_blocks = ( (FileInformation*)for_info )->number_of_blocks_in_use;
				temp = BF_ReadBlock(fileDesc, all_blocks-1 ,&info_for_in);
				if(temp < 0) return -1;

				blockinfo.number_of_records = 0;
				blockinfo.next_block = -1;
				blockinfo.previous_block = all_blocks-2;

				memcpy(info_for_in,&blockinfo,sizeof(BlockInfo));
				temp = BF_WriteBlock(fileDesc,all_blocks-1);
				if(temp<0) return -1;
				if(all_blocks >= 3){
					temp = BF_ReadBlock(fileDesc, all_blocks-2 ,&info_for_in);
					((BlockInfo*)info_for_in)->next_block = all_blocks-1;
				}
			}
			if(entered == 1){
				temp = BF_ReadBlock(fileDesc, all_blocks-1 ,&rec_for_in);
				if(temp < 0) return -1;
			}
			else if(entered == 0){
				temp = BF_ReadBlock(fileDesc, all_blocks ,&rec_for_in);
				if(temp < 0) return -1;
			}

			memcpy( rec_for_in+sizeof(BlockInfo) , &record , sizeof(Record) );

			( ((BlockInfo*)rec_for_in )->number_of_records)++;

			temp = BF_ReadBlock(fileDesc, 0 ,&for_info);
			if(temp < 0) return -1;
			( ((FileInformation*)for_info )->number_of_all_records )++;
			temp = BF_WriteBlock(fileDesc,0);
			if(temp < 0)return -1;

			if(entered == 0){//gia na min to afksiso dipla
				( ((FileInformation*)for_info )->number_of_blocks_in_use )++;
				temp = BF_WriteBlock(fileDesc,all_blocks);
				if(temp < 0)return -1;
				temp = BF_WriteBlock(fileDesc,0);
				if(temp < 0)return -1;
			}

			if(entered ==1) temp = BF_WriteBlock(fileDesc,all_blocks-1);


		}
		else{
			sizeofsum = sizeof(BlockInfo);
			temp = BF_ReadBlock(fileDesc, all_blocks-1 ,&rec_for_in);
			if(temp<0) return -1;
			sizeofsum+=(  ( ((BlockInfo*)rec_for_in )->number_of_records )*sizeof(Record));

			 memcpy( rec_for_in+ sizeofsum, &record , sizeof(Record) );

			( ((BlockInfo*)rec_for_in )->number_of_records)++;
			temp = BF_WriteBlock(fileDesc,all_blocks-1);
				if(temp < 0)return -1;

			temp = BF_ReadBlock(fileDesc, 0, &for_info);
			if(temp < 0)return -1;
			( ((FileInformation*)for_info )->number_of_all_records )++;
			temp = BF_WriteBlock(fileDesc,0);
				if(temp < 0 )return -1;
		}
	}
	else{
		BF_PrintError("Reading Block Error! ");
		return -1;
	}


return 0;


}




int HP_DeleteEntry(int fileDesc, char* fieldName, void* value)

{
	int block_just_deleted;
	int flag;
	int block, i, error;
	int num_of_blocks_in_use;
	int block_records;
	int field_to_compare;
	int num_records_deleted;

	void *records;
	void *ptr_at_last_block;
	void *records_last_block;
	void *for_info;

	error = BF_ReadBlock(fileDesc, 0, &for_info);

	if(error)
		return -1;

	num_of_blocks_in_use = ( (FileInformation*) for_info)->number_of_blocks_in_use;
	num_records_deleted = 0;

        if(!strcmp(fieldName, "id") )
                field_to_compare = 1;
        else if(!strcmp(fieldName, "name") )
                field_to_compare = 2;
        else if(!strcmp(fieldName, "surname") )
                field_to_compare = 3;
        else if(!strcmp(fieldName, "city") )
                field_to_compare = 4;

	for(block=1; block<num_of_blocks_in_use; block++){	/* loop for every block, first just marking the records about to be deleted */


			error = BF_ReadBlock(fileDesc, block, &records);
			if(error)		/* if an error occurs return -1 */
				return -1;

			block_records = ( (BlockInfo*) records)->number_of_records;
			records += sizeof(BlockInfo);

			for(i=0; i<block_records; i++){

				flag = 0;		/* reinintiallize flag */
				block_just_deleted = 0;

				if(field_to_compare == 1){
					if( *( (int*) value) == ( (Record*) records)->id)
						flag = 1;		/* meaning that the record is about to be deleted */
				}
				else if(field_to_compare == 2){
					if( !(strcmp( ((char*)value) , ((Record*)records)->name ) ))
						flag = 1;               /* meaning that the record is about to be deleted */
				}
				else if(field_to_compare == 3){
					if( !(strcmp( ((char*)value) , ((Record*)records)->surname ) ))
						flag = 1;               /* meaning that the record is about to be deleted */
				}
				else if(field_to_compare == 4){
					if( !(strcmp( ((char*)value) , ((Record*)records)->city ) ))
						flag = 1;               /* meaning that the record is about to be deleted */
				}

				if(flag){		/* if the record should be deleted */

					if(block != num_of_blocks_in_use - 1){	/* if we are not at the last block */

						error = BF_ReadBlock(fileDesc, num_of_blocks_in_use - 1, &ptr_at_last_block);	/* read last block to obtain last record and exchange it with the one about to delete */
						if(error)
							return -1;		/* if an error occured return exit failure */
					}
					else{		/* if current block is the last block in use */
						ptr_at_last_block = records - i*sizeof(Record) - sizeof(BlockInfo);	/* point to the BlockInfo record of the block */
					}


					records_last_block = ptr_at_last_block;		/* point to the beginning of the block */
					records_last_block += sizeof(BlockInfo) + (( (BlockInfo*) ptr_at_last_block)->number_of_records - 1)*sizeof(Record);
						/* make pointer point to the last record of the file */

						/* copy last record on the one about to delete */
					( (Record*) records)->id = ( (Record*) records_last_block)->id;
					strcpy( ( (Record*) records)->name, ( (Record*) records_last_block)->name );
					strcpy( ( (Record*) records)->surname, ( (Record*) records_last_block)->surname );
					strcpy( ( (Record*) records)->city, ( (Record*) records_last_block)->city );


					(( (BlockInfo*) ptr_at_last_block)->number_of_records)--;		/* decrease num of records in the block */
					error = BF_WriteBlock(fileDesc, num_of_blocks_in_use - 1);	/* in order to change current data */
					if(error)
						return -1;

					if( ( (BlockInfo*) ptr_at_last_block)->number_of_records == 0){	/* delete last block if necessary */
						num_of_blocks_in_use--;	/* the last block is empty now */
						block_just_deleted = 1;

						error = BF_ReadBlock(fileDesc, 0, &for_info);	/* change information at the first block */
						if(error)
               						return -1;
        					(( (FileInformation*) for_info)->number_of_blocks_in_use)--;



						error = BF_WriteBlock(fileDesc, 0);
						if(error)
							return -1;

					}

					if(!block_just_deleted){
						if(block != num_of_blocks_in_use - 1){
							error = BF_WriteBlock(fileDesc, block);		/* write the block we just deleted a record from */
							if(error)
								return -1;
						}
					}
					else{
						if(block != num_of_blocks_in_use){
							error = BF_WriteBlock(fileDesc, block);
							if(error)
								return -1;
						}
					}

					num_records_deleted++;		/* increase number of records we have deleted */

				}

				if(!flag){	/* if we exchange a record we sould check if we sould also delete the exchanged one */
					if(i < block_records - 1 )
						records += sizeof(Record);	/* pointer to the next record of the block */
				}
				else{
					i--;

					if(!block_just_deleted){
						if(block == num_of_blocks_in_use - 1)
							block_records--;




					}
					else{
						if(block == num_of_blocks_in_use)
							break;
						}

				}


			}	/* loop for every record in the block */

	}			/* loop for every block */


	error = BF_ReadBlock(fileDesc, 0, &for_info);
	if(error)
		return -1;
	( (FileInformation*) for_info)->number_of_all_records -= num_records_deleted;	/* decrease total records in the file */
	error = BF_WriteBlock(fileDesc, 0);

	if(error)
		return -1;

	return 0;	/* return success */

}



void HP_GetAllEntries(int fileDesc, char* fieldName, void* value)

{
	int flag;
	int print_all;			/* is true if value is NULL */
	int block, i, error;
	int field_to_compare;
	int blocks_in_use;
	int size_so_far;
	int block_records;		/* number of records in the block */
	void *records;			/* a void pointer to the block's records */
	void *for_info;


	if(value == NULL || fieldName == NULL)
		print_all = 1;
	else
		print_all = 0;

	if(fieldName != NULL){
		if(!strcmp(fieldName, "id") )
			field_to_compare = 1;
		else if(!strcmp(fieldName, "name") )
			field_to_compare = 2;
		else if(!strcmp(fieldName, "surname") )
			field_to_compare = 3;
		else if(!strcmp(fieldName, "city") )
			field_to_compare = 4;
	}

	error = BF_ReadBlock(fileDesc, 0, &for_info);
	if(error){
		printf("An error occured while reading a block!\n\n");
		return;
	}

	blocks_in_use = ( (FileInformation*) for_info)->number_of_blocks_in_use;
	printf("***************************************\n");
	printf("blocks in use: %d\n", blocks_in_use);
	printf("number of records: %d\n", ( (FileInformation*) for_info)->number_of_all_records);
	printf("***************************************\n");

	for(block=1; block< blocks_in_use; block++){

		error = BF_ReadBlock(fileDesc, block, &records);	/* read serial every block in use */
		if(error){
			printf("An error occured while reading a block!\n\n");
			return ;
		}
		block_records = ( (BlockInfo*) records)->number_of_records;
		records += sizeof(BlockInfo);

		for(i=0; i<block_records; i++){

			if(!print_all){

				flag = 0;

				if(field_to_compare == 1){
					if( *( (int*) value) == ( (Record*) records)->id )
						flag = 1;
				}

				else if(field_to_compare == 2){
					if( !(strcmp( ((char*)value) , ((Record*)records)->name ) ))
						flag = 1;
				}

				else if(field_to_compare == 3){
					if( !(strcmp( ((char*)value) , ((Record*)records)->surname ) ))
						flag = 1;
				}

				else if(field_to_compare == 4){
					if( !(strcmp( ((char*)value) , ((Record*)records)->city ) ))
						flag = 1;
				}
			}
			else
				flag = 1;	/* print_all is true, print every record */

			if(flag){		/* printing record */
				printf("Id:\t\t%d\n", ((Record*)records)->id);
				printf("Name:\t\t%s\n", ((Record*)records)->name);
				printf("Surname:\t%s\n", ((Record*)records)->surname);
				printf("City:\t\t%s\n\n\n", ((Record*)records)->city);
			}

			if(i < block_records - 1 )
				records += sizeof(Record);		/* increase pointer to point to the next record */

		}	/* end of loop for the records of the block */

	}		/* end of loop for the blocks of the file */
}





