#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include<stdbool.h>
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
    int clientSocket = 0, valread;
    struct sockaddr_in serverAddr;
    socklen_t serverAddrLen = sizeof(serverAddr);
    char buffer[BUFFER_SIZE] = {0};
    char input[BUFFER_SIZE] = {0};

    // Creating socket
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror(RED"Socket creation failed"RESET);
        exit(EXIT_FAILURE);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "172.20.10.6", &serverAddr.sin_addr) <= 0) {
        perror(RED"\nInvalid address/ Address not supported \n"RESET);
        exit(EXIT_FAILURE);
    }

    // Connect to the server

    if (connect(clientSocket, (struct sockaddr *)&serverAddr, (socklen_t)serverAddrLen) < 0) {
        perror(RED"Connection failed"RESET);
        exit(EXIT_FAILURE);
    }
    // connected to the server successfully
    recv(clientSocket, buffer, BUFFER_SIZE, 0);
    printf("%s", buffer);
    int playerId;
    if(strstr(buffer, "Player 1")){
        playerId = 1;
    }
    if(strstr(buffer, "Player 2")){
        playerId = 2;
    }
    playerId = playerId+0;
    printf("!\n");
    printf("Starting the game\n");
    // Game loop
    while (true)
    {
        printf(CYAN);
        printf("Board:\n");
        printf(" 1 | 2 | 3 \n");
        printf("---|---|---\n");
        printf(" 4 | 5 | 6 \n");
        printf("---|---|---\n");
        printf(" 7 | 8 | 9 \n");

        while (true) {
            memset(buffer, 0, BUFFER_SIZE);
            valread = recv(clientSocket, buffer, BUFFER_SIZE, 0);

            if (valread <= 0) {
                // perror(RED"Failed to read from server"RESET);
                fprintf(stderr, RED"[Connection lost from the server]\n"RESET);
                exit(EXIT_FAILURE);
                break;
            }
            
            // printf("Value received: %s\n", buffer);
            if(strstr(buffer,"Invalid")){ //  "Invalid Move"
                printf(RED"%s\n"RESET,buffer);
                
            }
            if(strstr(buffer,"Waiting") ){
                printf(YELLOW"%s\n"RESET,buffer); // "Waiting for Player 1 to make a move ..."
                
            }
            if(strstr(buffer, "Updating")){
                printf(GREEN"Updating the board ...\n"RESET);
                
            }
            if (strstr(buffer, "Board\n")) {
                printf(CYAN"Board:\n");
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
                // printf(RESET"\n");
                printf(RESET);
                
            }        
            // Check if the game is over
            if (strstr(buffer, "wins")) {
                // printf(GREEN"%s\n"RESET, buffer); // "Player 1 wins" 
                printf(GREEN"[You Win]\n"RESET); // "Player 1 wins"
                
                break;
            }
            else if (strstr(buffer, "Draw")) {
                printf(YELLOW"[Match Drawn]\n"RESET); // "Draw"
                
                break;
            }
            else if( strstr(buffer, "loses")){
                // printf(RED"%s\n"RESET, buffer); // "Player 2 loses" // length = 14
                printf(RED"[You Lose]\n"RESET); // "Player 2 loses"
                
                break;
            }
            // Get user input for the move
            if (strstr(buffer, "Your Turn Player")) { // "Your Turn Player 1"
                printf(YELLOW"Enter move [row col]: ");
                fgets(input, BUFFER_SIZE, stdin);
                printf(RESET);
                
                send(clientSocket, input, strlen(input), 0);
                
            }
        }

        // After the game ends, ask if they want to play again
        printf(YELLOW"Do you want to play again? [yes/no]: "RESET);
        fgets(input, BUFFER_SIZE, stdin);
        send(clientSocket, input, strlen(input), 0);


        memset(buffer, 0, BUFFER_SIZE);
        
        recv(clientSocket, buffer, BUFFER_SIZE, 0);
        // printf("Response: %s\n", buffer);
        if(strstr(buffer, "1 disconnected")){
            if(playerId == 2) printf(RED"[Player 1 disconnected]\n"RESET);
            printf(RED"Game Over\n"RESET);
            break;
        }
        else if(strstr(buffer,"2 disconnected")){
            if(playerId == 1) printf(RED"[Player 2 disconnected]\n"RESET);
            printf(RED"Game Over\n"RESET);
            break;
        }
        else if(strstr(buffer,"no")){
            printf(RED"[Game Over]\n"RESET);
            break;
        }
        else if(strstr(buffer,"yes")){
            printf(GREEN"[Restarting the game]\n"RESET);
        }
    }
    memset(buffer, 0, BUFFER_SIZE);
    strcpy(buffer, "exit");
    send(clientSocket, buffer, strlen(buffer), 0);
    close(clientSocket);
    return 0;

}




        
