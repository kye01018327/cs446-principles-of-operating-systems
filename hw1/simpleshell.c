#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

int parseInput(char * input, char splitWord[][500], int maxWords);
void changeDirectories();
void executeCommand();

int main() {
    char netid[] = "kye";
    char cwd[500];
    getcwd(cwd, sizeof(cwd));

    int maxWords = 10;
    char splitWord[maxWords][500];
    memset(splitWord, 0, sizeof(splitWord));

    char userInput[500];
    int hasInputRedirection = 0;
    int hasOutputRedirection = 0;
    int hasPipe = 0;

    while (1) {
        printf("%s:%s$ ", netid, cwd);
        scanf(" %[^\n]", userInput);
        printf("User Input: %s\n", userInput);
        parseInput(userInput, splitWord, maxWords);

        char * firstElement = splitWord[0];
        // if first element is cd, call changeDirectories
        if (strcmp(firstElement, "cd") == 0) {
            changeDirectories();
        }
        // if first element is exit, stop looping and return from main with 0 return value
        else if (strcmp(firstElement, "exit") == 0) {
            return 0;
        }
        // check if contains input redirection (<) or output redirection (>)
        for (int i = 0; i < maxWords; i++) {
            if (strcmp(splitWord[i], "<") == 0) {
                hasInputRedirection = 1;
                break;
            }
            if (strcmp(splitWord[i], ">") == 0) {
                hasOutputRedirection = 1;
                break;
            }
            if (strcmp(splitWord[i], "|") == 0) {
                hasPipe = 1;
                break;
            }
        }
        if (hasInputRedirection) {
            printf("Input redirection detected (<)\n");
            hasInputRedirection = 0;
        }
        else if (hasOutputRedirection) {
            printf("Output redirection detected (>)\n");
            hasOutputRedirection = 0;
        }
        else if (hasPipe) {
            printf("Pipe redirection detected (|)\n");
            hasPipe = 0;
        }
        // if no redirection (single CLI command)
        else {
            // Dynamically allocated c-string array
            // one more than the number of valid CLI elements
            int numValidElements = 0;
            for (int i = 0; i < maxWords; i++) {
                if (splitWord[i][0] == 0) {
                    break;
                }
                numValidElements++;
            }
            // printf("num valid elements: %d\n", numValidElements);
            char ** a = malloc((numValidElements + 1) * sizeof(char*));
            if (a == NULL) {
                printf("Memory allocation to 'a' array failed");
                return 1;
            }
            for (int i = 0; i < numValidElements; i++) {
                a[i] = malloc(500 * sizeof(char));
                strcpy(a[i], splitWord[i]);
            }
            a[numValidElements] = NULL;

            printf("\nDYNAMICALLY ALLOCATED ARRAY CONTENTS------\n");
            for (int i = 0; i < numValidElements; i++) {
                printf("%s ", a[i]);
                for (int j = 0; j < 500; j++) {
                    printf("%d ", a[i][j]);
                    if (a[i][j] == 0) {
                        printf("\n");
                        break;
                    }
                }
            }
            printf("%s\n\n", a[numValidElements]);

            // free memory
            for (int i = 0; i < numValidElements + 1; i++) {
                free(a[i]);
            }
            free(a);
        }

        
        // debug
        printf("Parsed Input:");
        for (int i = 0; i < maxWords; i++) {
            printf(" %s", splitWord[i]);
        }
        printf("\n");

        // reset
        memset(splitWord, 0, sizeof(splitWord));
    }


    return 0;
}

int parseInput(char * input, char splitWord[][500], int maxWords) {
    char * token = strtok(input, " ");
    int i = 0;
    while (token != NULL & i < maxWords) {
        strcpy(splitWord[i], token);
        i++;
        token = strtok(NULL, " ");
    }
    return i;
}

void changeDirectories() {
    printf("changeDirectories called\n");
    return;
}

void executeCommand() {
    return;
}