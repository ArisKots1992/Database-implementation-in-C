#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "BF.h"
#include "Record.h"
#include "HP.h"

int Join_MergeSort(char*, char*, char*);

int main(void){

int tmp1, tmp2;
void *x,*y;

	 BF_Init();

	if( HP_CreateFile("FILE1") == 0) printf("SUCCESS!!!!\n");


                tmp1 = HP_OpenFile("FILE1");
		if(tmp1 >=0 ) printf("SUCCESFULL OPENING (oxi gia club DJ's etc..)!!\n");


                        if ( BF_ReadBlock(tmp1, 0,&x) < 0) {
                                BF_PrintError("Error getting block");
				return 0;
                        }

       if( HP_CreateFile("FILE2") == 0) printf("SUCCESS!!!!\n");


                tmp2 = HP_OpenFile("FILE2");
                if(tmp2 >=0 ) printf("SUCCESFULL OPENING (oxi gia club DJ's etc..)!!\n");



		printf("APOTELESMA file_name -->>%s\n",( (FileInformation*)x )->file_name);
                printf("APOTELESMA file_type -->>%s\n",( (FileInformation*)x )->file_type);


Record xa;

int i;
xa.id=0;
strcpy((char*)xa.name,"Aris");
strcpy((char*)xa.surname,"Kots");
strcpy((char*)xa.city,"Athens");
for(i=0;i<10;i++){
	xa.id += 2;
	HP_InsertEntry(tmp1, xa);
}

//(xa.name[0])++;
//(xa.surname[0])++;
//(xa.city[0])++;
xa.id =1;
for(i=0;i<20;i++){
        xa.id += 2;
        HP_InsertEntry(tmp2, xa);
}


printf("/*---------------------------------------------------------------*/\n\t\tFILE1\n/*---------------------------------------------------------------*/\n\n");
HP_GetAllEntries(tmp1, NULL, NULL);
printf("\n\n");
printf("/*---------------------------------------------------------------*/\n\t\tFILE2\n/*--------------------------------------------------------------*/\n\n");
HP_GetAllEntries(tmp2, NULL, NULL);

HP_CloseFile(tmp1);
HP_CloseFile(tmp2);

printf("\n\n/*----------------------------------------- MERGE -----------------------------------------------*/\n\n");
if( Join_MergeSort("FILE1", "FILE2", "OUT") ){
	printf("ERROR!!!\n\n\n");
	return 1;
}
printf("OK!!\n\n\n");

tmp1 = HP_OpenFile("OUT");
HP_GetAllEntries(tmp1, NULL, NULL);
HP_CloseFile(tmp1);



return 0;

}

