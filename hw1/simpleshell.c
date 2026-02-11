#include <stdio.h>
#include <string.h>
#include <unistd.h>

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
            // pass
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