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
    return;
}

void executeCommand() {
    return;
}