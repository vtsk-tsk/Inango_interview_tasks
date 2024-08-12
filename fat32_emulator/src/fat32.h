#ifndef FAT32_H
#define FAT32_H

#define SECTOR_SIZE 512
#define CLUSTER_SIZE 4096
#define FAT_SIZE 1024
#define ROOT_DIR_CLUSTER 2

extern char current_path[256];

typedef struct {
    char filename[11];
    unsigned char attr;
    unsigned char reserved;
    unsigned char createTime[3];
    unsigned char createDate[2];
    unsigned short startCluster;
    unsigned int fileSize;
} DirectoryEntry;

int init_disk(const char *disk_file);
int format_disk();
int mkdir(const char *name);
int touch(const char *name);
int ls(const char *path);
int cd(const char *path);

#endif