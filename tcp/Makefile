# Variables for the compiler and compiler flags
CC = gcc
CFLAGS = -Wall
CLIENT = client
SERVER = server
CLIENT_SRC = client.c
SERVER_SRC = server.c

# Default target to build both client and server
all: $(CLIENT) $(SERVER)

# Rule to build the client
$(CLIENT): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $(CLIENT) $(CLIENT_SRC)

# Rule to build the server
$(SERVER): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $(SERVER) $(SERVER_SRC)

# Clean up the build files
clean:
	rm -f $(CLIENT) $(SERVER)

# Phony targets so they are always executed
.PHONY: all clean
