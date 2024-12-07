#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <ctype.h>

#define PORT 8080
#define MAX 100
#define BUFFER_SIZE 1024

#define Symbol_X 'X'
#define Symbol_O 'O'

#define winner1 1
#define draw 0
#define winner2 2
#define NOT_FINISHED -1
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define RESET "\033[0m"
#define YELLOW "\033[1;33m"
#define CYAN "\033[1;36m"

bool response1 = true;
bool response2 = true;
char board[3][3];
int player = 1;

// initialize the board
void initBoard() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            board[i][j] = ' ';
        }
    }
}

// print the board
void printBoard() {
    printf("\n");
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf(" %c ", board[i][j]);
            if (j < 2) printf("|");
        }
        if (i < 2) printf("\n---|---|---\n");
    }
    printf("\n");
}

// update the board after a move
int updateBoard(int row, int coloumn, int player) {
    if (row < 0 || row > 2 || coloumn < 0 || coloumn > 2) {
        return 0;
    }
    if (board[row][coloumn] == ' ') {
        if (player == 1) {
            board[row][coloumn] = Symbol_X;
        } else {
            board[row][coloumn] = Symbol_O;
        }
        return 1; // valid move
    }
    return 0; // invalid move
}

// check for winner
int checkWinner() {
    int cntBlank = 0;
    // check diagonals
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != ' ') {
        return board[0][0] == Symbol_X ? winner1 : winner2;
    }
    if (board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[0][2] != ' ') {
        return board[0][2] == Symbol_X ? winner1 : winner2;
    }
    // check rows and columns
    for (int i = 0; i < 3; i++) {
        if (board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][0] != ' ') {
            return board[i][0] == Symbol_X ? winner1 : winner2;
        }
        if (board[0][i] == board[1][i] && board[1][i] == board[2][i] && board[0][i] != ' ') {
            return board[0][i] == Symbol_X ? winner1 : winner2;
        }
    }
    // check for draw
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == ' ') cntBlank++;
        }
    }
    if (cntBlank == 0) return draw;
    return NOT_FINISHED;
}

// send board to client
void sendBoard(int socket, struct sockaddr_in clientAddr, socklen_t addrLen) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer, "Board\n");
    int offset = strlen(buffer);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            buffer[offset++] = board[i][j];
            buffer[offset++] = ' ';
        }
        buffer[offset - 1] = '\n';
    }
    sendto(socket, buffer, offset, 0, (struct sockaddr*)&clientAddr, addrLen);
}

// trim whitespace from input
void trimWhitespace(char *str) {
    if (str == NULL) {
        return;
    }

    char *start = str;
    while (isspace((unsigned char)*start)) start++;

    char *end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) end--;

    *(end + 1) = '\0';
    memmove(str, start, strlen(start) + 1);
}

// extract row and column from client's input
void extractRowAndCol(char *buffer, int *row, int *coloumn) {
    trimWhitespace(buffer);
    char *token;
    char *saveptr;
    token = strtok_r(buffer, " ", &saveptr);
    if (token == NULL) {
        *row = -1; *coloumn = -1;
        return;
    }
    trimWhitespace(token);
    if (!isdigit(token[0])) {
        *row = -1; *coloumn = -1;
        return;
    }
    *row = atoi(token);
    token = strtok_r(NULL, ",", &saveptr);
    if (token == NULL) {
        *coloumn = -1; return;
    }
    trimWhitespace(token);
    if (!isdigit(token[0])) {
        *coloumn = -1; return;
    }
    *coloumn = atoi(token);
    *row -= 1;
    *coloumn -= 1;
}

// handle the game between two players
void handleGame(int socket, struct sockaddr_in player1Addr, struct sockaddr_in player2Addr) {
    int row, coloumn;
    char getRowAndCol[BUFFER_SIZE];
    socklen_t addrLen1 = sizeof(player1Addr);
    socklen_t addrLen2 = sizeof(player2Addr);
    player = 1;

    while (true) {
        printBoard();
        if (player == 1) { // Player 1's turn
            printf("Player 1's turn\n");
            char buffer1[BUFFER_SIZE] = "Your Turn Player 1";
            char buffer2[BUFFER_SIZE] = "Waiting for Player 1 to make a move ...";
            sendto(socket, buffer1, sizeof(buffer1), 0, (struct sockaddr*)&player1Addr, addrLen1);
            sendto(socket, buffer2, sizeof(buffer2), 0, (struct sockaddr*)&player2Addr, addrLen2);
            recvfrom(socket, getRowAndCol, sizeof(getRowAndCol), 0, (struct sockaddr*)&player1Addr, &addrLen1);
            printf("Received: %s\n", getRowAndCol);
            extractRowAndCol(getRowAndCol, &row, &coloumn);
            if (updateBoard(row, coloumn, player) == 0) {
                char invalidMoveMsg[BUFFER_SIZE] = "Invalid Move";
                sendto(socket, invalidMoveMsg, sizeof(invalidMoveMsg), 0, (struct sockaddr*)&player1Addr, addrLen1);
                continue;
            }
            char updateMsg[BUFFER_SIZE] = "Updating the board\n";
            sendto(socket, updateMsg, sizeof(updateMsg), 0, (struct sockaddr*)&player1Addr, addrLen1);
            sendto(socket, updateMsg, sizeof(updateMsg), 0, (struct sockaddr*)&player2Addr, addrLen2);
            sleep(1);
            sendBoard(socket, player1Addr, addrLen1);
            sendBoard(socket, player2Addr, addrLen2);
            int result = checkWinner();
            if (result == winner1) {
                sleep(1);
                char winMsg[BUFFER_SIZE] = "Player 1 wins";
                char loseMsg[BUFFER_SIZE] = "Player 2 loses";
                sendto(socket, winMsg, sizeof(winMsg), 0, (struct sockaddr*)&player1Addr, addrLen1);
                sendto(socket, loseMsg, sizeof(loseMsg), 0, (struct sockaddr*)&player2Addr, addrLen2);
                break;
            } else if (result == draw) {
                sleep(1);
                char drawMsg[BUFFER_SIZE] = "Draw";
                sendto(socket, drawMsg, sizeof(drawMsg), 0, (struct sockaddr*)&player1Addr, addrLen1);
                sendto(socket, drawMsg, sizeof(drawMsg), 0, (struct sockaddr*)&player2Addr, addrLen2);
                break;
            }
            player = 2; // Switch to Player 2
        } else { // Player 2's turn
            printf("Player 2's turn\n");
            char buffer1[BUFFER_SIZE] = "Your Turn Player 2";
            char buffer2[BUFFER_SIZE] = "Waiting for Player 2 to make a move ...";
            sendto(socket, buffer1, sizeof(buffer1), 0, (struct sockaddr*)&player2Addr, addrLen2);
            sendto(socket, buffer2, sizeof(buffer2), 0, (struct sockaddr*)&player1Addr, addrLen1);
            recvfrom(socket, getRowAndCol, sizeof(getRowAndCol), 0, (struct sockaddr*)&player2Addr, &addrLen2);
            printf("Received: %s\n", getRowAndCol);
            extractRowAndCol(getRowAndCol, &row, &coloumn);
            if (updateBoard(row, coloumn, player) == 0) {
                char invalidMoveMsg[BUFFER_SIZE] = "Invalid Move";
                sendto(socket, invalidMoveMsg, sizeof(invalidMoveMsg), 0, (struct sockaddr*)&player2Addr, addrLen2);
                continue;
            }
            char updateMsg[BUFFER_SIZE] = "Updating the board\n";
            sendto(socket, updateMsg, sizeof(updateMsg), 0, (struct sockaddr*)&player1Addr, addrLen1);
            sendto(socket, updateMsg, sizeof(updateMsg), 0, (struct sockaddr*)&player2Addr, addrLen2);
            sleep(1);
            sendBoard(socket, player1Addr, addrLen1);
            sendBoard(socket, player2Addr, addrLen2);
            int result = checkWinner();
            if (result == winner2) {
                sleep(1);
                char winMsg[BUFFER_SIZE] = "Player 2 wins";
                char loseMsg[BUFFER_SIZE] = "Player 1 loses";
                sendto(socket, winMsg, sizeof(winMsg), 0, (struct sockaddr*)&player2Addr, addrLen2);
                sendto(socket, loseMsg, sizeof(loseMsg), 0, (struct sockaddr*)&player1Addr, addrLen1);
                break;
            } else if (result == draw) {
                sleep(1);
                char drawMsg[BUFFER_SIZE] = "Draw";
                sendto(socket, drawMsg, sizeof(drawMsg), 0, (struct sockaddr*)&player1Addr, addrLen1);
                sendto(socket, drawMsg, sizeof(drawMsg), 0, (struct sockaddr*)&player2Addr, addrLen2);
                break;
            }
            player = 1; // Switch back to Player 1
        }
    }
}

// main function
int main() {
    int socket_desc;
    struct sockaddr_in serverAddr, player1Addr, player2Addr;
    socklen_t addrLen1 = sizeof(player1Addr);
    socklen_t addrLen2 = sizeof(player2Addr);

    // Create UDP socket
    socket_desc = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_desc < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Bind the socket to the specified port
    if (bind(socket_desc, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        close(socket_desc);
        exit(EXIT_FAILURE);
    }

    printf("Server is waiting for players to join...\n");

    // Accept players (2 players)
    recvfrom(socket_desc, NULL, 0, 0, (struct sockaddr*)&player1Addr, &addrLen1);
    printf("Player 1 has joined.\n");
    sendto(socket_desc, "Welcome Player 1! Waiting for Player 2...\n", 45, 0, (struct sockaddr*)&player1Addr, addrLen1);

    recvfrom(socket_desc, NULL, 0, 0, (struct sockaddr*)&player2Addr, &addrLen2);
    printf("Player 2 has joined.\n");
    sendto(socket_desc, "Welcome Player 2! Game will start now...\n", 43, 0, (struct sockaddr*)&player2Addr, addrLen2);

    // initBoard();
    // handleGame(socket_desc, player1Addr, player2Addr);
    while (true) {
        initBoard();  // Initialize the game board
        handleGame(socket_desc, player1Addr, player2Addr);  // Handle the game logic

        // Check if player 1 wants to play again
        char playAgain[BUFFER_SIZE];
        memset(playAgain, 0, sizeof(playAgain)); // Clear the buffer
        recvfrom(socket_desc, playAgain, sizeof(playAgain), 0, (struct sockaddr*)&player1Addr, &addrLen1);
        printf("Received1: %s\n", playAgain);
        if (strstr(playAgain, "no")) {
            printf("Player 1 wants to exit\n");
            response1 = false;
        }

        // Check if player 2 wants to play again
        memset(playAgain, 0, sizeof(playAgain)); // Clear the buffer
        recvfrom(socket_desc, playAgain, sizeof(playAgain), 0, (struct sockaddr*)&player2Addr, &addrLen2);
        printf("Received2: %s\n", playAgain);
        if (strstr(playAgain, "no")) {
            printf("Player 2 wants to exit\n");
            response2 = false;
        }

        // Check the responses from both players
        if (response1 && response2) {
            // Notify both players that the game will restart
            sendto(socket_desc, "yes", sizeof("yes"), 0, (struct sockaddr*)&player1Addr, addrLen1);
            sendto(socket_desc, "yes", sizeof("yes"), 0, (struct sockaddr*)&player2Addr, addrLen2);
            printf(GREEN "Restarting the game\n" RESET);
        } else {
            printf(RED "Game Over\n" RESET);
            break; // Exit the loop if either player does not want to play again
        }
    }

    // Notify players about disconnection if one of them doesn't want to play again
    if (!response1) {
        sendto(socket_desc, "1 disconnected", sizeof("1 disconnected"), 0, (struct sockaddr*)&player2Addr, addrLen2);
        sendto(socket_desc, "1 disconnected", sizeof("1 disconnected"), 0, (struct sockaddr*)&player1Addr, addrLen1);
    } else if (!response2) {
        sendto(socket_desc, "2 disconnected", sizeof("2 disconnected"), 0, (struct sockaddr*)&player1Addr, addrLen1);
        sendto(socket_desc, "2 disconnected", sizeof("2 disconnected"), 0, (struct sockaddr*)&player2Addr, addrLen2);
    } else {
        // If both players want to exit, send exit signals
        sendto(socket_desc, "no", sizeof("no"), 0, (struct sockaddr*)&player1Addr, addrLen1);
        sendto(socket_desc, "no", sizeof("no"), 0, (struct sockaddr*)&player2Addr, addrLen2);
    }
    // recvfrom(socket_desc, NULL, 0, 0, (struct sockaddr*)&player1Addr, &addrLen1); // exit signal from player 1
    // recvfrom(socket_desc, NULL, 0, 0, (struct sockaddr*)&player2Addr, &addrLen2); // exit signal from player 2
    printf("Closing the server\n"); 
    sleep(10);
    close(socket_desc);
    return 0;
}
