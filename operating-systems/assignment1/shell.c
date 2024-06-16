// Nikolaos Kontogeorgis - csd4655

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LENGTH 1024
#define YB "\033[1;33m"
#define GB "\033[1;32m"
#define W "\033[0;37m"
#define WB "\033[1;37m"

char* user;
char* dir;

char* remove_quotes(char* s) {
    while (strchr(s, '"') != NULL)
        memmove(strchr(s, '"'),
                strchr(s, '"') + 1,
                strlen(strchr(s, '"')));

    while (strchr(s, '\'') != NULL)
        memmove(strchr(s, '\''),
                strchr(s, '\'') + 1,
                strlen(strchr(s, '\'')));

    return s;
}

void print_prompt() {
    printf("%scsd4655-hy345sh@%s%s", YB, user, W);
    printf(":%s%s> %s", GB, dir, W);
}

char** tokenized(char* string, char* delim) {
    char** tokens = malloc(MAX_LENGTH * sizeof(char*));
    char* token;
    int i = 0;

    token = strtok(string, delim);

    for (i = 0; token != NULL; i++) {
        tokens[i] = token;
        token = strtok(NULL, delim);
    }
    tokens[i] = NULL;
    free(token);

    return tokens;
}

int cd(char* dest) {
    if (dest == NULL) {
        // If no arguments given, change to home directory
        chdir(getenv("HOME"));
    }
    else {
        // Change to given directory
        if (chdir(dest) != 0) {
            perror("csd4655-hy345sh");
            return 1;
        }
    }
    dir = getcwd(NULL, 0);
    if (dir == NULL)
        dir = "(unknown)";
    return 0;
}

int exec(char** args, int fd_in, int fd_out) {
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Child
        if (fd_in != 0) {
            dup2(fd_in, 0);
            close(fd_in);
        }
        if (fd_out != 1) {
            dup2(fd_out, 1);
            close(fd_out);
        }

        if (execvp(args[0], args) == -1)
            perror("csd4655-hy345sh: execvp");

        exit(1);
    }
    else if (pid > 0)
        // Parent
        wpid = waitpid(pid, &status, 0);
    else {
        // Error forking
        perror("csd4655-hy345sh: fork");
        return 1;
    }

    return wpid;
}

int exec_single(char *command, int fd_in, int fd_out) {
    char** args = tokenized(command, " ");
    for (int i = 0; args[i] != NULL; i++)
        args[i] = remove_quotes(args[i]);

    if (args[0] == NULL)
        return 1;

    // Check if command is exit or quit
    if (!strcmp(args[0], "exit") || !strcmp(args[0], "quit"))
        exit(0);

    // Check if command is cd or chdir
    if (!strcmp(args[0], "cd") || !strcmp(args[0], "chdir"))
        return cd(args[1]);

    // Check if command has input redirection
    for (int i = 0; args[i] != NULL; i++) {
        args[i] = args[i];

        if (args[i][0] == '<') {
            args[i] = NULL;
            fd_in = open(args[++i], O_RDONLY);
            if (fd_in < 0) {
                perror("csd4655-hy345sh: open");
                return 1;
            }
        }
        else if (args[i][0] == '>') {
            int oflag;
            if (args[i][1] == '>')
                oflag = O_WRONLY | O_CREAT | O_APPEND;
            else
                oflag = O_WRONLY | O_CREAT | O_TRUNC;

            args[i] = NULL;
            fd_out = open(args[++i], oflag, 0777);
            if (fd_out < 0) {
                perror("csd4655-hy345sh: open");
                return 1;
            }
        }
    }

    exec(args, fd_in, fd_out);

    if (fd_in != 0)
        close(fd_in);

    if (fd_out != 1)
        close(fd_out);

    return 0;
}

int exec_command(char *command) {
    // Split command into multiple commands by pipe
    char **sub_commands = tokenized(command, "|");

    if (sub_commands[0] == NULL)
        return 1;

    int fd_in = 0, fd_out = 1;  // Default to stdin and stdout
    int p[2];

    // No pipes
    if (sub_commands[1] == NULL) {
        exec_single(sub_commands[0], fd_in, fd_out);
        return 0;
    }

    for (int i = 0; sub_commands[i] != NULL; i++) {
        char* next = sub_commands[i + 1];

        if (i == 0) {               // No pipe before
            if (pipe(p) < 0)
                exit(1);

            fd_out = p[1];
        }
        else if (next == NULL) {    // No pipe after
            fd_in = p[0];
            fd_out = 1;
        }
        else {                      // Pipe before and after
            char* curr = sub_commands[i];

            if (((strchr(curr, '<') != NULL) && i != 0) ||
                ((strchr(curr, '>') != NULL))) {
                perror("csd4655-hy345sh: syntax error near unexpected token '|'");
                return 1;
            }

            fd_in = p[0];
            if (pipe(p) < 0)
                exit(1);

            fd_out = p[1];
        }

        exec_single(sub_commands[i], fd_in, fd_out);

        if (fd_in != 0)
            close(fd_in);

        if (fd_out != 1)
            close(fd_out);
    }

    return 0;
}

int exec_line(char *input) {
    // Remove newline character from input
    input[strcspn(input, "\n")] = 0;

    // Split input into multiple commands by semicolon
    char **commands = tokenized(input, ";");

    for (int i = 0; commands[i] != NULL; i++)
        exec_command(commands[i]);

    return 0;
}

int main(int argc, char **argv) {
    char command[MAX_LENGTH];

    user = getenv("USER");
    if (user == NULL) user = "(unknown)";

    dir = getcwd(NULL, 0);
    if (dir == NULL) dir = "(unknown)";

    while(1) {
        print_prompt();

        fgets(command, MAX_LENGTH, stdin);

        if (strcmp(command, "\n") == 0 || command == NULL)
            continue;

        exec_line(command);
    }

    return 0;
}
