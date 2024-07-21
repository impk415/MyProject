#include<iostream>
#include<bits/stdc++.h>
#include<chrono>
#include<pthread.h>
#include<atomic>
using namespace std;

int N,K,rowInc;
int C = 0;            //shared Counter as mentioned in the Assignment
FILE* fptr;           //file pointer for input file
FILE* fp;             // file pointer for output file

//Initialising Compare_and_swap variabes used
std:: atomic<int> ticket(0);  //A ticket number for each thread, initialised to 0
std:: atomic<int> turn(0);   //Determines the turn of the thread whose ticket_no matches with turn

class intermediary{

    public:              //giving public access
    int** input_array;
    int thread_no;
};
intermediary** bridge;        //bridge variable points to an array of structures
                            //ACTS AS A BRIDGE BY CONTAINING ALL THE ARGUMENTS TO BE PASSED TO THE function dynamic_Atomic

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

//dynamic_atomic technique to compute the C matrix
void* dynamic_Atomic(void* arg){
    intermediary* inner = (intermediary*)arg;
    int local_row_counter = 0;
    do
    {
        //entry section
        int my_ticket = ticket.fetch_add(1, memory_order_relaxed);
        while (my_ticket != turn.load(memory_order_relaxed)) {
            // Spin until it's our turn
        }

        // Critical section
        int starting_row = C;
        C += rowInc;

        // Exit the critical section
        turn.fetch_add(1, memory_order_relaxed); //incrementing turn to the next value, thread whose ticket number matches with the turn enters the CS.
        for(int i = starting_row; i < starting_row + rowInc; i++){
            for(int j = 0; j < N; j++){
                global_array[i][j] = compute_Cij(inner->input_array, i, j);
            }
            local_row_counter++;
        }
        if(local_row_counter == N/K){
            break;
        }
    }while (true);
    
    
    return NULL;
}

int main(){

    //Timer starts here 
    auto start = chrono::steady_clock::now();

    //Opening the Files
    fptr = fopen("inp.txt", "r");
    fp = fopen("output_AtomicIncrement.txt", "w");

    //Checking for possible errors
    if(fp == NULL){
        perror("Error");
        return 2;
    }

    if(fptr == NULL){
        perror("Error");
        return 1;
    }
    fscanf(fptr, "%d %d %d", &N, &K, &rowInc);

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
        if(pthread_create(thread_ptr + i, NULL, dynamic_Atomic, bridge[i])){
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
    
    delete[] thread_ptr;

    //Timer ends here
    auto end = chrono::steady_clock::now();

    //Differnce is calculated
    auto diff = end - start;
    auto seconds = std::chrono::duration<double>(diff);
    fprintf(fp, "Program runtime: %.6f seconds\n", seconds.count());
    fclose(fp);

return 0;
}