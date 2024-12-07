#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define PORT 8080
#define BUFFER_SIZE 1024
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define RESET "\033[0m"
#define YELLOW "\033[1;33m"
#define CYAN "\033[1;36m"

#define winner1 1
#define draw 0
#define winner2 2
#define NOT_FINISHED -1

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE] = {0};
    char input[BUFFER_SIZE] = {0};

    // Creating socket
    if ((clientSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror(RED "Socket creation failed" RESET);
        exit(EXIT_FAILURE);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("172.20.10.6");
    serverAddr.sin_port = htons(PORT);

    // Send an initial message to the server to join the game
    const char *joinMsg = "Join Game";
    sendto(clientSocket, joinMsg, strlen(joinMsg), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    // Receive welcome message
    socklen_t addrLen = sizeof(serverAddr);
    recvfrom(clientSocket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&serverAddr, &addrLen);
    printf("%s", buffer);
    
    int playerId = (strstr(buffer, "Player 1")) ? 1 : 2;
    printf("Starting the game\n");

    // Game loop
    while (true) {
        printf(CYAN);
        printf("Board:\n");
        printf(" 1 | 2 | 3 \n");
        printf("---|---|---\n");
        printf(" 4 | 5 | 6 \n");
        printf("---|---|---\n");
        printf(" 7 | 8 | 9 \n");

        while (true) {
            memset(buffer, 0, BUFFER_SIZE);
            ssize_t valread = recvfrom(clientSocket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&serverAddr, &addrLen);

            if (valread <= 0) {
                fprintf(stderr, RED "[Connection lost from the server]\n" RESET);
                exit(EXIT_FAILURE);
            }

            if (strstr(buffer, "Invalid")) { 
                printf(RED "%s\n" RESET, buffer);
            }
            if (strstr(buffer, "Waiting")) {
                printf(YELLOW "%s\n" RESET, buffer);
            }
            if (strstr(buffer, "Updating")) {
                printf(GREEN "Updating the board ...\n" RESET);
            }
            if (strstr(buffer, "Board\n")) {
                printf(CYAN "Board:\n");
                int offset = 6;
                for (int i = 0; i < 3; i++) {
                    printf(" ");
                    for (int j = 0; j < 3; j++) {
                        printf(" %c ", buffer[offset]);
                        offset += 2;
                        if (j < 2) {
                            printf("|");
                        }
                    }
                    printf("\n");
                    if (i < 2) {
                        printf(" ---|---|---\n");
                    }
                }
                printf(RESET);
            }        
            // Check if the game is over
            if (strstr(buffer, "wins")) {
                printf(GREEN "[You Win]\n" RESET);
                break;
            } else if (strstr(buffer, "Draw")) {
                printf(YELLOW "[Match Drawn]\n" RESET);
                break;
            } else if (strstr(buffer, "loses")) {
                printf(RED "[You Lose]\n" RESET);
                break;
            }
            // Get user input for the move
            if (strstr(buffer, "Your Turn Player")) {
                printf(YELLOW "Enter move [row col]: ");
                fgets(input, BUFFER_SIZE, stdin);
                printf(RESET);
                sendto(clientSocket, input, strlen(input), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
            }
        }

        // After the game ends, ask if they want to play again
        printf(YELLOW "Do you want to play again? [yes/no]: " RESET);
        fgets(input, BUFFER_SIZE, stdin);
        sendto(clientSocket, input, strlen(input), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

        memset(buffer, 0, BUFFER_SIZE);
        recvfrom(clientSocket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&serverAddr, &addrLen);
        
        if (strstr(buffer, "1 disconnected")) {
            if (playerId == 2) printf(RED "[Player 1 disconnected]\n" RESET);
            printf(RED "Game Over\n" RESET);
            break;
        } else if (strstr(buffer, "2 disconnected")) {
            if (playerId == 1) printf(RED "[Player 2 disconnected]\n" RESET);
            printf(RED "Game Over\n" RESET);
            break;
        } else if (strstr(buffer, "no")) {
            printf(RED "[Game Over]\n" RESET);
            break;
        } else if (strstr(buffer, "yes")) {
            printf(GREEN "[Restarting the game]\n" RESET);
        }
    }
    
    // memset(buffer, 0, BUFFER_SIZE);
    // strcpy(buffer, "exit");
    // sendto(clientSocket, buffer, strlen(buffer), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    close(clientSocket);
    return 0;
}
