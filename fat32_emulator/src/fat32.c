#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fat32.h"

// Global variables to manage disk state
FILE *disk_fp = NULL;
char current_path[256] = "/";

// Initialize or create disk file
int init_disk(const char *disk_file) {
    disk_fp = fopen(disk_file, "r+b");
    if (!disk_fp) {
        // Create a new file if it doesn't exist
        disk_fp = fopen(disk_file, "w+b");
        if (!disk_fp) {
            return -1;
        }
        // Initialize the file with zeros
        ftruncate(fileno(disk_fp), 20 * 1024 * 1024);
    }
    return 0;
}

// Format the disk
int format_disk() {
    // Zero out the file and setup FAT32 structures
    fseek(disk_fp, 0, SEEK_SET);
    // Write boot sector, FAT, and root directory here
    return 0;
}

// List directory contents
int ls(const char *path) {
    // List files in the current or specified directory
    return 0;
}

// Change directory
int cd(const char *path) {
    // Change the current directory
    return 0;
}

// Create directory
int mkdir(const char *name) {
    // Create a new directory entry
    return 0;
}

// Create an empty file
int touch(const char *name) {
    // Create a new file entry
    return 0;
}
