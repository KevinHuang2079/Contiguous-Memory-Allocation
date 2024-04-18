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

unsigned char *memory;
int memorySize;
GArray* array = g_array_new(FALSE, FALSE, sizeof(int));



struct Hole{
    int start;
    int end;
    int size;
};

void trimWhitespace(char* str) {
    int start = 0;
    int end = strlen(str) - 1;

    while (isspace(str[start])) {
        start++;
    }

    while (end > start && isspace(str[end])) {
        end--;
    }

    memmove(str, str + start, end - start + 1);
    str[end - start + 1] = '\0';
}

void FIRST_request(){
    if (){

    }
}

void BEST_request(){

}

void WORST_request(){

}


//Request for a contiguous block of memory.
void request(char* process, int requestAmount, char* allocationApproach){
    trimWhitespace(allocationApproach);
    if ( strcmp(allocationApproach, "F") == 0){
        FIRST_request();
    }
    else if ( strcmp(allocationApproach, "B") == 0){

    }
    else if ( strcmp(allocationApproach, "W") == 0){
    }
   
}

//Release of a contiguous block of memory.
void release(){

}

//Compact unused holes of memory into one single block.
void compact(){

}

//Report the regions of free and allocated memory
int main( int argc, char* argv[] ){
    memorySize = atoi(argv[1]);
    memory = malloc(memorySize*sizeof(unsigned char));

    while(1){
        printf("allocator> ");
        char line[30];
        fgets(line, sizeof(line), stdin);
        char* token = strtok(line, " ");
        int counter = 0;
        char** words;
        while (token != NULL){
            words[counter] = token;
            counter++;
            token = strtok(NULL, " ");
        }

        if (strcmp(words[0], "RQ") == 0){
            if (counter == 4){
                char* process = words[1];
                int requestAmount = atoi(words[2]);
                char* allocationApproach = words[3];

                request(process, requestAmount, allocationApproach);
            }

        }
        else if (strcmp(words[0], "RL") == 0){
            char* process = words[1];

        }
        else if (strcmp(words[0], "C") == 0){

        }
        else if (strcmp(words[0], "STAT") == 0){

        }
        else if (strcmp(word[0], "X") == 0){
            exit(0);
        }
        else{
            printf("Invalid entry");
        }
    }
    
}
