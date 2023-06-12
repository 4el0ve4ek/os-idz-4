#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

int main(int argc, char *argv[]) {
    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[1024];

    clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(argv[2]));
    if (inet_pton(AF_INET, argv[1], &serverAddr.sin_addr) <= 0) {
        perror("Invalid address/Address not supported");
        exit(EXIT_FAILURE);
    }

//    if (connect(clientSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == -1) {
//        perror("Connection failed");
//        exit(EXIT_FAILURE);
//    }

    printf("Start working.\n");

    srand(time(NULL));
    int client_number = strtol(argv[3], NULL, 10);

    while (1) {
        for (int i = 0; i < 40; ++i) {
            memset(buffer, 0, sizeof(buffer));

            sprintf(buffer, "%d", i);
            ssize_t bytesSent = sendto(clientSocket, buffer, strlen(buffer), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
            if (bytesSent <= 0) {
                break;
            }

            memset(buffer, 0, sizeof (buffer));
            ssize_t bytesRead = recvfrom(clientSocket, buffer, sizeof(buffer), 0, NULL, NULL);
            if (bytesRead <= 0) {
                break;
            }

            long server_response = strtol(buffer, NULL, 10);
            if (server_response == 0) {
                printf("%d watered the %d flower\n", client_number, i);
            } else if (server_response == -1) {
                printf("error occured while %d tried to water the %d flower\n", client_number, i);
            }

        }
    }
    close(clientSocket);

    return 0;
}