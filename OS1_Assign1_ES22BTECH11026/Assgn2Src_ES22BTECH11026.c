#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>

int M;                                                   //No. Of Threads
int N;                                                   //WE need to find the no. of Vampire nos. till 'N'

int global_counter = 0;                                  //Counts the no. of elements in the global array
FILE* fptr;                                              //FILE Pointer to the Output file
	
struct buffer{
	int* global_buffer;
};

struct buffer* memory;

//Function which takes pointer to two nos. as input and swaps them
void swapper(int* a, int* b){
	int swap = *a;
	*a = *b;
	*b = swap;
}

//Function which takes two nos. as input and forms an array consisting of their digits
void build_Array(int num1, int num2, int result[]){
	int i = 0;
	while(num1 > 0){
		result[i] = num1%10;
		i++;
		num1 = num1/10;
	}
	while(num2 > 0){
		result[i] = num2%10;
		i++;
		num2 = num2/10;
	}
}

//Function takes an array as input and sorts it.
void bubble_Sort(int arr[], int size){
	for(int i = 0; i < size; i++){
		for(int j= i+1; j<size; j++){
			if(arr[i] > arr[j]){
				swapper(&arr[i], &arr[j]);
			}
		}
	}
}

//Function which takes a no. as input and stores it's digits in an array
void makeArr(int num, int final[]){
	int i = 0;
	while(num > 0){
		final[i] = num%10;
		i++;
		num = num/10;
	}
}

//Function takes an input number and return the integer output describing the no. of digits in the num
int no_Of_digits(int num){
	int no_Of_digits = 0;
	while(num > 0){
		no_Of_digits++;
		num = num/10;
	}
	return no_Of_digits;
}

//Bool function returns true if the 2 input arrays are equal, false otherwise
bool compareArrays(int arr[], int brr[], int a_size, int b_size){
	if(a_size != b_size){
		return false;
	}
	for(int i = 0; i<a_size; i++){
		if(arr[i] != brr[i]){
			return false;
		}
	}
	return true;
}

//Bool Function returns true if input num is Vampire, else false otherwise
bool isVampire(int num) {
	int digitCount = no_Of_digits(num);
	if ((digitCount % 2) != 0){
		return false;
	}
	if (digitCount == 2){
		return false;
	}
	int upper_bound;
	int lower_bound;
	lower_bound = pow(10, (digitCount/2) -1);
	upper_bound = pow(10, digitCount/2);
	for(int i = lower_bound; i< upper_bound; i++){
		if(num%i == 0){
			int num2 = num/i;
			if(no_Of_digits(num2) > (no_Of_digits(num)/2) || no_Of_digits(i) > (no_Of_digits(num)/2)){
				continue;
			}
			if(num2%10 == 0 && i%10 == 0){
				continue;
			}
			int result[100];
			int final[100];
			build_Array(i, num2, result);                              //Result array stores the digits of 'num2' and 'i'
			bubble_Sort(result, no_Of_digits(i) + no_Of_digits(num2));
			makeArr(num, final);                                       //Final Array stores the digits of 'num'
			bubble_Sort(final, digitCount);
			if(compareArrays(result, final, no_Of_digits(i) + no_Of_digits(num2), digitCount)){
				return true;
			}
		}
	}
	return false;
}

//A function which forms the local buffer for each of the M threads and transfers it's contents into the global array
void* store_Vampire(void* arg){
	
	int index = *(int*)arg;
	int local_buffer[N/M];
	int j = 0;
	for(int i = index; i<=N; i += M){
		if(isVampire(i)){
			fprintf(fptr, "%d%s %s %s %s %d\n", i, ":", "Found", "by", "Thread", index);
			local_buffer[j] = i;
			j++;
		}
	}
	for(int i = 0; i< j; i++){
		memory->global_buffer[global_counter] = local_buffer[i];
		global_counter++;
	}
	if(index == M){
		fprintf(fptr, "%s %s %s%s %d", "Total", "Vampire", "numbers", ":", global_counter);
	}
}

int main(int argc, char* argv[]) {

	//FILE Pointer to the Input File
	FILE* fp;                                                       
	fp = fopen("InputFile.txt", "r");
	if(fp == NULL){
		perror("Error");
	}
	else{
		fscanf(fp, "%d %d", &N, &M);
	}
	fclose(fp);
	
	//Opening the Output File
	fptr = fopen("Outfile.txt", "w");
	if(fptr == NULL){
		printf("file doesn't exist\n");
	}
	
	//Allocating memory to the 'buffer structure'
	memory = (struct buffer*)malloc(sizeof(struct buffer));
	memory->global_buffer = (int*)malloc(N*sizeof(int));
	pthread_t* th = (pthread_t*)malloc(M*sizeof(pthread_t));
    int i;
    
	for (i = 0; i < M; i++) {
		int* a = malloc(sizeof(int));
		*a = i+1;
        if (pthread_create(th + i, NULL, &store_Vampire, a) != 0) {       
            perror("Failed to create thread");
            return 1;
        }
    }
    for (i = 0; i < M; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            return 2;
        }
    }
    fclose(fptr);
    
    return 0;
}
