
# NetTacToe 🎮🌐
![Waketime](https://img.shields.io/badge/Waketime-64%20hrs%2042%15mins-blueviolet?style=flat&labelColor=black&logo=clock&logoColor=white)

**NetTacToe** is a multiplayer Tic-Tac-Toe game developed in C, implementing both **TCP** (Transmission Control Protocol) and **UDP** (User Datagram Protocol) protocols for communication between players. The game allows two players to play Tic-Tac-Toe over a network, with one server hosting the game and clients connecting to it.

---

Here’s a nicely formatted "Authors" section for your document:

---

## Authors 🧑‍💻

**Shreyas Mehta**  

## Index 📚
- [File Structure](#file-structure)
- [Description](#description)
- [Features](#features)
- [How to Compile and Run](#how-to-compile-and-run)
  - [For TCP](#for-tcp)
  - [For UDP](#for-udp)
- [Code Explanation](#code-explanation)
    - [TCP Implementation](#tcp-implementation)
        - [Client Implementation](#client-implementation)
        - [Server Implementation](#server-implementation)
    - [UDP Implementation](#udp-implementation)
        - [Client Implementation](#client-implementation-1)
        - [Server Implementation](#server-implementation-1)
- [Summary](#summary)
- [Next Steps/Improvements](#next-stepsimprovements)

## File Structure 

```
NetTacToe/
│
├── README.md        # Project documentation
├── tcp/              # TCP implementation files
│   ├── client.c      # Client-side code for TCP
│   ├── Makefile      # Makefile for building the TCP project
│   └── server.c      # Server-side code for TCP
│
└── udp/              # UDP implementation files
    ├── client        # Compiled output for UDP client
    ├── client.c      # Client-side code for UDP
    ├── Makefile      # Makefile for building the UDP project
    ├── server        # Compiled output for UDP server
    └── server.c      # Server-side code for UDP
```

---

## Description 

This project includes two implementations of a multiplayer Tic-Tac-Toe game, one using **TCP** (Transmission Control Protocol) and the other using **UDP** (User Datagram Protocol).

- **TCP Version**: The TCP implementation ensures reliable communication with guaranteed message delivery, ideal for the game where synchronization is crucial. 💻🔗
  
- **UDP Version**: The UDP implementation focuses on low-latency communication, sacrificing some reliability in exchange for speed. ⚡📡

- This project was part of the **Operating Systems and Networks** course at **IIIT Hyderabad**.

## Features 

- The game allows two players to play Tic-Tac-Toe over a network, with one server hosting the game and clients connecting to it.
- The server manages the game state, validates moves, and sends updates to the clients.
- The clients send their moves to the server and receive updates on the game state.
- The game continues until one player wins or the game ends in a draw.
- Option for a rematch after the game ends.
- The game supports both TCP and UDP protocols for communication between the server and clients.
- The TCP version ensures reliable communication with guaranteed message delivery.
- The UDP version focuses on low-latency communication, sacrificing some reliability in exchange for speed.
- The game is implemented in C, using socket programming for network communication.
- The project includes separate implementations for TCP and UDP, allowing for a comparison of the two protocols.

---

## How to Compile and Run 

### For TCP:
1. Navigate to the `tcp/` directory.
2. Run `make` to compile the code.
3. Run the server using:  
   ```bash
   ./server
   ```
4. Run the client using:  
   ```bash
   ./client
   ```
5. To clean the compiled files, run:  
   ```bash
   make clean
   ```
### For UDP:
1. Navigate to the `udp/` directory.
2. Run `make` to compile the code.
3. Run the server using:  
   ```bash
   ./server
   ```
4. Run the client using:  
   ```bash
   ./client
   ```
5. To clean the compiled files, run:  
   ```bash
    make clean
    ```

---

## Code Explanation 

### TCP Implementation 
- **client.c**: This file contains the client-side logic for the TCP connection.

#### Client Implementation 
The `client.c` file implements the client-side logic for the **TCP** version of the Tic-Tac-Toe game. Below is a detailed explanation of the code and its functions.

#### Header Inclusions 📚

```c
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
```

These are standard C libraries included to support socket programming, string manipulation, input/output, and Boolean operations.

#### Constants and Colors 🌈

```c
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
```

- `PORT`: The port number used for the server communication.
- `BUFFER_SIZE`: The size of the buffer for reading and writing data.
- Color codes (`RED`, `GREEN`, `YELLOW`, `CYAN`, `RESET`): These are used for terminal text formatting.
- `winner1`, `winner2`, `draw`, `NOT_FINISHED`: These constants represent different states in the game.

#### Main Function Setup 🚀

```c
int main() {
    int clientSocket = 0, valread;
    struct sockaddr_in serverAddr;
    socklen_t serverAddrLen = sizeof(serverAddr);
    char buffer[BUFFER_SIZE] = {0};
    char input[BUFFER_SIZE] = {0};
```

- `clientSocket`: A variable that holds the client socket descriptor.
- `serverAddr`: A structure that contains server address information.
- `buffer` and `input`: Buffers for sending and receiving messages.

#### Creating and Connecting the Socket 📡

```c
if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror(RED"Socket creation failed"RESET);
    exit(EXIT_FAILURE);
}

serverAddr.sin_family = AF_INET;
serverAddr.sin_addr.s_addr = INADDR_ANY;
serverAddr.sin_port = htons(PORT);

if (inet_pton(AF_INET, "172.20.10.6", &serverAddr.sin_addr) <= 0) {
    perror(RED"\nInvalid address/ Address not supported \n"RESET);
    exit(EXIT_FAILURE);
}

if (connect(clientSocket, (struct sockaddr *)&serverAddr, (socklen_t)serverAddrLen) < 0) {
    perror(RED"Connection failed"RESET);
    exit(EXIT_FAILURE);
}
```

- A TCP socket is created using the `socket()` function.
- The `serverAddr` is initialized with the server's IP address and port.
- The `connect()` function is used to establish a connection to the server.

#### Receiving the Player ID  🆔

```c
recv(clientSocket, buffer, BUFFER_SIZE, 0);
printf("%s", buffer);
int playerId;
if(strstr(buffer, "Player 1")) {
    playerId = 1;
}
if(strstr(buffer, "Player 2")) {
    playerId = 2;
}
```

- The client receives a message from the server, which indicates whether the client is Player 1 or Player 2.

#### Game Loop 🔄


The game loop starts with displaying the game board:

```c
printf(CYAN);
printf("Board:\n");
printf(" 1 | 2 | 3 \n");
printf("---|---|---\n");
printf(" 4 | 5 | 6 \n");
printf("---|---|---\n");
printf(" 7 | 8 | 9 \n");
```

The game board is displayed, showing numbered cells. Then, the client enters a loop where it keeps receiving updates from the server and sending moves.

#### Receiving Server Messages 📩

```c
while (true) {
    memset(buffer, 0, BUFFER_SIZE);
    valread = recv(clientSocket, buffer, BUFFER_SIZE, 0);
    if (valread <= 0) {
        fprintf(stderr, RED"[Connection lost from the server]\n"RESET);
        exit(EXIT_FAILURE);
        break;
    }

    if(strstr(buffer, "Invalid")) {
        printf(RED"%s\n"RESET, buffer);
    }

    if(strstr(buffer, "Waiting")) {
        printf(YELLOW"%s\n"RESET, buffer);
    }

    if(strstr(buffer, "Updating")) {
        printf(GREEN"Updating the board ...\n"RESET);
    }

    if (strstr(buffer, "Board\n")) {
        // Display the updated board
    }

    // Check if the game is over
    if (strstr(buffer, "wins")) {
        printf(GREEN"[You Win]\n"RESET);
        break;
    }
    else if (strstr(buffer, "Draw")) {
        printf(YELLOW"[Match Drawn]\n"RESET);
        break;
    }
    else if (strstr(buffer, "loses")) {
        printf(RED"[You Lose]\n"RESET);
        break;
    }
}
```

- The client keeps receiving messages from the server.
- Depending on the message content, it updates the board, displays the game state (waiting, invalid move, board updates, etc.), or announces the winner.

#### Sending User Input 📤

```c
if (strstr(buffer, "Your Turn Player")) {
    printf(YELLOW"Enter move [row col]: ");
    fgets(input, BUFFER_SIZE, stdin);
    send(clientSocket, input, strlen(input), 0);
}
```

- If it's the player's turn, the client prompts the user to enter a move (row and column).
- The move is sent to the server using the `send()` function.

#### Restarting or Ending the Game 🔄

```c
printf(YELLOW"Do you want to play again? [yes/no]: "RESET);
fgets(input, BUFFER_SIZE, stdin);
send(clientSocket, input, strlen(input), 0);
```

- After the game ends, the client asks the player if they want to play again.
- The response is sent to the server.

#### Closing the Connection 🚪

```c
memset(buffer, 0, BUFFER_SIZE);
strcpy(buffer, "exit");
send(clientSocket, buffer, strlen(buffer), 0);
close(clientSocket);
```

- The client sends an "exit" message to the server and then closes the socket to end the connection.

---
#### Server Implementation 
- **server.c**: This file contains the server-side logic for the TCP connection.

### 1. **Imports and Definitions** 📚:
   - The code includes various libraries for socket programming (`<sys/socket.h>`, `<netinet/in.h>`, etc.) and other functionalities like string manipulation and threading.
   - Constants are defined for the port (`PORT`), the board size (`MAX`), and buffer size (`BUFFER_SIZE`).
   - `Symbol_X` and `Symbol_O` represent the two possible player symbols in the game.
   - Game states are defined, including winning conditions and draw.

### 2. **Global Variables** 🌐:
   - `board[3][3]`: The Tic-Tac-Toe board, represented as a 3x3 grid of characters.
   - `player`: Keeps track of whose turn it is (1 for player 1, 2 for player 2).
   - `player1` and `player2`: Socket file descriptors for the two players.
   - `response1` and `response2`: Flags to check if both players are still connected.

### 3. **Functions** 🛠️:

   - **`initBoard()`**: Initializes the game board, filling it with spaces (`' '`).
   
   - **`printBoard()`**: Prints the current state of the game board to the console.
   
   - **`updateBoard(int row, int column, int player)`**: Updates the board at the specified position if the move is valid (i.e., the cell is empty).
   
   - **`checkWinner()`**: Checks the current board to determine if there is a winner or if the game is a draw. It checks rows, columns, and diagonals for three matching symbols. It returns `winner1`, `winner2`, `draw`, or `NOT_FINISHED`.
   
   - **`sendBoard(int newSocket)`**: Sends the current game board to the client via the provided socket.
   
   - **`trimWhitespace(char *str)`**: Trims leading and trailing whitespaces from a string.
   
   - **`extractRowAndCol(char *buffer, int *row, int *column)`**: Extracts the row and column numbers from the client input (which should be in the form of `row,column`).
   
   - **`handleGame(void *arg)`**: This function manages the main game loop. It alternates between players, sending game updates and receiving moves. It handles invalid moves, checks for a winner, and sends the result back to both players. It also updates the board after each valid move.
   
### 4. **Main Function** 🚀:
   - **`main()`**: This is the entry point of the server. It sets up the server socket, binds it to the specified port, and listens for incoming connections.
     - The server first accepts connections from two players (Player 1 and Player 2).
     - Once both players are connected, the `handleGame()` function is called to manage the game.
     - The game continues until one player wins, the game is drawn, or a player disconnects.
     - After the game ends, the server closes the connections and exits.

### 5. **Flow of the Game** 🔄:
   - The game alternates between two players (Player 1 and Player 2), sending messages to indicate whose turn it is.
   - Players send their moves as `row,column` input, which is processed and updated on the board.
   - After each move, the board is sent to both players to update their view of the game.
   - The server checks if there is a winner or a draw after each move and informs both players of the game result.

### 6. **Handling Disconnections** 🚪:
   - The server periodically checks if a player has disconnected by receiving `"no"` from the client. If a player disconnects, the game ends, and the server sends a message to the other player.

### 7. **Closing the Server** 🛑:
   - When the game ends or a player disconnects, the server closes all open sockets and exits.

### Summary of the Client-Server Interaction: `server.c` and `client.c` (TCP) 🔄:
   - **Server**: Waits for two clients to connect, then alternates turns between them, updating the board and checking for a winner or draw.
   - **Client**: Sends player moves to the server, receives the updated board, and is notified when it's their turn or if the game has ended.

### Next Steps/Improvements: `server.c` and `client.c` (TCP) 🛠️:
   - **Error Handling**: More robust error handling can be added, especially for network-related issues.
   - **Concurrency**: For handling more players or multiple games simultaneously, threading or a more scalable architecture would be needed.
   - **GUI**: The current implementation is text-based. A graphical user interface (GUI) for the game could improve the user experience.

This server is a good starting point for a simple multiplayer game over TCP, where the server handles the logic and communication between two players.

### UDP Implementation 

- **client.c**: This file contains the client-side logic for the UDP connection.
#### Client Implementation 
  This `udp/client.c` file is a UDP-based client for a two-player game (likely Tic-Tac-Toe). The client connects to a server, participates in the game by sending and receiving messages, and can play multiple rounds. Here's a breakdown of its functionality:

### Key Features and Functions: `client.c` (UDP) 
1. **Socket Creation**: 
   - The client creates a UDP socket to communicate with the server.
   
2. **Server Connection**:
   - The server is at IP `172.20.10.6`, and the client communicates with it using UDP on port `8080`.
   
3. **Joining the Game**: 
   - The client sends a "Join Game" message to the server and waits for a welcome message indicating whether it is Player 1 or Player 2.

4. **Game Loop**:
   - The client enters a loop where it continuously displays the game board, checks for messages (like invalid moves, game updates, etc.), and listens for responses from the server.
   - The game board is displayed, and the client makes a move by entering the row and column of the desired move. The server updates the board and responds accordingly.
   
5. **Game Outcome**:
   - The client checks for game outcomes like winning, losing, or drawing by looking for specific server messages (e.g., "wins", "loses", "Draw").
   
6. **Restart/Exit**:
   - After the game ends, the client is prompted to ask whether they want to play again. If yes, it restarts the game; if no, the game ends.

7. **Color Coding**: 
   - The client uses different colors to print messages (using ANSI escape codes):
     - Red for errors, player losses, and disconnections.
     - Green for successful actions and winning.
     - Yellow for updates or waiting states.
     - Cyan for board display and game updates.
<!-- add emojis -->
### Flow: `client.c` (UDP) 
- The client continuously listens for responses from the server, updates the game board, and prompts the user for input during their turn.
- If the game ends, it checks for disconnections and whether the player wants to restart the game or exit.

### Key Functions: `client.c` (UDP)
- `sendto` sends data to the server (game commands, player moves, etc.).
- `recvfrom` receives responses from the server (game state, board updates, etc.).

### Error Handling: `client.c` (UDP)
- It checks for lost connections (e.g., "Connection lost from the server") and prints error messages if necessary.
- If a player disconnects, the game ends for both players with a message showing who disconnected.

This client expects a functioning server that can handle UDP communication and respond with appropriate game states.
  
- **server.c**: This file contains the server-side logic for the UDP connection.

#### Server Implementation 
  This `udp/server.c` file is a UDP-based server for a two-player game (likely Tic-Tac-Toe). The server listens for client connections, manages the game state, and sends updates to clients. Here's a breakdown of its functionality:
  This is the server-side code for a two-player UDP-based Tic-Tac-Toe game. The game alternates turns between two players, sending board updates and handling player input. Below is a breakdown of the key sections:

### Key Components: `server.c` (UDP) 
1. **Initialization**:
   - `initBoard()`: Initializes the Tic-Tac-Toe board with empty spaces.
   - `printBoard()`: Displays the board with 'X' and 'O' symbols.
   - `updateBoard()`: Updates the board after a player's move.
   - `checkWinner()`: Checks if there is a winner or a draw based on the current state of the board.

2. **Game Logic**:
   - The game alternates between two players (Player 1 and Player 2).
   - The server waits for a move from the current player, updates the board, checks for a winner or draw, and sends the updated board and result to both players.
   - The game ends when either a player wins or a draw occurs.

3. **Network Handling**:
   - The server is a UDP socket-based server that listens on port `8080` and communicates with the clients (players) using `sendto()` and `recvfrom()`.
   - It handles two players, ensuring that the game is synchronized between them, by sending messages like "Your Turn", "Waiting for other player", and "Invalid Move".

4. **Restarting or Ending the Game**:
   - After a game finishes (either through a win or draw), the server checks if the players want to play again by listening to their responses.
   - If either player doesn't want to play again, the server sends a "disconnected" message and ends the game.
   - The game can be restarted if both players agree.

### Key Functions: `server.c` (UDP) 
- **`sendBoard()`**: Sends the current game board to both players.
- **`trimWhitespace()`**: Trims leading and trailing whitespace from player input.
- **`extractRowAndCol()`**: Extracts the row and column from the player’s input (which is expected in a specific format).

### Client Communication: `server.c` (UDP) 
The game expects the clients (players) to send their moves in the format "row,col" (e.g., "1,2" for the second column of the first row). The server checks if the move is valid and updates the board accordingly.

### Handling Player Disconnects: `server.c` (UDP)
- If a player disconnects or chooses not to play again, the server broadcasts a message to the remaining player and exits the loop.

### Suggestions for Improvement: `server.c` and `client.c` (UDP)
1. **Error Handling**: Ensure that network failures or unexpected behaviors are handled gracefully (e.g., player disconnection during the game).
2. **Timeouts**: Implement a timeout for waiting for player moves to avoid hanging if one player does not respond in a timely manner.
3. **Improved User Input**: Input validation can be enhanced, especially for moves (e.g., handling invalid input better).


---

## Summary

**NetTacToe** is a multiplayer Tic-Tac-Toe game developed in C, implementing both **TCP** and **UDP** protocols for communication between players. The game allows two players to play Tic-Tac-Toe over a network, with one server hosting the game and clients connecting to it. The TCP version ensures reliable communication with guaranteed message delivery, while the UDP version focuses on low-latency communication. The project includes separate implementations for TCP and UDP, allowing for a comparison of the two protocols.

---

## Next Steps/Improvements

- **Error Handling**: Implement more robust error handling for network issues and unexpected behaviors.
- **Concurrency**: Explore threading or other methods for handling multiple players or games simultaneously.
- **GUI**: Develop a graphical user interface (GUI) for the game to enhance the user experience.
- **Timeouts**: Add timeouts for player moves to prevent hanging if a player takes too long.
- **Input Validation**: Improve input validation for moves and other user inputs to handle invalid data better.
