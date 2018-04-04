#include <string>
#include <iostream>
#include <zconf.h>
#include <wait.h>
#include <cstring>
#include <vector>
#include <cstdio>
#include <cstdlib>

void execute(char **argv) {
    pid_t pid;
    int status;
    pid = fork();
    if (pid == 0) {
        if (execvp(argv[0], argv) < 0) {
            perror("ERROR. Execute command failed");
            exit(-1);
        }
    } else if (pid > 0) {
        if (waitpid(pid, &status, 0) == -1) {
            perror("ERROR. Caught error in execution");
        };
    } else {
        perror("ERROR. Fork failed");
    }
}

std::vector<char *> parse_command(std::string &command) {
    std::vector<char *> new_argv;
    char *char_command = strdup(command.c_str());
    new_argv.push_back(strtok(char_command, " "));
    char *parameter;
    while ((parameter = strtok(NULL, " \t\r\n")) != NULL) {
        new_argv.push_back(parameter);
    }
    new_argv.push_back('\0');
    return new_argv;
};

int main(int argc, char **argv) {
    std::string command;
    printf("Shell$ ");
    while (std::getline(std::cin, command)) {
        if (command == "exit")
            break;
        std::vector<char *> new_command = parse_command(command);
        execute(new_command.data());
        free(new_command[0]);
        printf("Shell$ ");
    }
    return 0;
}