#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<stdbool.h>
#include<pthread.h>
#include<ctype.h>

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
int player1, player2;

// initialize the board
void initBoard(){
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            board[i][j] = ' ';
        }
    }
}
// print the board
void printBoard(){
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
// 
int updateBoard(int row,int coloumn,int player){
    if(row < 0 || row > 2 || coloumn < 0 || coloumn > 2){
        return 0;
    }
    if(board[row][coloumn] == ' '){
        if(player == 1){
            board[row][coloumn] = Symbol_X;
            
        }else{
            board[row][coloumn] = Symbol_O;
        }
        return 1; // return 1 if the move is valid
    }
    else return 0; // return 0 if the move is invalid
}
// check for winner
int checkWinner(){
    int cntBlank = 0;
    // check diagonals
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != ' '){
        if (board[0][0] == Symbol_X) return winner1;
        else return winner2;
    }
    if(board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[0][2] != ' '){
        if(board[0][2] == Symbol_X) return winner1;
        else return winner2;
    }
    // check rows and columns
    for (int i = 0; i < 3; i++){
        if (board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][0] != ' '){
            if(board[i][0] == Symbol_X) return winner1;
            else return winner2;

        }
        if (board[0][i] == board[1][i] && board[1][i] == board[2][i] && board[0][i] != ' '){
            if(board[0][i] == Symbol_X) return winner1;
            else return winner2;
        }
    }
    // check for draw
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            if (board[i][j] == ' ') cntBlank++;
        }
    }
    if(cntBlank == 0) return draw;
    return NOT_FINISHED;
}


void sendBoard(int newSocket) {
    char buffer[BUFFER_SIZE];
    
    // Clear the buffer
    memset(buffer, 0, sizeof(buffer));
    
    // Add "Board" message to the buffer
    strcpy(buffer, "Board\n");
    
    // Append board content to the buffer
    int offset = strlen(buffer);  // Start appending after "Board\n"
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            buffer[offset++] = board[i][j];
            buffer[offset++] = ' ';  // Add a space between board elements
        }
        buffer[offset - 1] = '\n';  // Replace last space in the row with a newline
    }
    // printf("Length of buffer: %ld\n", strlen(buffer));
    // Send the buffer with the "Board" message and the board content
    send(newSocket, buffer, offset, 0);
}
void trimWhitespace(char *str) {
    if(str==NULL){
        return;
    }

    char *start = str;
    while (isspace((unsigned char)*start)) start++;

    char *end = start + strlen(start) - 1;
    while(end > start && isspace((unsigned char)*end)) end--;

    *(end+1) = '\0';

    memmove(str, start, strlen(start) + 1);
}

void extractRowAndCol(char *buffer, int *row, int *coloumn){
    // use strtok_r to extract the row and coloumn
    trimWhitespace(buffer);
    char *token;
    char* saveptr;
    token = strtok_r(buffer, " ", &saveptr);
    if(token == NULL){
        *row = -1; *coloumn = -1;
        return;
    }
    trimWhitespace(token);
    // check if the token is a number
    if(!isdigit(token[0])){
        *row = -1; *coloumn = -1;
        return;
    }
    *row = atoi(token);
    token = strtok_r(NULL, ",", &saveptr);
    if (token == NULL){
        *coloumn = -1; return;
    }
    trimWhitespace(token);
    if(!isdigit(token[0])){
        *coloumn = -1; return;
    }
    *coloumn = atoi(token);
    *row -= 1;
    *coloumn -= 1;

}

// handle the game
void handleGame(void *arg){
    int Socket1 = *((int *)arg); // basically we are getting the socket1 and socket2
    int Socket2 = *((int *)arg + 1);
    int row, coloumn; // defined for taking the input from the client
    char getRowAndCol[BUFFER_SIZE];
    player = 1;
    // char startMessage[BUFFER_SIZE] = "Starting Game \n";
    // send(Socket1, startMessage, sizeof(startMessage), 0);
    // send(Socket2, startMessage, sizeof(startMessage), 0);
    while (true){
        // if player is 1 then send the board to the client 
        // Notifying the client that it is their turn
        printBoard();
        if (player == 1){ // if player is 1 then send the board to the client
            char buffer[BUFFER_SIZE] = "Your Turn Player 1";
            printf("Player 1's turn\n");
            char buffer2[BUFFER_SIZE] = "Waiting for Player 1 to make a move ...";
            send(Socket2, buffer2, sizeof(buffer2), 0);

            send(Socket1, buffer, sizeof(buffer), 0);
            // sendBoard(Socket1);
            recv(Socket1, getRowAndCol, sizeof(getRowAndCol), 0);
            printf("Received: %s\n", getRowAndCol);
            extractRowAndCol(getRowAndCol, &row, &coloumn);
            int update = updateBoard(row, coloumn, player); // update the board
            if (update == 0){ // if the move is invalid then continue the loop
                // we need to send the message to the client that the move is invalid
                char buffer[BUFFER_SIZE] = "Invalid Move";
                send(Socket1, buffer, sizeof(buffer), 0);

                continue;
            }
            char message[BUFFER_SIZE] = "Updating the board\n";
            send(Socket2, message, sizeof(message), 0);
            send(Socket1, message, sizeof(message), 0);
            sleep(1); // to 
            sendBoard(Socket1);
            sendBoard(Socket2);
            int result = checkWinner();
            if (result == winner1){
                // instead of send "Player 1 wins" we can send the result as winner1
                sleep(1);
                char buffer[BUFFER_SIZE] = "Player 1 wins";
                send(Socket1, buffer, sizeof(buffer), 0);
                char buffer2[BUFFER_SIZE] = "Player 2 loses";
                send(Socket2, buffer2, sizeof(buffer2), 0);
                break;
            }else if (result == draw){
                sleep(1);
                char buffer[BUFFER_SIZE] = "Draw";
                send(Socket1, buffer, sizeof(buffer), 0);
                char buffer2[BUFFER_SIZE] = "Draw";
                send(Socket2, buffer2, sizeof(buffer2), 0);
                break;
            }

            player = 2;
        }
        else{
            printf("Player 2's turn\n");
            char buffer[BUFFER_SIZE] = "Your Turn Player 2";
            send(Socket2, buffer, sizeof(buffer), 0);
            char buffer2[BUFFER_SIZE] = "Waiting for Player 2 to make a move ...";
            send(Socket1, buffer2, sizeof(buffer2), 0);

            // sendBoard(Socket2);
            recv(Socket2, getRowAndCol, sizeof(getRowAndCol), 0);
            extractRowAndCol(getRowAndCol, &row, &coloumn);
            int update = updateBoard(row, coloumn, player);
            if (update == 0){
                char buffer[BUFFER_SIZE] = "Invalid Move";
                send(Socket2, buffer, sizeof(buffer), 0);
                continue;
            }
            char message[BUFFER_SIZE] = "Updating the board\n";
            send(Socket1, message, sizeof(message), 0);
            send(Socket2, message, sizeof(message), 0);
            sleep(1);
            sendBoard(Socket1);
            sendBoard(Socket2);
            int result = checkWinner();
            if (result == winner2){
                sleep(1);
                char buffer[BUFFER_SIZE] = "Player 2 wins";
                send(Socket2, buffer, sizeof(buffer), 0);
                char buffer2[BUFFER_SIZE] = "Player 1 loses";
                send(Socket1, buffer2, sizeof(buffer2), 0);
                break;
            }else if (result == draw){
                sleep(1);
                char buffer[BUFFER_SIZE] = "Draw";
                send(Socket2, buffer, sizeof(buffer), 0);
                char buffer2[BUFFER_SIZE] = "Draw";
                send(Socket1, buffer2, sizeof(buffer2), 0);
                break;
            }
            player = 1;
        }
        sleep(1);
    }
}


int main(){
    int serverFD, newSocket1 , newSocket2; // server file descriptor and new socket
    struct sockaddr_in serverAddr;
    socklen_t addressLen = sizeof(serverAddr);
    char buffer[BUFFER_SIZE];

    // create the socket
    if((serverFD = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror(RED"Socket creation failed"RESET);
        exit(EXIT_FAILURE);
    }
    printf(GREEN"Server socket created\n"RESET);

    // forcefull attach the socket to the port
    int option = 1;
    if(setsockopt(serverFD, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option, sizeof(option)) < 0){
        perror(RED"Setsockopt failed"RESET);
        exit(EXIT_FAILURE);
    }
    // bind the socket to the network address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    if(bind(serverFD, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0){
        perror(RED"Bind failed"RESET);
        exit(EXIT_FAILURE);
    }
    // listen for incoming connections
    if(listen(serverFD, 3) < 0){
        perror(RED"Listen failed"RESET);
        exit(EXIT_FAILURE);
    }
    printf(YELLOW"Waiting for players to connect...\n"RESET);

    // accept the connection from the player1
    if((newSocket1 = accept(serverFD, (struct sockaddr *)&serverAddr, (socklen_t*)&addressLen)) < 0){
        perror(RED"Accept failed"RESET);
        exit(EXIT_FAILURE);
    }
    printf(GREEN"Player 1 (X) connected\n"RESET);
    send(newSocket1, GREEN"You are Player 1"RESET, strlen(GREEN"You are Player 1"RESET), 0);

    if((newSocket2 = accept(serverFD, (struct sockaddr *)&serverAddr, (socklen_t*)&addressLen)) < 0){
        perror(RED"Accept failed"RESET);
        exit(EXIT_FAILURE);
    }
    printf(GREEN"Player 2 (O) connected\n"RESET);
    send(newSocket2, GREEN"You are Player 2"RESET, strlen(GREEN"You are Player 2"RESET), 0);


    // handle the game
    int sockets[2] = {newSocket1, newSocket2};
    player1 = newSocket1;
    player2 = newSocket2;
    // bool response1 = true,response2 = true;
    while(true){
        // send the message to the client that the game is starting
        initBoard();
        // printBoard();
        handleGame(sockets);
        char input[BUFFER_SIZE];

        recv(newSocket1, input, sizeof(input), 0);
        printf("Received 1: %s\n", input);
        char response[BUFFER_SIZE] = "yes";
        if(strstr(input, "no")){
            printf("Player 1 disconnected\n");
            response1 = false;
        }
        memset(input, 0, BUFFER_SIZE);
        recv(newSocket2, input, sizeof(input), 0);
        printf("Received 2: %s\n", input);
        if(strstr(input, "no")){
            printf("Player 2 disconnected\n");
            response2 = false;
        }
        if(response1 && response2){
            send(newSocket1, response, sizeof(response), 0);
            send(newSocket2, response, sizeof(response), 0);
        }
        else{
            printf("Game Over\n");
            break;
        }
    }
    
    if(!response1 && !response2){
        printf("No player connected\n");
        char response[BUFFER_SIZE] = "no";
        send(newSocket1, response, sizeof(response), 0);
        send(newSocket2, response, sizeof(response), 0);
    }
    else if(!response1){
        char responseM1[BUFFER_SIZE] = "Player 1 disconnected";
        char response[BUFFER_SIZE] = "no";
        send(newSocket1, response, sizeof(response), 0);
        send(newSocket2, responseM1, sizeof(responseM1), 0);
        printf("Player 1 disconnected\n");
    }
    else if(!response2){
        printf("Player 2 disconnected\n");
        char responseM2[BUFFER_SIZE] = "Player 2 disconnected";
        char response[BUFFER_SIZE] = "no";
        send(newSocket2, response, sizeof(response), 0);
        send(newSocket1, responseM2, sizeof(responseM2), 0);
    }
    printf("Closing the server...\n");
    sleep(10);
    close(serverFD);
    close(newSocket1);
    close(newSocket2);
    return 0;

}
// gcc client.c -o client -Wall; gcc server.c -o server -Wall;


