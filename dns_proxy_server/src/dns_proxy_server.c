#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define BUFFER_SIZE 512
#define MAX_BLACKLIST 100
#define CONFIG_FILE "dns_proxy_server.conf"
#define PORT_LISTEN 1053

// Configuration structure
typedef struct {
    char upstream_dns_ip[16];
    char blacklist[MAX_BLACKLIST][256];
    int blacklist_count;
    int response_type;
    char redirect_ip[16];
} config_t;

config_t config;

// Function prototypes
void load_config(const char *filename);
int is_blacklisted(const char *domain);
void handle_request(int sockfd, struct sockaddr_in *client_addr, socklen_t client_len, unsigned char *buffer, int nbytes);
void send_response(int sockfd, struct sockaddr_in *client_addr, socklen_t client_len, unsigned char *buffer, int response_type, const char *redirect_ip);

// Load configuration from file
void load_config(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open configuration file");
        exit(EXIT_FAILURE);
    }

    char line[256];
    config.blacklist_count = 0;

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "upstream_dns_ip", 15) == 0) {
            sscanf(line, "upstream_dns_ip = %15s", config.upstream_dns_ip);
        } else if (strncmp(line, "blacklist", 9) == 0) {
            if (config.blacklist_count < MAX_BLACKLIST) {
                sscanf(line, "blacklist = %255s", config.blacklist[config.blacklist_count++]);
            }
        } else if (strncmp(line, "response_type", 13) == 0) {
            sscanf(line, "response_type = %d", &config.response_type);
        } else if (strncmp(line, "redirect_ip", 11) == 0) {
            sscanf(line, "redirect_ip = %15s", config.redirect_ip);
        }
    }

    fclose(file);
}

// Check if the domain is in the blacklist
int is_blacklisted(const char *domain) {
    for (int i = 0; i < config.blacklist_count; i++) {
        if (strcasecmp(domain, config.blacklist[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Handle DNS request
void handle_request(int sockfd, struct sockaddr_in *client_addr, socklen_t client_len, unsigned char *buffer, int nbytes) {
    char domain[256];
    int i = 12; // Skip the header

    // Extract domain name from the DNS query
    int pos = 0;
    while (i < nbytes && buffer[i] != 0) {
        int len = buffer[i++];
        if (pos > 0) {
            domain[pos++] = '.';
        }
        for (int j = 0; j < len; j++) {
            domain[pos++] = buffer[i++];
        }
    }
    domain[pos] = '\0';

    // Check if the domain is blacklisted
    if (is_blacklisted(domain)) {
        send_response(sockfd, client_addr, client_len, buffer, config.response_type, config.redirect_ip);
    } else {
        // Forward the request to the upstream DNS server
        struct sockaddr_in upstream_addr;
        int upstream_sock = socket(AF_INET, SOCK_DGRAM, 0);

        upstream_addr.sin_family = AF_INET;
        upstream_addr.sin_port = htons(53);
        inet_pton(AF_INET, config.upstream_dns_ip, &upstream_addr.sin_addr);

        sendto(upstream_sock, buffer, nbytes, 0, (struct sockaddr *)&upstream_addr, sizeof(upstream_addr));

        int len = recvfrom(upstream_sock, buffer, BUFFER_SIZE, 0, NULL, NULL);
        sendto(sockfd, buffer, len, 0, (struct sockaddr *)client_addr, client_len);

        close(upstream_sock);
    }
}

// Send DNS response
void send_response(int sockfd, struct sockaddr_in *client_addr, socklen_t client_len, unsigned char *buffer, int response_type, const char *redirect_ip) {
    buffer[2] = 0x81; // Set response flags
    buffer[3] = 0x80;

    // Clear the answer count, authority count, and additional count
    memset(buffer + 6, 0, 6);

    // Set the answer count based on the response type
    if (response_type == 3) {
        buffer[7] = 1; // One answer
    }

    int i = 12;
    while (buffer[i] != 0) {
        i += buffer[i] + 1;
    }
    i += 5; // Skip QTYPE and QCLASS

    // Construct the response based on the type
    if (response_type == 1) { // NOT FOUND
        // Do nothing, the answer count is already set to 0
    } else if (response_type == 2) { // REFUSED
        buffer[3] = 0x85; // Set REFUSED in the flags
    } else if (response_type == 3) { // Resolve to a pre-configured IP
        buffer[i++] = 0xc0;
        buffer[i++] = 0x0c;
        buffer[i++] = 0x00;
        buffer[i++] = 0x01;
        buffer[i++] = 0x00;
        buffer[i++] = 0x01;
        buffer[i++] = 0x00;
        buffer[i++] = 0x00;
        buffer[i++] = 0x00;
        buffer[i++] = 0x3c;
        buffer[i++] = 0x00;
        buffer[i++] = 0x04;

        struct in_addr ip;
        inet_pton(AF_INET, redirect_ip, &ip);
        memcpy(buffer + i, &ip, sizeof(ip));
        i += sizeof(ip);
    }

    sendto(sockfd, buffer, i, 0, (struct sockaddr *)client_addr, client_len);
}

// Main function
int main() {
    load_config(CONFIG_FILE);

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    unsigned char buffer[BUFFER_SIZE];

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT_LISTEN);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    printf("DNS Proxy Server is running...\n");

    while (1) {
        int nbytes = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client_len);
        handle_request(sockfd, &client_addr, client_len, buffer, nbytes);
    }

    close(sockfd);
    return 0;
}
