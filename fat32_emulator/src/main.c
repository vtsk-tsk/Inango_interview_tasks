#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fat32.h"
#include "cli.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <disk_file>\n", argv[0]);
        return 1;
    }

    const char *disk_file = argv[1];
    if (init_disk(disk_file) != 0) {
        printf("Failed to initialize disk\n");
        return 1;
    }

    cli_loop();
    
    return 0;
}
