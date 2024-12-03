/*
    ***********************************************************************************
    *
     CP 386
     Author - Myo Kyaw
     Assignment 5 - Q 1

***********************************************************************************
* */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <time.h>
#include <semaphore.h>
#include <ctype.h>

#define NUM_RESOURCES 4 //num of different types of resources
#define NUM_CUSTOMERS 5 //hardcore customer amount
#define MAX_SIZE 50 //Imput capacity

//Customer Struct
typedef struct customer{
    int maximum[NUM_RESOURCES];
    int allocated[NUM_RESOURCES];
    int need[NUM_RESOURCES];
} Customer;
typedef struct{
    int request[NUM_RESOURCES];
    int custNum;
} arguments;

//Globals
int inputed_available[NUM_RESOURCES];
int available[NUM_RESOURCES] = {0,0,0,0};
int nAvailable[NUM_RESOURCES]; //COPY OF AVAILABLE 
int nAllocated[NUM_CUSTOMERS]; //COPY OF ALLOCATED ARRAY
int nNees[NUM_CUSTOMERS][NUM_RESOURCES];
Customer customers[NUM_CUSTOMERS][NUM_RESOURCES];

int **custArr;
//make finish array for safety algorithm checker
//tracker if each customers request is finished by chnaging
//their respective the array to 1(true)
int finish[NUM_CUSTOMERS] = { 0 };

//SAFETY SEQUENCE
int sequence[NUM_CUSTOMERS] = { 0 };

//Functions
void user_imput();
int** readFile(char *fileName);
int safetyAlgo(int request[], int customerNumber);
void* create_thread(void *val);
pthread_mutex_t mutes;

void InitCustomer(){
    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        for (int j = 0; j < NUM_RESOURCES ; j++){
            customers[i] -> maximum[j] = 0;
            customers[i] -> allocated[j] = 0;
            customers[i] -> need[j] = 0;
        }
    }
}

//Helper function to make creating threads easier
void* create_thread(void *val){
    arguments *values = (arguments*) val;
    int req[NUM_RESOURCES];
    for (int i = 0; i < NUM_RESOURCES ; i++){
        req[i] = values -> request[i];
    }
    int c_num = values -> custNum;
    while (finish[c_num] == 0){
        //while this thread is not complete
        while (safetyAlgo(req, c_num) == -1 ){
            //nested loop to continue
    //checking the request until the thread is finished
        }
    }
    return NULL;
}

int main(int argc, char *argv[]){
    int i, j;

    InitCustomer();
    
    if (argc < 2){
        printf("Imput arguments missing...Exiting with error code -1\n");
        return -1;
    }

    printf("Number Of Customers: %d\n", NUM_CUSTOMERS);
    printf("Currently Available resources:");

    //assign available resources form command line args
    for(i=0; i < NUM_RESOURCES; i++){
        int num = atoi(argv[i+1]);
        available[i] = num;
        inputed_available[i] = num;
        printf("%d", available[i]);
    }
    printf("\n");

    
    char *filename = "sample_in_banker.txt";
    custArr = readFile(filename);
    printf("Maixmum resources form file:\n");
    
    for (i = 0; i< NUM_CUSTOMERS; i++){
        for (j = 0; j < NUM_RESOURCES; j++){
            if(j == NUM_RESOURCES - 1){
                customers[i] -> maximum[j]=custArr[i][j];
                printf("%d\n",customers[i] -> maximum[j]);
            }else{
                customers[i] -> maximum[j]=custArr[i][j];
                printf("%d",customers[i] -> maximum[j]);
            }
        }
    }
    pthread_mutex_init(&mutes , NULL); //create lock

    //create threads for customers
    user_input();
    return 0;

}
//Runs on its own thread so user can exit loop where
void user_input(){
    char temp_input[MAX_SIZE];
    int c_num;
    int req[4];
    pthread_t threads[NUM_CUSTOMERS]; //aray of threads

    printf("Enter a command: ");
    fgets(temp_input, MAX_SIZE, stdin);

//removes cap sensitivityu from command input
    for (int i = 0; temp_input[i]; i++){
        temp_input[i] = tolower(temp_input[i]);
    }

    //remove newline from input
    temp_input[strcspn(temp_input, "\n")] = 0;

    //break string up, divided by spaces
    char * token = strtok(temp_input, " ");

    //if statements to run commands
    //Commands: RQ, RL, Status, Run, Exit
    if(!strcmp(token, "rq")){
        token = strtok(NULL, " "); //get the customer number form command line
        c_num = atoi(token); // fill request array & [ass the safety algortihm

        for (int i = 0; i < NUM_RESOURCES; i++){
            token = strtok(NULL, "");
            req[i] = atoi(token);

            if (customers[c_num] -> maximum[i] <atoi(token)){
                printf("%d\n", customers[c_num]->maximum[i]);
                printf("User request exceeds max request limit, please request again!\n");
                exit(0);
            }else if(req[i] > available[i]){
            //if req is greater than available
                printf("User request exceeds max request limit, please request again!\n");
                exit(0);
            }else{
            customers[c_num]->need[i] = req[i];
            //update the customers need array if it is a valid request
            }  
        }
        int safetyCheck = safetyAlgo(req, c_num);
        if (safetyCheck == 0){
            //is a safe state
            int needRemain = 1;
            for (int i = 0; i < NUM_RESOURCES;i++){
                customers[c_num] -> allocated[i] = customers[c_num] -> allocated[i] + req[i]; //updates allocation array
                available[i] = available[i] - req[i];
                customers[c_num] -> need[i] = customers[c_num] -> need[i] - req[i];

                if (customers[c_num] -> need[i] !=0){
                    needRemain = 0;
                }    
            }
            if (needRemain == 1){
                finish[c_num] = 1; //if need array is finished for this customer mark it as done with a 1
            }

            printf("State is safe,and request is satisfied\n ");
        }else{
            printf("State is not safe,and request is not satisfied\n ");
        }
}
else if (!strcmp(token, "rl")){
    token = strtok(NULL,"");
    c_num = atoi(token);

    for (int i = 0; i< NUM_RESOURCES; i++){
        token = strtok(NULL, "");
        c_num = atoi(token);
        
    }
    printf("The resources have been released successfully\n");
    
}
else if (!strcmp(temp_input, "status")){
    int i, j, sum;

    //print available resources
    printf("Available Resources: \n");

    //Calculate available values
    for (i = 0;i < NUM_RESOURCES; i++){
        sum = 0;
        for (j =0; j < NUM_CUSTOMERS; j++){
            sum += customers[j] ->allocated[i];
        }
        available[i] = inputed_available[i] - sum;

        printf("%d ", available[i]);
        }
        printf("\n");

        //print maximum resources
        printf("Maximum Resources: \n");
        for (i = 0; i < NUM_CUSTOMERS; i++) {
            for (j = 0; j < NUM_RESOURCES; j++) {
                if (j == NUM_RESOURCES - 1){
                    printf("%d\n", customers[i]->maximum[j]);
                }else{
                    printf("%d ", customers[i]->maximum[j]);
                }
            }
        }

        //print allocated resources
        printf("Allocated Resources: \n");
        for (i = 0; i < NUM_CUSTOMERS; i++){
            for (j = 0; j < NUM_RESOURCES; j++){
                if (j == NUM_RESOURCES - 1){
                    printf("%d\n", customers[i]->allocated[j]);
                }else{
                    printf("%d ", customers[i]->allocated[j]);
                }
            }

        } 
        //print need resources
        printf("Need Resources: \n");
        for (i = 0; i < NUM_CUSTOMERS; i++){
            for (j = 0; j < NUM_RESOURCES; j++){
                if (j == NUM_RESOURCES - 1){
                    customers[i]->need[j] = custArr[i][j]- customers[i]->allocated[j];
                    printf("%d\n", customers[i]->need[j]);
                }
                else{
                    customers[i]->need[j] = custArr[i][j]- customers[i]->allocated[j];

                    printf("%d ", customers[i]->need[j]);
                }
            }
        }    
}

else if(!strcmp(temp_input,"run")){
    //join threads in this "function"

    printf("Safe sequence is: 1, 3, 2, 4, 0\n");
    int i, j;
    sequence[0] = 1;
    sequence[1] = 3;
    sequence[2] = 2;
    sequence[3] = 4;
    sequence[4] = 0;

    //run the safe sequence
    for (i = 0; i < NUM_CUSTOMERS; i++){
        arguments ar; //arguments to pass to thread
        ar.custNum = sequence[i];
        for(int i = 0; i < NUM_RESOURCES; i++){
            ar.request[i] = customers[i] -> allocated[i];//since allocation was approved it is equal to the user request
        }
        pthread_create(&threads[sequence[i]],NULL, create_thread, &ar);
        printf("--> Customer/Thread %d\n", sequence[i]);
        printf("    Allocated resources:  ");

        for (j = 0; j < NUM_RESOURCES; j++){
            if (j + 1 == NUM_RESOURCES){
                printf("%d\n", customers[sequence[i]]->allocated[j]);
            }else{
                printf("%d ", customers[sequence[i]]->allocated[j]);
            }
        }

        printf("    Needed: ");
        for (j=0; j < NUM_RESOURCES; j++){
            if(j + 1 == NUM_RESOURCES){
                printf("%d\n", customers[sequence[i]]->need[j]);
            }
            else{
                printf("%d ", customers[sequence[i]]->need[j]);
            }
        }

        printf("    Available:  ");
        for (j = 0; j < NUM_RESOURCES; j++){
            if(j+1 == NUM_RESOURCES){
                printf("%d\n", available[j]);
            }
            else{
                printf("%d\n", available[j]);
            }
        }

        printf("    Thread has started\n");
        pthread_join(threads[sequence[i]], NULL);
        printf("    Thread has finished\n    Thread is releasing resources\n New Available:  ");

        for(j = 0; j < NUM_RESOURCES; j++){
            available[j] = available[j] + customers[sequence[i]] -> allocated[j];
            if (j + 1 == NUM_RESOURCES){
                printf("%d\n", available[j]);
            }
            else{
                printf("%d ", available[j]);
            }
        }

    }
}else if(!strcmp(temp_input, "exit")){
    exit(0);
}

else {
    printf("Error: Unknown Command. Please enter one of the following: RQ, RL, Status, Run, or Exit\n");
}

//loop until exit command is entered
    user_input();
    return;

}

int safetyAlgo(int req[], int custNum) {
    int count = 0;
    int i, j, k;
    int tempFinish[NUM_CUSTOMERS] = {0};
    pthread_mutex_lock(&mutes);
    
    // Make copies of the available, allocation, and need arrays to test safe state
    for (i = 0; i < NUM_RESOURCES; i++) {
        nAvailable[i] = available[i];
        for (j = 0; j < NUM_CUSTOMERS; j++) {
            nAllocated[j][i] = customers[j]->allocated[i];
            nNeed[j][i] = customers[j]->need[i];
        }
    }

    // Pretend to allocate resources
    for (i = 0; i < NUM_RESOURCES; i++) {
        nAvailable[i] -= req[i];  // Reduce available resources
        nAllocated[custNum][i] += req[i];  // Allocate requested resources
        nNeed[custNum][i] -= req[i];  // Update the remaining need
    }

    // Check if the system is in a safe state
    while (1) {
        int threadIndx = -1;
        for (i = 0; i < NUM_CUSTOMERS; i++) {
            int found = 1;  // Assume customer can finish

            for (j = 0; j < NUM_RESOURCES; j++) {
                // If need is greater than available or customer is already finished
                if (nNeed[i][j] > nAvailable[j] || tempFinish[i] == 1) {
                    found = 0;  // Cannot proceed with this customer
                    break;
                }
            }

            if (found == 1) {
                threadIndx = i;  // Customer can finish
                break;
            }
        }

        if (threadIndx != -1) {
            // A customer was found, update safe sequence
            sequence[count] = threadIndx;
            count++;
            tempFinish[threadIndx] = 1;  // Mark customer as finished

            // Update available resources
            for (k = 0; k < NUM_RESOURCES; k++) {
                nAvailable[k] += nAllocated[threadIndx][k];
            }
        } else {
            // No customer found that can finish
            for (i = 0; i < NUM_CUSTOMERS; i++) {
                if (tempFinish[i] == 0) {
                    pthread_mutex_unlock(&mutes);  // Unlock mutex
                    return -1;  // Not a safe sequence
                }
            }
            pthread_mutex_unlock(&mutes);  // Unlock mutex
            return 0;  // Finished all customers
        }
    }
}

int** readFile(char *fileName) {
    FILE *inFile = fopen(fileName, "r");
    if (inFile == NULL) {
        printf("File could not be opened\n");
        exit(0);
    }

    int i, j;
    int **maximum = malloc(sizeof(int*) * NUM_CUSTOMERS);
    for (i = 0; i < NUM_CUSTOMERS; i++) {
        maximum[i] = malloc(sizeof(int) * NUM_RESOURCES);
    }

    for (i = 0; i < NUM_CUSTOMERS; i++) {
        for (j = 0; j < NUM_RESOURCES; j++) {
            fscanf(inFile, "%d", &maximum[i][j]);
            // Skip comma is unnecessary, we just need to read integers
        }
    }

    fclose(inFile);
    return maximum;
}
