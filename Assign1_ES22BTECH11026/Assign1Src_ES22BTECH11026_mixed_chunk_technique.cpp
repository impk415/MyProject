#include<iostream>
#include<bits/stdc++.h>
#include<chrono>
using namespace std;

int N,K;
FILE* fptr;           //file pointer for input file
FILE* fp;             // file pointer for output file

struct intermediary{
    int** input_array;
    int thread_no;
};
intermediary** bridge;        //bridge variable points to an array of structures
                            //ACTS AS A BRIDGE BY CONTAINING ALL THE ARGUMENTS TO BE PASSED TO THE function mixed_chunk_technique

int global_array[2048][2048];    //array to store the row entries for the matrix C = A.A

//function which takes the row(i) & column(j) index as input and evaluates the corresponding dot product Cij
int compute_Cij(int* arr[], int i, int j){
    int k;
    int sum = 0;
    for(k = 0; k < N; k++){
        sum += arr[i][k] * arr[k][j];
    }
    return sum;
}

//equal-chunk technique to compute the C matrix
void* mixed_chunk_technique(void* arg) {
    intermediary* inner = (intermediary*)arg;
    
    int p = N/(2*K);
    for(int i = inner->thread_no*p; i < (inner->thread_no+1)*p; i++){
        for(int j = 0; j < N; j++){
            global_array[i][j] = compute_Cij(inner->input_array, i, j);

        }
    }

    for(int i = (N/2 + inner->thread_no*p); i < (N/2 + (inner->thread_no+1)*p); i++){
        for(int j = 0; j < N; j++){
            global_array[i][j] = compute_Cij(inner->input_array, i, j);

        }
    }

    return NULL;
}

int main(){

    //Timer starts here 
    auto start = chrono::steady_clock::now();

    //Opening the Files
    fptr = fopen("inp.txt", "r");
    fp = fopen("output_mixed_chunk.txt", "w");

    //Checking for possible errors
    if(fp == NULL){
        perror("Error");
        return 2;
    }

    if(fptr == NULL){
        perror("Error");
        return 1;
    }
    fscanf(fptr, "%d %d", &N, &K);

    //making an array of strctures where each element of the array is allocated a structure
    intermediary** bridge = new intermediary*[K];
    for(int i = 0; i < K; i++){
        bridge[i] = new intermediary;
        bridge[i]->input_array = new int*[N];
            for(int j = 0; j < N; j++){
                bridge[i]->input_array[j] = new int[N];
            }
    }

    //File Input
    char c = fgetc(fptr);
    int rows = 0;
    int columns = 0;

    while(c != EOF){
        fscanf(fptr, "%d", &bridge[0]->input_array[rows][columns]);
        columns++;
        c = fgetc(fptr);
        if(c == '\n'){
            rows++;
            columns = 0;
        }
    }
    fclose(fptr);

    //Copying the contents of the input_array into the rest of the structures
    for(int i = 1; i < K; i++){
            bridge[i]->input_array = bridge[0]->input_array;
    }

    //Creation of threads
    pthread_t* thread_ptr = new pthread_t[K];
    for(int i = 0; i < K; i++){
        bridge[i]->thread_no = i;
        if(pthread_create(thread_ptr + i, NULL, mixed_chunk_technique, bridge[i])){
            cout<<"Error in creating the thread "<<i<<endl;
        }
    }

    //Joining all the threads
    for(int i = 0; i < K; i++){
        if(pthread_join(thread_ptr[i], NULL)){
            cout<<"Error in joining the thread "<<i<<endl;
        }
    }
    
    //Outputting the contents of the global_array into the the output file named "output_mixed.txt"
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            fprintf(fp, "%d ", global_array[i][j]);
        }fprintf(fp, "\n");
    }
    delete []thread_ptr;

    //Timer ends here
    auto end = chrono::steady_clock::now();

    //Differnce is calculated
    auto diff = end - start;
    auto seconds = std::chrono::duration<double>(diff);
    fprintf(fp, "Program runtime: %.6f seconds\n", seconds.count());
    fclose(fp);

return 0;
}