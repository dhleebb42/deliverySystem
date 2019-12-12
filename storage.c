#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "storage.h"

/* 
  definition of storage cell structure ----
  members :
  int building : building number of the destination
  int room : room number of the destination
  int cnt : number of packages in the cell
  char passwd[] : password setting (4 characters)
  char *contents : package context (message string)
*/
typedef struct {
	int building;
	int room;
	int cnt;
	char passwd[PASSWD_LEN+1];	
	
	char *context;
} storage_t;


static storage_t** deliverySystem; 			//deliverySystem
static int storedCnt = 0;					//number of cells occupied
static int systemSize[2] = {0, 0};  		//row/column of the delivery system
static char masterPassword[PASSWD_LEN+1];	//master password




// ------- inner functions ---------------

//print the inside context of a specific cell
//int x, int y : cell to print the context
static void printStorageInside(int x, int y) {
	printf("\n------------------------------------------------------------------------\n");
	printf("------------------------------------------------------------------------\n");
	if (deliverySystem[x][y].cnt > 0)
		printf("<<<<<<<<<<<<<<<<<<<<<<<< : %s >>>>>>>>>>>>>>>>>>>>>>>>>>>>\n", deliverySystem[x][y].context);
	else
		printf("<<<<<<<<<<<<<<<<<<<<<<<< empty >>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
		
	printf("------------------------------------------------------------------------\n");
	printf("------------------------------------------------------------------------\n\n");
}

//initialize the storage
//set all the member variable as an initial value
//and allocate memory to the context pointer
//int x, int y : cell coordinate to be initialized
static void initStorage(int x, int y) {

	deliverySystem[x][y].cnt = 0; 
	deliverySystem[x][y].building = NULL;
	deliverySystem[x][y].room = NULL;
	deliverySystem[x][y].context = NULL;
	deliverySystem[x][y].passwd[PASSWD_LEN+1] = NULL;
}

//get password input and check if it is correct for the cell (x,y)
//int x, int y : cell for password check
//return : 0 - password is matching, -1 - password is not matching
static int inputPasswd(int x, int y) {
	
	char passwd[PASSWD_LEN + 1];
	printf("password: ");
	scanf("%4s", passwd);
	fflush(stdin);

	if (strcmp(deliverySystem[x][y].passwd, passwd) == 0) 
	{
		return 0;
	}
	else 
	{
		return -1;
	}
}





// ------- API function for main.c file ---------------

//backup the delivery system context to the file system
//char* filepath : filepath and name to write
//return : 0 - backup was successfully done, -1 - failed to backup
int str_backupSystem(char* filepath) 
{
	FILE *f;
	f = fopen(filepath, "w");
	
	fprintf(f, "%d %d\n", systemSize[0], systemSize[1]);
	fprintf(f, "%s\n", masterPassword);
	
	fclose(f);
	
	return 0;	
}



//create delivery system on the double pointer deliverySystem
//char* filepath : filepath and name to read config parameters (row, column, master password, past contexts of the delivery system
//return : 0 - successfully created, -1 - failed to create the system
int str_createSystem(char* filepath) {
	
	FILE *f;
	f = fopen(filepath, "r");

	//structure 2 dimensional array dynamic memory allocation
	fscanf(f, "%d %d", &systemSize[0], &systemSize[1]);

	deliverySystem = (storage_t**)malloc(sizeof(storage_t*)*systemSize[0]);
	
	int i;
	
	for (i = 0; i < systemSize[0]; i++) 
	{
		deliverySystem[i] = (storage_t*)malloc(sizeof(storage_t)*systemSize[1]);
	}

	//master keyword save
	fscanf(f, "%s", masterPassword);
	
	//value initialization
	int x;
	for (x = 0; x < systemSize[0]; x++) 
	{
		int y;
		for (y = 0; y < systemSize[i]; y++)
		{
			initStorage(x, y);
		}
	} 

	//information save
	while (feof(f) == 0) 
	{
		int a = 0, b = 0, c = 0, d = 0;
		char e[100];
		fscanf(f, "%d %d %d %d %[^\n]s", &a, &b, &c, &d, e);
		deliverySystem[a][b].building = c;
		deliverySystem[a][b].room = d;
		deliverySystem[a][b].cnt = 1;
		storedCnt++;
		
		char *ptr = strtok(e, " "); 							//cut the string
		char *sArr[2] = { NULL, };
		int i = 0;
		while (ptr != NULL)										//repeat until cut string do not appear
		{
			sArr[i] = ptr; 										//cut the string and save the memory address in string pointer array
			i++; 												//index increase
			
			ptr = strtok(NULL, " "); 							//cut the following string and return the pointer
		}
		
		strcpy(deliverySystem[a][b].passwd, sArr[0]); 			//copy first string in passwd
		//strcpy(&deliverySystem[a][b].context, sArr[1]); 		//copy second string in passwd
	}

	fclose(f);
	
	return 0;
}

//free the memory of the deliverySystem 
void str_freeSystem(void) {

	free(deliverySystem);

}



//print the current state of the whole delivery system (which cells are occupied and the destination of the each occupied cells)
void str_printStorageStatus(void) {
	int i, j;
	printf("----------------------------- Delivery Storage System Status (%i occupied out of %i )-----------------------------\n\n", storedCnt, systemSize[0]*systemSize[1]);
	
	printf("\t");
	for (j=0;j<systemSize[1];j++)
	{
		printf(" %i\t\t",j);
	}
	printf("\n-----------------------------------------------------------------------------------------------------------------\n");
	
	for (i=0;i<systemSize[0];i++)
	{
		printf("%i|\t",i);
		for (j=0;j<systemSize[1];j++)
		{
			if (deliverySystem[i][j].cnt > 0)
			{
				printf("%i,%i\t|\t", deliverySystem[i][j].building, deliverySystem[i][j].room);
			}
			else
			{
				printf(" -  \t|\t");
			}
		}
		printf("\n");
	}
	printf("--------------------------------------- Delivery Storage System Status --------------------------------------------\n\n");
}



//check if the input cell (x,y) is valid and whether it is occupied or not
int str_checkStorage(int x, int y) {
	if (x < 0 || x >= systemSize[0])
	{
		return -1;
	}
	
	if (y < 0 || y >= systemSize[1])
	{
		return -1;
	}
	
	return deliverySystem[x][y].cnt;	
}


//put a package (msg) to the cell
//input parameters
//int x, int y : coordinate of the cell to put the package
//int nBuilding, int nRoom : building and room numbers of the destination
//char msg[] : package context (message string)
//char passwd[] : password string (4 characters)
//return : 0 - successfully put the package, -1 - failed to put
int str_pushToStorage(int x, int y, int nBuilding, int nRoom, char msg[MAX_MSG_SIZE+1], char passwd[PASSWD_LEN+1]) {

	deliverySystem[x][y].building = nBuilding;
	deliverySystem[x][y].room = nRoom;
	deliverySystem[x][y].cnt = 1; //'1' because input the new package 
	strcpy(deliverySystem[x][y].passwd, msg); 
	strcpy(&deliverySystem[x][y].context, passwd);
	
	return 0;
	//make to return -1 when error occur
}



//extract the package context with password checking
//after password checking, then put the msg string on the screen and re-initialize the storage
//int x, int y : coordinate of the cell to extract
//return : 0 - successfully extracted, -1 = failed to extract
int str_extractStorage(int x, int y) {
	
	if (inputPasswd(x, y)==0) 
	{
		deliverySystem[x][y].building = NULL;
		deliverySystem[x][y].room = NULL;
		deliverySystem[x][y].cnt = 0; //'0' because extracted package 	
		strcpy(deliverySystem[x][y].passwd, NULL);
		strcpy(&deliverySystem[x][y].context, NULL);
	
		printf(" -----------> Succeeded to extract package for (%i, %i)\n", x, y);
		printf("building : %d\nroom : %d\nmsg : %s\npasswd : %s\n",deliverySystem[x][y].building, deliverySystem[x][y].room, &deliverySystem[x][y].context, deliverySystem[x][y].passwd);
		
		return 0;
	}
	else 
	{
		return -1;
	}
}

//find my package from the storage
//print all the cells (x,y) which has my package
//int nBuilding, int nRoom : my building/room numbers
//return : number of packages that the storage system has
int str_findStorage(int nBuilding, int nRoom) {

	if(deliverySystem==0)
	{	
		return 0;
	}
	else
	{
		char* filepath;
		
		FILE *f;
		f = fopen(filepath, "r");
	
		int a = 0, b = 0, c = 0, d = 0;
		char e[100];
		fscanf(f, "%d %d %d %d %[^\n]s", &a, &b, &c, &d, e);
		deliverySystem[a][b].building = c;
		deliverySystem[a][b].room = d;
		deliverySystem[a][b].cnt = 1;
		storedCnt++;
		
		printf("%d %d %d %d %[^\n]", a, b, c, d, e);
		
		int cnt = deliverySystem[nBuilding][nRoom].cnt;
		
		fclose(f);
		
		return cnt;
	}
}
