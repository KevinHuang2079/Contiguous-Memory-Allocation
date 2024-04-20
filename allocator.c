#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <pthread.h>
#include <math.h>
#include <ctype.h>
#include <glib.h>
#include <limits.h>
#include <stdbool.h>

unsigned char *memory;
int memorySize;
GArray* holeList;
GHashTable* hashmap; //key = processes, value = array int of start, end ranges

void printMemory() {
    printf("    Memory Array:\n");
    for (unsigned int i = 0; i < memorySize; i++) {
        printf("%d ", memory[i]);
    }
    printf("\n");
}

void printHoleList(){
    printf("    Hole List:\n");
    for (unsigned int i = 0; i < holeList->len; i++) {
        int* currHole = g_array_index(holeList, int*, i);
        printf("    Hole %d: Start=%d, End=%d, Size=%d\n", i + 1, currHole[0], currHole[1], currHole[1]-currHole[0]+1);
    }
}

void trimWhitespace(char* str) {
    int start = 0;
    int end = strlen(str) - 1;
    //find where the first char starts
    while (isspace(str[start])) {
        start++;
    }
    //find where the end (the first char moving backwards)
    while (end > start && isspace(str[end])) {
        end--;
    }
    //move it into that new spot
    memmove(str, str + start, end - start + 1);
    //delete slot so that anything after isn't considered to be part of the new string
    str[end - start + 1] = '\0';
}

void fillMemory(int start, int size){
    for (unsigned int i = start; i < start+size ; ++i){
        memory[i] = true; //true for a full slot 
    }
    printMemory();
}


void updateHoles(){
    holeList->len = 0;
    int start = -1;
    int end = -1;
    for (unsigned int i = 0; i < memorySize ; ++i){
        //set start
        if ( memory[i] == false ){ 
            if (start == -1) {
                start = i;
            }
        }
        //set end
        if ( memory[i] == true ){
            end = i;
            if (end >= start && start != -1) {
                int* hole = malloc(sizeof(int)*2);
                hole[0] = start;
                hole[1] = end-1; //index before the taken slot
                g_array_append_val(holeList, hole);
                start = -1;//reset start
            }
        }
        else if (i == memorySize-1){
                end = i;
                int* hole = malloc(sizeof(int)*2);
                hole[0] = start;
                hole[1] = end; 
                g_array_append_val(holeList, hole);
        }   
    }
    printHoleList();
}


void FIRST_request(char* process, int requestAmount, char* allocationApproach){
    if (holeList->len == 0){
        printf("No free holes, release memory\n");
    }
    for (unsigned int i = 0; i < holeList->len; i++) {
        int* currHole = g_array_index(holeList, int*, i);
        int currStart = currHole[0];
        int currEnd = currHole[1];
        printf("currHoleSize=%d, requestAmount=%d\n", currHole[1]-currHole[0]+1, requestAmount);
        if (requestAmount <= currEnd - currStart+1){
            printf("available space, adding process...\n");
            //add the process to the hashmap
            int* processRange = malloc(sizeof(int)*2);
            processRange[0] = currStart;
            processRange[1] = currStart + requestAmount;
            g_hash_table_insert(hashmap, g_strdup(process), processRange);

            printf("updating memory...\n");
            //update the memory
            printf("    added processRange: %d, %d\n", processRange[0], processRange[1]);
            fillMemory(processRange[0], requestAmount);
            updateHoles();
            break;
        }
        else{
            printf("No available space\n");
        }
    }
}

void BEST_request(char* process, int requestAmount, char* allocationApproach){
    if (holeList->len == 0){
        printf("No free holes, release memory\n");
    }

    int minSize = INT_MAX;
    int minStart;

    //find the smallest Hole
    for (unsigned int i = 0; i < holeList->len; i++) {
        int* currHole = g_array_index(holeList, int*, i);
        int currStart = currHole[0];
        int currEnd = currHole[1];
        int currSize = currEnd - currStart +1;
        printf("i: %d currSize: %d\n", i, currSize);
        if (currSize < minSize && requestAmount <= minSize){
            minSize = currSize;
            minStart = currStart;
        }
    }

    //found available slot
    if (requestAmount <= minSize){
        printf("available space, adding process...\n");
        //add the process to the hashmap
        int* processRange = malloc(sizeof(int)*2);
        processRange[0] = minStart;
        processRange[1] = minStart + requestAmount;
        g_hash_table_insert(hashmap, g_strdup(process), processRange);

        printf("updating memory...\n");
        //update the memory
        printf("    added processRange: %d, %d\n", processRange[0], processRange[1]);
        fillMemory(processRange[0], requestAmount);
        updateHoles();
    }
    else{
        printf("No available space\n");
    }
}

void WORST_request(char* process, int requestAmount, char* allocationApproach){
    if (holeList->len == 0){
        printf("No free holes, release memory\n");
    }

    int maxSize = INT_MIN;
    int maxStart;

    //find the largest Hole 
    for (unsigned int i = 0; i < holeList->len; i++) {
        int* currHole = g_array_index(holeList, int*, i);
        int currStart = currHole[0];
        int currEnd = currHole[1];
        int currSize = currEnd - currStart +1;
        if (currSize > maxSize){
            maxSize = currSize;
            maxStart = currStart;
        }
    }
    
    //found available slot
    if (requestAmount <= maxSize){
        printf("available space, adding process...\n");
        //add the process to the hashmap
        int* processRange = malloc(sizeof(int)*2);
        processRange[0] = maxStart;
        processRange[1] = maxStart + requestAmount;
        g_hash_table_insert(hashmap, g_strdup(process), processRange);

        printf("updating memory...\n");
        //update the memory
        printf("    added processRange: %d, %d\n", processRange[0], processRange[1]);
        fillMemory(processRange[0], requestAmount);
        updateHoles();
    }
    else{
        printf("No available space\n");
    }
}


//Request for a contiguous block of memory.
void request(char* process, int requestAmount, char* allocationApproach){
    if ( strcmp(allocationApproach, "F") == 0){
        FIRST_request(process, requestAmount, allocationApproach);
    }
    else if ( strcmp(allocationApproach, "B") == 0){
        BEST_request(process, requestAmount, allocationApproach);
    }
    else if ( strcmp(allocationApproach, "W") == 0){
        WORST_request(process, requestAmount, allocationApproach);
    }
    else {
        printf("request failed\n");
    }
   
}

//Release of a contiguous block of memory.
void release(char* process){
    if ( g_hash_table_lookup(hashmap, process) != NULL){ //make sure the process exists
        int* processRange = g_hash_table_lookup(hashmap, process);
        int processStart = processRange[0];
        int processEnd = processRange[1];

        //0 out the memory block
        for (unsigned int i = processStart; i < processEnd ; ++i){
            memory[i] = false;
        }

        //update Holes
        updateHoles();

        //delete the process in hashmap
        gboolean removed = g_hash_table_remove(hashmap, process);
        if (removed) {
            printf("Process %s released\n", process);
        } else {
            printf("Failed to remove process %s\n", process);
        }
        printMemory();
        printHoleList();
    }
    else {
        printf("No process %s\n", process);
        printMemory();
        printHoleList();
    }
}

//Compact unused holes of memory into one single block.
void compact(){
    int lowestStart = INT_MAX;
    //loop through processes and if one of them has start = 0
    for ( unsigned int i = 0 ; i < hashmap->len ; ++i ){
        if ( )
    }
    
    //if yes then loop through and find the lowest start number and have it start at the lowest end 
    //if no then loop through and find the lowest start number and have it start at 0
}

//Report the regions of free and allocated memory
int main( int argc, char* argv[] ){
    memorySize = atoi(argv[1]);
    memory = malloc(memorySize*sizeof(bool));

    int* startingHole = malloc(sizeof(int)*2);
    startingHole[0] = 0; 
    startingHole[1] = memorySize - 1;

    holeList = g_array_new(FALSE, FALSE, sizeof(int)*2);
    g_array_append_val(holeList, startingHole);

    hashmap = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL); 

    while(1){
        printf("allocator> ");
        char line[50];
        fgets(line, sizeof(line), stdin);
        char* token = strtok(line, " ");
        int counter = 0;
        char** words;
        while (token != NULL) {
            words[counter] = malloc(strlen(token) + 1); //+1 for the null character from trimWhitespace()
            strcpy(words[counter], token);
            trimWhitespace(words[counter]);
            counter++;
            token = strtok(NULL, " ");
        }

        //REQUEST
        if (strcmp(words[0], "RQ") == 0){
            if (counter == 4){
                char* process = words[1];
                int requestAmount = atoi(words[2]);
                char* allocationApproach = words[3];
                if ( g_hash_table_lookup(hashmap, process) == NULL ){
                    request(process, requestAmount, allocationApproach);
                }
                else {
                    printf("process %s is in already in memory\n", process);
                }

            }
            else{
                printf("RQ parameters: allocator> <RQ> <process> <requestMemoryAmount> <allocationAlgorithm>\n");
            }
        }

        //RELEASE
        else if (strcmp(words[0], "RL") == 0){
            char* process = words[1];
            release(process);
        }

        //
        else if (strcmp(words[0], "C") == 0){

        }

        //
        else if (strcmp(words[0], "STAT") == 0){

        }
        else if (strcmp(words[0], "X") == 0){
            printf("exiting...");
            exit(0);
        }
        else{
            printf("Invalid entry\n");
        }
    }
    
}
