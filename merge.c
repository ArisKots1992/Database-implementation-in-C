#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BF.h"
#include "HP.h"
#include "Record.h"

int Join_MergeSort(char *inputName1, char *inputName2, char *outputName)

{
	int i, j;
	int record_from;
	int record_num1, record_num2;
	int not_empty1, not_empty2, empty_block1, empty_block2;
	int total_block_records1, total_block_records2;
	int output_blocks, out_file_records;
	int fileDesc1, fileDesc2, outDesc, tempDesc;
	int blocks1, num_of_blocks_in_use1, blocks2, num_of_blocks_in_use2;
	int total_records, memory_block_records;

	void *fileInformation;
	void *records1, *records2, *mem_records, *out_records;


	if(!strcmp(inputName1, inputName2) || !strcmp(inputName1, outputName) || !strcmp(inputName2, outputName) )
		return -1;

	if( HP_CreateFile(outputName) < 0 ){	/* create the output file */
		BF_PrintError("Error in output's file creation!\n");
		return -1;
	}
	if( HP_CreateFile("tempfile") < 0){
		BF_PrintError("Error in temporary's file creation!\n");
		return -1;
	}

		/* open all four files */
	fileDesc1 = HP_OpenFile(inputName1);
	if(fileDesc1 < 0)
		return -1;
	fileDesc2 = HP_OpenFile(inputName2);
	if(fileDesc2 < 0)
		return -1;
	outDesc = HP_OpenFile(outputName);
	if(outDesc < 0)
		return -1;
	tempDesc = HP_OpenFile("tempfile");
	if(tempDesc < 0)
		return -1;

	total_records = 0;	/* all the records that is about to have the output file */

	if( BF_ReadBlock(fileDesc1, 0, &fileInformation) )
		return -1;	/* return if an error occurs */
	num_of_blocks_in_use1 = ( (FileInformation*) fileInformation)->number_of_blocks_in_use;
	total_records += ( (FileInformation*) fileInformation)->number_of_all_records;	/* add all the records of the file1 */
	if(num_of_blocks_in_use1 > 1)	/* if there are blocks in use then file1 is not empty */
		not_empty1 = 1;
	else
		not_empty1 = 0;

        if( BF_ReadBlock(fileDesc2, 0, &fileInformation) )
                return -1;      /* return if an error occurs */
        num_of_blocks_in_use2 = ( (FileInformation*) fileInformation)->number_of_blocks_in_use;
	total_records += ( (FileInformation*) fileInformation)->number_of_all_records;  /* add all the records of the file2 */
	if(num_of_blocks_in_use2 > 1)	/* if there are blocks in use then file2 is not empty */
		not_empty2 = 1;
	else
		not_empty2 = 0;

	if(BF_AllocateBlock(tempDesc) < 0)	/* allocate a memory-block to be used as the output of the merge of two inputfile's blocks */
		return -1;
	if( BF_ReadBlock(tempDesc, 0, &fileInformation) )
                return -1;      /* return if an error occurs */
	(((FileInformation*) fileInformation)->number_of_blocks_in_use)++;	/* the tempfile has two blocks */
	if( BF_WriteBlock(tempDesc, 0) < 0)
		return -1;

	blocks1 = 1;	/* the blocks about to be merged */
	empty_block1 = 1;
	blocks2 = 1;
	empty_block2 = 1;
	memory_block_records = 0;	/* the memory block is empty */
	output_blocks = 0;		/* no blocks in the outputfile yet */
	out_file_records = 0;


/*------------------------------------------------------------------------------------------------------------*/

	for(i=0; i<total_records; i++){		/* repeat for every record */

		/* read new blocks if necessary */
		if(not_empty1){
			if(empty_block1){
				if(BF_ReadBlock(fileDesc1, blocks1, &records1))	/* return if an error occured while reading block */
					return -1;
				empty_block1 = 0;	/* block has records */
				record_num1 = 1;	/* we are now at the frst record of the block */
				total_block_records1 = ( (BlockInfo*) records1)->number_of_records;
				records1 += sizeof(BlockInfo);		/* records1 points to the first record of the block */

			}
		}

		if(not_empty2){
			if(empty_block2){
				if(BF_ReadBlock(fileDesc2, blocks2, &records2))	/* return if an error occured while reading block */
					return -1;
				empty_block2 = 0;	/* block has records */
				record_num2 = 1;	/* we are now at the frst record of the block */
				total_block_records2 = ( (BlockInfo*) records2)->number_of_records;
				records2 += sizeof(BlockInfo);          /* records2 points to the first record of the block */

			}
		}


		if(!not_empty1)		/* record of file2 to be insert in the outputFile */
			record_from = 2;
		else if(!not_empty2)		/* record of file1 to be insert in the outputFile */
			record_from = 1;
		else if( ((Record*) records1)->id <= ((Record*) records2)->id){	/* record of file1 to be insert in the outputFile */
			record_from = 1;

				/* if the records are the same for both blocks insert only one */
			if( ((Record*) records1)->id == ((Record*) records2)->id ){
				if( !strcmp( ((Record*) records1)->name, ((Record*) records2)->name) ){
					if( !strcmp( ((Record*) records1)->surname, ((Record*) records2)->surname) ){
						if( !strcmp( ((Record*) records1)->city, ((Record*) records2)->city) ){

							i++;	/* one record overtaken */
				                        record_num2++;
                        				if(record_num2 <= total_block_records2) /* block has a maximum of 9 records */
                                				records2 += sizeof(Record);     /* pointer to the next record of the file2 */

				                        else{   /* this record was the last of the block */
                                				blocks2++;      /* next block to be used */
                  				                if(blocks2 >= num_of_blocks_in_use2)
                                        				not_empty2 = 0;         /* file2 has no more blocks */
                                				else
                                        				empty_block2 = 1;
                        				}

						}
					}
				}
			}


		}
		else if( ((Record*) records1)->id > ((Record*) records2)->id)	/* record of file2 to be insert in the outputFile */
			record_from = 2;

		if( BF_ReadBlock(tempDesc, 1, &mem_records) )	/* read the memory block */
			return -1;
		mem_records += sizeof(BlockInfo);
		mem_records += memory_block_records * sizeof(Record);	/* pointer to the first available position of the block */

		if(record_from == 1){

			memcpy(mem_records, records1, sizeof(Record));	/* copy record to the memory block */

			record_num1++;
			if(record_num1 <= total_block_records1)	/* block has a maximum of 9 records */
				records1 += sizeof(Record);	/* pointer to the next record of the file1 */

			else{	/* this record was the last of the block */
				blocks1++;	/* next block to be used */
				if(blocks1 >= num_of_blocks_in_use1)
					not_empty1 = 0;		/* file1 has no more blocks */
				else
					empty_block1 = 1;
			}


		}
		else{			/* record of file2 to be insert in the outputFile */

                        memcpy(mem_records, records2, sizeof(Record));  /* copy record to the memory block */

                        record_num2++;
                        if(record_num2 <= total_block_records2) /* block has a maximum of 9 records */
                                records2 += sizeof(Record);     /* pointer to the next record of the file2 */

                        else{   /* this record was the last of the block */
                                blocks2++;      /* next block to be used */
                                if(blocks2 >= num_of_blocks_in_use2)
                                        not_empty2 = 0;         /* file2 has no more blocks */
				else
					empty_block2 = 1;
                        }

		}

		out_file_records++;
		memory_block_records++;		/* records in the memory block were increased by 1 */
		if(memory_block_records == 9 || i == total_records - 1 ){	/* if memory_block is full or we are at the last record */
			output_blocks++;

			if( BF_AllocateBlock(outDesc) )		/* allocate a new block to copy the one from the memory to the outputfile */
				return -1;

                        if( BF_ReadBlock(tempDesc, 1, &mem_records) )
                                return -1;
			mem_records += sizeof(BlockInfo);

			if( BF_ReadBlock(outDesc, output_blocks, &out_records) )
				return -1;
			( (BlockInfo*) out_records)->number_of_records = memory_block_records;
			out_records += sizeof(BlockInfo);

				/* copy records from the memory to the output file */
			for(j=0; j<memory_block_records; j++){
				memcpy(out_records, mem_records, sizeof(Record));
				mem_records += sizeof(Record);	/* pointer to the next record */
				out_records += sizeof(Record);
			}

			if( BF_WriteBlock(outDesc, output_blocks) )
				return -1;

			memory_block_records = 0;	/* reinitiallize memory_block_records */
		}

	}	/* end of loop */
/*------------------------------------------------------------------------------------------------------------*/

	if( BF_ReadBlock(outDesc, 0, &fileInformation) )	/* insert the output's file data in the first block */
		return -1;

	((FileInformation*) fileInformation)->number_of_all_records = out_file_records;
	((FileInformation*) fileInformation)->number_of_blocks_in_use = output_blocks + 1;

	if( BF_WriteBlock(outDesc, 0) )
		return -1;

		/* close all files used */
	if(HP_CloseFile(fileDesc1) )
		return -1;
	if(HP_CloseFile(fileDesc2) )
		return -1;
	if(HP_CloseFile(outDesc) )
		return -1;
	if(HP_CloseFile(tempDesc) )
		return -1;

	return 0;	/* return successfully */

}
