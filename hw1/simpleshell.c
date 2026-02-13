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
void changeDirectories(const char * path);
enum CommandType detectCommandType(char splitWord[][500], int maxWords, int * redirectionIdx);
int createCommand(char *** command, char splitWord[][500], int start, int end);
void freeCommandMem(char *** command, int size);
void displayCommand(char *** command, int size);
int executeCommand(char * const* commannd, const char* infile, const char* outfile, char * const* command_2);

void handleCd(char splitWord[][500], char cwd[], int size);
void handleSimpleCommand(char splitWord[][500], int maxWords);
void handleInputRedirection(char splitWord[][500], int maxWords, int redirectionIdx);
void handleOutputRedirection(char splitWord[][500], int maxWords, int redirectionIdx);
void handlePipe(char splitWord[][500], int maxWords, int redirectionIdx);

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
            case CMD_CD: handleCd(splitWord, cwd, sizeof(cwd)); break;
            case CMD_SIMPLE: handleSimpleCommand(splitWord, maxWords); break;
            case CMD_INPUT_REDIRECTION: handleInputRedirection(splitWord, maxWords, redirectionIdx); break;
            case CMD_OUTPUT_REDIRECTION: handleOutputRedirection(splitWord, maxWords, redirectionIdx); break;
            case CMD_PIPE: handlePipe(splitWord, maxWords, redirectionIdx); break;
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

void changeDirectories(const char *path) {

    if (path == NULL || path[0] == '\0') {
        fprintf(stderr, "chdir Failed: %s\n", strerror(errno));
        return;
    }

    if (chdir(path) == -1) {
        fprintf(stderr, "chdir Failed: %s\n", strerror(errno));
    }
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

int createCommand(char *** command, char splitWord[][500], int start, int end) {
    // printf("\ncreateCommand called\n");
    int size = 0;
    for (int i = start; i < end; i++) {
        if (splitWord[i][0] == 0) {
            break;
        }
        size++;
    }
    *command = malloc((size + 1) * sizeof(char*));

    for (int i = 0; i < size; i++) {
        (*command)[i] = malloc(500 * sizeof(char));
        strcpy((*command)[i], splitWord[start + i]);
    }
    (*command)[size] = NULL;

    // displayCommand(command, size);

    // printf("%s\n\n", (*command)[size]);
    return size;
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

int executeCommand(
    char * const* command, 
    const char* infile, 
    const char* outfile, 
    char * const* command_2
) {

    int pipe_fds[2];
    pid_t pid1, pid2;
    int status;

    int isPipe = (command_2 != NULL);

    if (isPipe) {
        if (pipe(pipe_fds) == -1) {
            perror("pipe failed");
            return -1;
        }
    }

    pid1 = fork();
    if (pid1 < 0) {
        fprintf(stderr, "fork Failed: %s\n", strerror(errno));
        return -1;
    }

    if (pid1 == 0) {
        if (infile && infile[0] != '\0') {
            int fd_in = open(infile, O_RDONLY);
            if (fd_in < 0) {
                fprintf(stderr, "open infile Failed: %s\n", strerror(errno));
                _exit(1);
            }
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
        }

        if (outfile && outfile[0] != '\0' && !isPipe) {
            int fd_out = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (fd_out < 0) {
                fprintf(stderr, "open outfile Failed: %s\n", strerror(errno));
                _exit(1);
            }
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
        }

        if (isPipe) {
            dup2(pipe_fds[1], STDOUT_FILENO);
            close(pipe_fds[0]);
            close(pipe_fds[1]);
        }

        execvp(command[0], command);
        fprintf(stderr, "exec Failed: %s\n", strerror(errno));
        _exit(1);
    }

    if (isPipe) {
        pid2 = fork();
        if (pid2 < 0) {
            fprintf(stderr, "fork Failed: %s\n", strerror(errno));
            return -1;
        }

        if (pid2 == 0) {
            dup2(pipe_fds[0], STDIN_FILENO);
            close(pipe_fds[0]);
            close(pipe_fds[1]);

            if (outfile && outfile[0] != '\0') {
                int fd_out = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                if (fd_out < 0) {
                    fprintf(stderr, "open outfile Failed: %s\n", strerror(errno));
                    _exit(1);
                }
                dup2(fd_out, STDOUT_FILENO);
                close(fd_out);
            }

            execvp(command_2[0], command_2);
            fprintf(stderr, "exec Failed: %s\n", strerror(errno));
            _exit(1);
        }

        close(pipe_fds[0]);
        close(pipe_fds[1]);
        waitpid(pid1, &status, 0);
        waitpid(pid2, &status, 0);
    } else {
        waitpid(pid1, &status, 0);
    }

    return 0;
}

void handleCd(char splitWord[][500], char cwd[], int size) {

    if (splitWord[1][0] == 0) {
        fprintf(stderr, "chdir Failed: No path specified\n");
        return;
    }

    changeDirectories(splitWord[1]);

    getcwd(cwd, size);
}


void handleSimpleCommand(char splitWord[][500], int maxWords) {
    // printf("\nhandleSimpleCommand called\n");
    char ** command = NULL;
    int size = createCommand(&command, splitWord, 0, maxWords);
    
    executeCommand(command, NULL, NULL, NULL);

    freeCommandMem(&command, size);
}

void handleInputRedirection(char splitWord[][500], int maxWords, int redirectionIdx) {
    // printf("\nhandleInputRedirection called\n");
    char ** command = NULL;
    int size = createCommand(&command, splitWord, 0, redirectionIdx);

    char infile[500] = "";
    strcpy(infile, splitWord[redirectionIdx + 1]);

    executeCommand(command, infile, NULL, NULL);

    freeCommandMem(&command, size);
}

void handleOutputRedirection(char splitWord[][500], int maxWords, int redirectionIdx) {
    // printf("\nhandleOutputRedirection called\n");
    char ** command = NULL;
    int size = createCommand(&command, splitWord, 0, redirectionIdx);

    char outfile[500] = "";
    strcpy(outfile, splitWord[redirectionIdx + 1]);

    executeCommand(command, NULL, outfile, NULL);

    freeCommandMem(&command, size);
}

void handlePipe(char splitWord[][500], int maxWords, int redirectionIdx) {
    // printf("\nhandlePipe called\n");
    char ** command_1 = NULL;
    int size_1 = createCommand(&command_1, splitWord, 0, redirectionIdx);

    char ** command_2 = NULL;
    int size_2 = createCommand(&command_2, splitWord, redirectionIdx + 1, maxWords);

    executeCommand(command_1, NULL, NULL, command_2);

    freeCommandMem(&command_1, size_1);
    freeCommandMem(&command_2, size_2);
    
}