#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define SIZE 5
#define THREAD_MAX 2

int array[SIZE];
int opt_a;

//thread control 
struct threadControl{
    int threadCon_low;
    int threadCon_high;
    int threadCon_no;
};

void fillArrayWithRandomNumbers(int arr[SIZE]) {
    for(int i = 0; i<SIZE; i++) array[i] = rand()%100;
}

void printArray(int arr[SIZE]) {
    for(int i = 0; i<SIZE; i++) printf("%5d", arr[i]);
    printf("\n");
}


//Function to merge two sides together
void merger(int low, int middle, int high) 
{  
    int size1 = middle - low + 1;
    int size2 = high - middle, i, j; 
        
    int* left = malloc(size1 * sizeof(int)); 
    int* right = malloc(size2 * sizeof(int));
   
    //Store values in the left side
    for (i = 0; i < size1; i++) 
        left[i] = array[i + low]; 
    
    //Store values in the right side
    for (i = 0; i < size2; i++) 
        right[i] = array[i + middle + 1]; 
  
    int k = low; 
    i = j = 0; 
  
    // Merge the left and right sides together 
    while (i < size1 && j < size2) { 
        if (left[i] <= right[j]) 
            array[k++] = left[i++]; 
        else
            array[k++] = right[j++]; 
    } 
  
    // Insert any remaining values from the left and right 
    while (i < size1) { 
        array[k++] = left[i++]; 
    }  
    while (j < size2) { 
        array[k++] = right[j++]; 
    } 
    
    //Deallocate memory 
    free(left);
    free(right);
} 

// Function that performs merge sort algorithm
void mergeSortAlg(int low, int high) 
{  
    //Find the array midpoint
    int middle = low + (high - low) / 2; 
    if (low < high) {  
        //Call function with first half of array
        mergeSortAlg(low, middle);
        //Call function with second half of array
        mergeSortAlg(middle + 1, high);
        //Merge the two sides together
        merger(low, middle, high); 
    } 
}

// Runs mergesort on the array segment described in the
// argument. Spawns two threads to mergesort each half
// of the array segment, and then merges the results.
void* mergeSort(void* args) {
    
    struct threadControl *threadControl = args;
  
    int low = threadControl->threadCon_low;
    int high = threadControl->threadCon_high; 
  
    int middle = low + (high - low) / 2;
    
    if (low < high) { 
        mergeSortAlg(low, middle); 
        mergeSortAlg(middle + 1, high); 
        merger(low, middle, high); 
    }
}

int main() {
    srand(time(0));
    struct threadControl *threadControl;
    opt_a = 1;
    
    // 1. Fill array with random numbers.
    fillArrayWithRandomNumbers(array);
    
    // 2. Print the array.
    printf("Unsorted array: ");
    printArray(array);
    
    // 3. Create a thread for merge sort.
    pthread_t threads[THREAD_MAX]; 
    struct threadControl threadConList[THREAD_MAX];
    int len = SIZE / THREAD_MAX;
    int low = 0;
    
    for (int i = 0; i < THREAD_MAX; i++, low += len) {
        threadControl = &threadConList[i];
        threadControl->threadCon_no = i;

        if (opt_a) {
            threadControl->threadCon_low = low;
            threadControl->threadCon_high = low + len - 1;
            if (i == (THREAD_MAX - 1))
                threadControl->threadCon_high = SIZE - 1;
        }

        else {
            threadControl->threadCon_low = i * (SIZE / THREAD_MAX);
            threadControl->threadCon_high = (i + 1) * (SIZE / THREAD_MAX) - 1;
        }
    }
    
    //Creating 2 threads
    for (int i = 0; i < THREAD_MAX; i++){ 
        threadControl = &threadConList[i];
        pthread_create(&threads[i], NULL, mergeSort, threadControl); 
    } 
    //Join the threads
    for (int i = 0; i < THREAD_MAX; i++){
        pthread_join(threads[i], NULL);
    } 
    
    //merge both parts
    if (opt_a) {
        struct threadControl *tcb = &threadConList[0];
        for (int i = 1; i < THREAD_MAX; i++) {
            struct threadControl *threadControl = &threadConList[i];
            merger(tcb->threadCon_low, threadControl->threadCon_low - 1, threadControl->threadCon_high);
        }
    }
    else {
        merger(0, (SIZE / 2 - 1) / 2, SIZE / 2 - 1);
        merger(SIZE / 2, SIZE / 2 + (SIZE - 1 - SIZE / 2) / 2, SIZE - 1);
        merger(0, (SIZE - 1) / 2, SIZE - 1);
    } 
    
    // 4. Wait for mergesort to finish.
    
    // 5. Print the sorted array.
    printf("Sorted array:   ");
    printArray(array);
}