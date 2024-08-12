#include <stdio.h>
#include <string.h>
#include "fat32.h"

void cli_loop() {
    char command[256];
    while (1) {
        printf("%s> ", current_path);
        fgets(command, 256, stdin);
        command[strcspn(command, "\n")] = 0;

        if (strncmp(command, "cd ", 3) == 0) {
            cd(command + 3);
        } else if (strcmp(command, "ls") == 0) {
            ls(NULL);
        } else if (strncmp(command, "ls ", 3) == 0) {
            ls(command + 3);
        } else if (strncmp(command, "mkdir ", 6) == 0) {
            mkdir(command + 6);
        } else if (strncmp(command, "touch ", 6) == 0) {
            touch(command + 6);
        } else if (strcmp(command, "format") == 0) {
            format_disk();
        } else if (strcmp(command, "exit") == 0) {
            break;
        } else {
            printf("Unknown command\n");
        }
    }
}
