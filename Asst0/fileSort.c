#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/uio.h>
#include<unistd.h>
#include<ctype.h>

//GLOBAL VARIABLES AND FUNCTION PROTOTYPES-----------------------------------------------------
int commaCounter;
int compareFunc(void* ptr1, void* ptr2);
int insertionSort(void* toSort, int(*comparator)(void*, void*));
int partition(char** sortThis, int left, int right, int(*comparator)(void*, void*));
void quickSortPt2(char** sortThis, int left, int right, int(*comparator)(void*, void*));
int quickSort(void* toSort, int(*comparator)(void*, void*));

int main(int argc, char** argv){
	//OPEN AND READ----------------------------------------------------------------------------

	//CHECK IF COMMAND LINE ARGUMENT INPUTTED CORRECTLY
	if(argc!=3){
		printf("fatal error: input is as follows\n");
		printf("./a.out <sortType> <fileName>\n");
		exit(1);
	}

	int fd = open(argv[2], O_RDONLY);

	//CHECK IF FILE SUCCESSFULLY OPENED
	if(fd==-1){
		printf("fatal error: failure to open file\n");
		exit(1);
	}

	//LENGTH OF FILE
	int length = lseek(fd, 0, SEEK_END);

	//CHECK IF FILE IS EMPTY
	if(length==0){
		printf("warning: file is empty\n");
	}

	lseek(fd, -length, SEEK_END);
	char* buffer = (char*)(malloc(sizeof(char)*length+1));
	buffer[length] = '\0';
	int status=1, readln=0;

	//READ ENTIRE FILE
	do{
		status = read(fd, buffer+readln, length-readln);
		readln+=status;
	}while(status > 0 && readln < length);

	//printf("%s\n", buffer);

	//COUNT NON SPACE CHARACTERS AND COMMAS
	int nonWhitespaceCounter=0;
	commaCounter = 1;
	int i;
	for(i=0; i<length; i++){
		if(isspace(buffer[i])==0){
			nonWhitespaceCounter++;
		}
		if(buffer[i]==','){
			commaCounter++;
		}
	}

	if(nonWhitespaceCounter==0 || nonWhitespaceCounter==commaCounter-1){
		printf("warning: file contains no tokens\n");
	}

	//NEW BUFFER WITH NO WHITESPACES
	char* buffer2 = (char*)(malloc(sizeof(char)*nonWhitespaceCounter+1));
	buffer2[nonWhitespaceCounter] = '\0';
	int index=0;
	for(i=0; i<length; i++){
		if(isspace(buffer[i])!=0){
			continue;
		}else{
			buffer2[index]=buffer[i];
			index++;
		}
	}

	//printf("%s\n", buffer2);

	//IGNORE LAST EMPTY TOKEN
	/*if(buffer2[nonWhitespaceCounter-2]==','){
		commaCounter--;
	}*/

	//PUT COMMA-SEPARATED TOKENS INTO ARRAY
	int first = 0;
	int last = 0;
	int index2 = 0;
	char* temp;
	char** items = (char**)(malloc(sizeof(char*)*commaCounter));
	for(i=0; i < nonWhitespaceCounter; i++){
		last++;
		if(buffer2[i]==','){
			temp = (char*)(malloc(sizeof(char)*(last-first)));		
			for(i=first; i<last-1; i++){
				strncat(temp, &buffer2[i], 1);
			}
			//temp[last-1]='\0';
			items[index2]=temp;
			index2++;
			first = last;
		}
	}

	//INSERT LAST TOKEN
	temp = (char*)(malloc(sizeof(char)*(last-first)));
	//temp = "";
	for(i=first; i<last; i++){
		strncat(temp, &buffer2[i], 1);
	}
	items[index2]=temp;

	//DECLARE FUNCTION POINTER
	int(*comparator)(void*, void*) = compareFunc;

	//CALL SORTING FUNCTIONS
	if(argv[1][0]=='-' && argv[1][1]=='q'){
		quickSort(items, *comparator);
	}else if(argv[1][0]=='-' && argv[1][1]=='i'){
		insertionSort(items, *comparator);
	}else{
		printf("fatal error: type sorting algorithm as '-q' for quick sort or '-i' for insertion sort");
		exit(1);
	}

	//PRINT FINAL OUTPUT
	//int count = 1;
	for(i=0; i<commaCounter; i++){
		printf("%s\n", items[i]);
		//count++;
	}

	//printf("\n\n%s\n%lu", items[499936], strlen(items[499936]));

	free(buffer);
	free(buffer2);
	free(items);
	free(temp);
	return 0;
}

//COMPARATOR---------------------------------------------------------------------------
int compareFunc(void* ptr1, void* ptr2){
	//TYPE CAST
	char* newPtr1 = (char*)ptr1;
	char* newPtr2 = (char*)ptr2;

	if(strlen(newPtr1)==0){
		return -1;
	}else if(strlen(newPtr2)==0){
		return 1;
	}

	//CHECK IF NUMBERS OR LETTERS AND COMPARE
	if(isdigit(newPtr1[0])==1 || isdigit(newPtr1[1])==1){
		int newInt1 = atoi(newPtr1);
		int newInt2 = atoi(newPtr2);
		if(newInt1>newInt2){
			printf("1: %d\t%d\n", newInt1, newInt2);
			return 1;
		}else if(newInt2>newInt1){
			printf("-1: %d\t%d\n", newInt1, newInt2);
			return -1;
		}else{
			printf("0: %d\t%d\n", newInt1, newInt2);
			return 0;
		}
	}else{
		char currChar1;
		char currChar2;
		int strLen1 = strlen(newPtr1);
		int strLen2 = strlen(newPtr2);
		int min = 0;
		if(strLen1<strLen2){
			min = strLen1;
		}else{
			min = strLen2;
		}
		int i;
		for(i = 0; i < min; i++){
			currChar1 = newPtr1[i];
			currChar2 = newPtr2[i];
			if(currChar1==currChar2){
				continue;
			}else{
				return currChar1 - currChar2;
			}
		}
		if(strLen1==strLen2){
			return 0;
		}else if(min==strLen1){
			return -1;
		}else{
			return 1;
		}
	}
}

//SORTS--------------------------------------------------------------------------------
int insertionSort(void* toSort, int(*comparator)(void*, void*)){
	char** sortThis = (char**)toSort;
	int i;
	for(i = 0; i < commaCounter; i++){
		int j=i;
		while(j>0 && (*comparator)(sortThis[j-1], sortThis[j]) > 0){
			char* temp = sortThis[j];
			sortThis[j] = sortThis[j-1];
			sortThis[j-1]=temp;
			j = j-1;
		}
	}
	return 0;
}

//QUICKSORT PARTITION FUNCTION
int partition(char** sortThis, int left, int right, int(*comparator)(void*, void*)){
	char* pivot = sortThis[left];
	int i = left;
	int j;
	for(j=left+1; j<=right; j++){
		if(((*comparator)(sortThis[j], pivot))<0){
			i++;
			char* temp = sortThis[i];
			sortThis[i] = sortThis[j];
			sortThis[j] = temp;
		}
	}
	char* temp = sortThis[i];
	sortThis[i] = sortThis[left];
	sortThis[left] = temp;
	return i;
}

//NEW QUICKSORT FUNCTION
void quickSortPt2(char** sortThis, int left, int right, int(*comparator)(void*, void*)){
	if(left < right){
		int pivot = partition(sortThis, left, right, (*comparator));
		quickSortPt2(sortThis, left, pivot-1, (*comparator));
		quickSortPt2(sortThis, pivot+1, right, (*comparator));
	}
}

//MAIN QUICKSORT FUNCTION
int quickSort(void* toSort, int(*comparator)(void*, void*)){
	char** sortThis = (char**)toSort;
	quickSortPt2(sortThis, 0, commaCounter-1, (*comparator));
	return 0;
}
