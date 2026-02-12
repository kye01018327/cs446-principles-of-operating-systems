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
enum CommandType detectCommandType(char splitWord[][500], int maxWords, int * redirectionIdx);
int createCommand(char *** command, char splitWord[][500], int maxWords);
void freeCommandMem(char *** command, int size);
void displayCommand(char *** command, int size);
void executeCommand();

void handleCd(char cwd[], int size);
void handleSimpleCommand(char splitWord[][500], int maxWords);
void handleInputRedirection(char splitWord[][500], int maxWords, int redirectionIdx);
void handleOutputRedirection(char splitWord[][500], int maxWords, int redirectionIdx);
void handlePipe();

enum CommandType {
    CMD_CD,
    CMD_EXIT,
    CMD_SIMPLE,
    CMD_INPUT_REDIRECTION,
    CMD_OUTPUT_REDIRECTION,
    CMD_PIPE
};

int main() {
    char netid[] = "kye";
    char cwd[500];
    getcwd(cwd, sizeof(cwd));

    int maxWords = 10;
    char splitWord[maxWords][500];
    memset(splitWord, 0, sizeof(splitWord));

    char userInput[500];
    int redirectionIdx = 0;
    while (1) {
        printf("%s:%s$ ", netid, cwd);

        scanf(" %[^\n]", userInput);
        parseInput(userInput, splitWord, maxWords);
        enum CommandType cmdType = detectCommandType(splitWord, maxWords, &redirectionIdx);

        switch (cmdType) {
            case CMD_EXIT: return 0;
            case CMD_CD: handleCd(cwd, sizeof(cwd)); break;
            case CMD_SIMPLE: handleSimpleCommand(splitWord, maxWords); break;
            case CMD_INPUT_REDIRECTION: handleInputRedirection(splitWord, maxWords, redirectionIdx); break;
            case CMD_OUTPUT_REDIRECTION: handleOutputRedirection(splitWord, maxWords, redirectionIdx); break;
            case CMD_PIPE: handlePipe(); break;
        }
        memset(splitWord, 0, sizeof(splitWord));
    }
}

int parseInput(char * input, char splitWord[][500], int maxWords) {
    char * token = strtok(input, " ");
    int i = 0;
    while (token != NULL && i < maxWords) {
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

enum CommandType detectCommandType(char splitWord[][500], int maxWords, int * redirectionIdx) {
    char * firstElement = splitWord[0];
    if (strcmp(firstElement, "exit") == 0) {
        return CMD_EXIT;
    }
    if (strcmp(firstElement, "cd") == 0) {
        return CMD_CD;
    }
    for (int i = 0; i < maxWords; i++) {
        if (strcmp(splitWord[i], "<") == 0) {
            *redirectionIdx = i;
            return CMD_INPUT_REDIRECTION;
        }
        if (strcmp(splitWord[i], ">") == 0) {
            *redirectionIdx = i;
            return CMD_OUTPUT_REDIRECTION;
        }
        if (strcmp(splitWord[i], "|") == 0) {
            *redirectionIdx = i;
            return CMD_PIPE;
        }
    }
    return CMD_SIMPLE;
    
}

int createCommand(char *** command, char splitWord[][500], int maxWords) {
    printf("\ncreateCommand called\n");
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
    *command = malloc((numValidElements + 1) * sizeof(char*));
    if (*command == NULL) {
        printf("Memory allocation to 'command' array failed");
        return 1;
    }
    for (int i = 0; i < numValidElements; i++) {
        (*command)[i] = malloc(500 * sizeof(char));
        strcpy((*command)[i], splitWord[i]);
    }
    (*command)[numValidElements] = NULL;

    displayCommand(command, numValidElements);

    printf("%s\n\n", (*command)[numValidElements]);
    return numValidElements;
}

void freeCommandMem(char *** command, int size) {
    for (int i = 0; i < size; i++) {
        free((*command)[i]);
    }
    free(*command);
}

void displayCommand(char *** command, int size) {
    printf("\ndisplayCommand called\n");
    for (int i = 0; i < size; i++) {
        printf("%s ", (*command)[i]);
        for (int j = 0; j < 500; j++) {
            printf("%d ", (*command)[i][j]);
            if ((*command)[i][j] == 0) {
                printf("\n");
                break;
            }
        }
    }
}

void executeCommand() {
    printf("executeCommand function called\n");
    return;
}

void handleCd(char cwd[], int size) {
    printf("\nhandleCd called\n");
    changeDirectories();
    getcwd(cwd, size);
}

void handleSimpleCommand(char splitWord[][500], int maxWords) {
    printf("\nhandleSimpleCommand called\n");
    char ** command = NULL;
    int size = createCommand(&command, splitWord, maxWords);
    executeCommand();
    freeCommandMem(&command, size);
}

void handleInputRedirection(char splitWord[][500], int maxWords, int redirectionIdx) {
    printf("\nhandleInputRedirection called\n");
    char ** command = NULL;
    int size = createCommand(&command, splitWord, redirectionIdx);
    freeCommandMem(&command, size);
}

void handleOutputRedirection(char splitWord[][500], int maxWords, int redirectionIdx) {
    printf("\nhandleOutputRedirection called\n");
    char ** command = NULL;
    int size = createCommand(&command, splitWord, redirectionIdx);
    freeCommandMem(&command, size);
}

void handlePipe() {
    printf("\nhandlePipe called\n");
}