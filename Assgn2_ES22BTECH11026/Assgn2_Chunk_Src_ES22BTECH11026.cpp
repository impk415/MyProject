#include<iostream>
#include<bits/stdc++.h>
#include<pthread.h>
#include<sched.h>
#include<chrono>
using namespace std;

int N,K,C,BT;
FILE* fptr;           //file pointer for input file
FILE* fp;             // file pointer for output file

double time_array[96];  //Time array stores the time of execution for each of the 'K' threads

class intermediary{
    public:
        int** input_array;
        int thread_no;
};

intermediary** bridge;        //bridge variable points to an array of structures
                            //ACTS AS A BRIDGE BY CONTAINING ALL THE ARGUMENTS TO BE PASSED TO THE function mixed_technique

int global_array[1024][1024];    //array to store the row entries for the matrix C = A.A


//function which takes the row(i) & column(j) index as input and evaluates the corresponding dot product Cij
int compute_Cij(int* arr[], int i, int j){
    int k;  
    int sum = 0;
    for(k = 0; k < N; k++){
        sum += arr[i][k] * arr[k][j];
    }
    return sum;
}

//Function takes an array and it's size as input, calculates the sum of the elements and returns the average
double avg_of_sum_of_array(double* arr, int n){
    double sum = 0.0;
    if(BT == 0){
        for(int i = 0; i < n; i++){
            sum += arr[i];
        }
    }
    else if(n == BT){
        for(int i = 0; i < n; i++){
            sum += arr[i];
        }
    }
    else{
        for(int i = BT; i < n; i++){
            sum += arr[i];
        }
        return sum/(K-BT);
    }
    return sum/n;
}

//chunk technique to compute the C matrix
void* chunk_technique(void* arg){
    intermediary* inner = (intermediary*)arg;
    int p = N/K;

    //For Bounded threads
    if(inner->thread_no < BT){
        auto start = chrono::steady_clock::now();    //Timer starts
        cpu_set_t mask;                              //initialsing the CPU SET mask
        CPU_ZERO(&mask);                             //Emptying the set
        int cpu_no = (inner->thread_no)*C/K;         
        CPU_SET(cpu_no, &mask);

        pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &mask);   //setting the affinity for the calling thread to CPU set pointed by mask

        for(int i = (inner->thread_no)*p; i < (inner->thread_no+1)*p; i++){
            for(int j = 0; j < N; j++){
                global_array[i][j] = compute_Cij(inner->input_array, i, j);
            }
        }
        auto end = chrono::steady_clock::now();       //Timer ends
        std::chrono::duration<double> duration = end - start;
        double iteration_time = duration.count();
        time_array[inner->thread_no] = iteration_time;  //Storing the execution time in the time_array
    }

    //For normal threads
    else{
        auto start = chrono::steady_clock::now();
        if(inner->thread_no < K-1){
            for(int i = (inner->thread_no)*p; i < (inner->thread_no+1)*p; i++){
                for(int j = 0; j < N; j++){
                    global_array[i][j] = compute_Cij(inner->input_array, i, j);
                }
            }
        }
        else{
            for(int i = (inner->thread_no)*p; i < N; i++){
                for(int j = 0; j < N; j++){
                    global_array[i][j] = compute_Cij(inner->input_array, i, j);
                }
            }
        }
        auto end = chrono::steady_clock::now();
        std::chrono::duration<double> duration = end - start;
        double iteration_time = duration.count();
        time_array[inner->thread_no] = iteration_time;
    }
    return NULL;
}

int main(){

    //Timer starts here 
    auto start = chrono::steady_clock::now();

    //Opening the Files
    fptr = fopen("inp.txt", "r");
    fp = fopen("output_chunk.txt", "w");

    //Checking for possible errors
    if(fp == NULL){
        perror("Error");
    }

    if(fptr == NULL){
        perror("Error");
    }
    fscanf(fptr, "%d %d %d %d", &N, &K, &C, &BT);

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
        if(pthread_create(thread_ptr + i, NULL, chunk_technique, bridge[i])){
            cout<<"Error in creating the thread "<<i<<endl;
            return 1;
        }
    }

    //Joining all the threads
    for(int i = 0; i < K; i++){
        if(pthread_join(thread_ptr[i], NULL)){
            cout<<"Error in joining the thread "<<i<<endl;
            return 2;
        }
    }

    //Outputting the contents of the global_array into the the output file named "output_mixed.txt"
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            fprintf(fp, "%d ", global_array[i][j]);
        }fprintf(fp, "\n");
    }
    
    //Deallocating memory already allocated
    delete []thread_ptr;

    //Timer ends here
    auto end = chrono::steady_clock::now();

    //Difference is calculated
    auto diff = end - start;
    auto seconds = std::chrono::duration<double>(diff); //Program Runtime
    fprintf(fp, "Program runtime: %.6f second\n", seconds.count());
       
    fprintf(fp, "Normal Threads Avg runtime: %.4f seconds\n", avg_of_sum_of_array(time_array, K));
    fprintf(fp, "Bounded Threads Avg runtime: %.4f seconds\n", avg_of_sum_of_array(time_array, BT));

    //Closing the file
    fclose(fp);

return 0;
}