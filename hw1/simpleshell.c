#include <stdio.h>
#include <string.h>

int parseInput(char * input, char splitWord[][500], int maxWords);
void changeDirectories();
void executeCommand();

int main() {
    char input[] = "Hello World";
    int maxWords = 10;
    char splitWord[maxWords][500];
    int numElements = parseInput(input, splitWord, maxWords);
    printf("%d\n", numElements);
    return 0;
}

int parseInput(char * input, char splitWord[][500], int maxWords) {
    char * token = strtok(input, " ");
    int i = 0;
    while (token != NULL & i < maxWords) {
        printf("%s\n", token);
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