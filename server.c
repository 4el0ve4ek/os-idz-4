#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

pthread_mutex_t flower_mutex;

#define PERIOD_OF_DRY 80000
#define FLOWERS_AMOUNT 40
int flowers[FLOWERS_AMOUNT];


int process_flower(int flower_number){
    if (flower_number >= FLOWERS_AMOUNT || flower_number < 0) {
        return -1;  /* wrong input */
    }
    pthread_mutex_lock(&flower_mutex);
    int ret = flowers[flower_number];
    if (ret == 0) {
        flowers[flower_number] = 1; /* caller water flower */
    }
    pthread_mutex_unlock(&flower_mutex);
    return ret;
}

void dry_some_flower() {
    if (rand() % PERIOD_OF_DRY != 0) {
        return;
    }

    int flower_number = (rand() % FLOWERS_AMOUNT + FLOWERS_AMOUNT) % FLOWERS_AMOUNT;

    pthread_mutex_lock(&flower_mutex);
    flowers[flower_number] = 0;
    pthread_mutex_unlock(&flower_mutex);

    printf("%d flower now need water\n", flower_number);
}

void init_flowers() {
    for (int i = 0; i < 40; ++i) {
        flowers[i] = 1;
    }

    printf("All flowers are initially watered\n");
}

void mutex_init() {
    if (pthread_mutex_init(&flower_mutex, NULL) != 0) {
        perror("Mutex initialization failed");
        exit(EXIT_FAILURE);
    }
}

int main(int arc, char* argv[]) {
    srand(time(NULL));

    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(struct sockaddr_in);
//    long port = strtol(argv[1], NULL, 10);


    int serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(argv[1]);
    serverAddr.sin_port = htons(atoi(argv[2]));

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, addrLen) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", atoi(argv[2]));

    mutex_init();

    init_flowers();

    int clientAddrLen = sizeof(clientAddr);
    // start server work
    while (1) {
        char buffer[1024];

        // receive request for client
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytesRead = recvfrom(serverSocket, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&clientAddr, (socklen_t *)&clientAddrLen);
        if (bytesRead <= 0) { /* error occurred */
            continue;
        }
        buffer[bytesRead] = '\0';

        int flower_number = strtol(buffer, NULL, 10);
        int ret = process_flower(flower_number);
        dry_some_flower();

        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer, "%d", ret);
        sendto(serverSocket, buffer, strlen(buffer), 0, (struct sockaddr *)&clientAddr, clientAddrLen);
    }

    pthread_mutex_destroy(&flower_mutex);
    close(serverSocket);

    return 0;
}