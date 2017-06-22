/**
 * Created by soulierantoine
 * 19/06/2017
 *
 * A simple server in the internet domain using TCP
 * The user must specify if the program must act as a server or as a client
 * If the program act as a client, the host must be specified
 * The port number is also passed as an argument
 */

// Standard C library
#include <stdlib.h>

// Input / output
#include <stdio.h>

// Utility functions for strings
#include <string.h>

// Definition of data types in system calls
#include <sys/types.h> 
#include <sys/wait.h>

// Include a number of definitions of structured needed for sockets
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// Used for thread
#include <signal.h>
#include <pthread.h>

// GMP
#include "lib/gmp-6.1.0/include/gmp.h"

#include "crypt-eight.h"

#define BUF_SIZE 256



int 	main(int argc, char *argv[])
{
	// Print args (debugging purposes)
	/* printf("argc = %d\n", argc);
	for (int i = 0; i < argc; ++i)
		printf("%d -> %s\n", i, argv[i]); */

	// Check args
	if (argc < 3)
	{
		print_usage();
		exit(1);
	}


	int port = str_to_int(argv[1]);
    if (port == 0)
		print_error("The port is invalid");


	if ((argc == 4) && (strcmp(argv[2], "client") == 0))
	{
		return client(port, argv[3]);
	}

	if ((argc == 3) && (strcmp(argv[2], "serveur") == 0))
	{	
		return server(port);
	}

	print_usage();
	exit(1);
}


int 	client(int port, char* host)
{
	// For explanation regarding how to create sockets, see create_socket() function
	struct sockaddr_in server;
	char buffer[BUF_SIZE] = {0};
	int sockfd;

	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET; 
	server.sin_port = htons(port);
	inet_pton(AF_INET, host, &server.sin_addr);
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
        print_error("Error while creating client socket");


    // Establish a connection between the client and the server
	if (connect(sockfd, (struct sockaddr*) &server, sizeof(server)) < 0)
		print_error("Error while attempting to connect");

    if (read(sockfd, buffer, BUF_SIZE) < 0)
        print_error("Erreur while reading from the server.\n");
    
    printf("From server: %s\n", buffer);
	
	// TODO
	printf("Socket are connected, attempting establishing secure connection...\n");

	while (1)
	{	
		printf("You: ");

		fflush(stdout); 
		memset(&buffer, 0, sizeof(buffer));

		fgets(buffer, BUF_SIZE, stdin);

        /*
         * Chiffrement de Nawako ! Attention tout peut péter !
         */
        char key[] = {'C'}; //Can be any chars, and any size array

        char* output = NULL;
        output = malloc(BUF_SIZE);
        memcpy(output, buffer, BUF_SIZE);
        // encryptDecrypt(buffer, encrypted, key);
        encryptDecrypt(output, buffer);
        printf("Buffer: %s\n", buffer);

        // memset(&buffer, 0, sizeof(buffer));
        // encryptDecrypt(buffer, key);
        printf("Buffer: %s\n", output);

        // char decrypted[strlen(buffer)];
        // encryptDecrypt(encrypted, decrypted, key);
        // printf("Decrypted:%s\n", decrypted);
        // =====================================================

		if (write(sockfd, output, BUF_SIZE) < 0)
	        print_error("Erreur while writing to the server.\n");
	}

    close(sockfd);
	return 0;
}

//void xor_encrypt(char *key, char *string)
//{
//    int i, string_length = strlen(string);
//    for(i=0; i<string_length; i++)
//    {
//        string[i]=string[i]^key[i];
//        // printf("%i", string[i]);
//    }
//}

// void encryptDecrypt(char *input, char *output, char *key)
void encryptDecrypt(char *output, char *input)
{
    char key[] = {'K', 'C', 'Q'}; //Can be any chars, and any size array

    for(int i = 0; i < strlen(input); ++i) {
        // output[i] = input[i] ^ key[i % (sizeof(key)/sizeof(char))];
        output[i] = input[i] ^ key[i % (sizeof(key) / sizeof(char))];
    }
}

void *connection_handler(void *arg)
{
	int client_socketfd = *(int*) arg;
	char buffer[BUF_SIZE] = {0};
	int bytes = 0;

    if (write(client_socketfd, "Connection established", 22) < 0)
        print_error("Error writing to socket");
    
    printf("Reading from client...\n");

    while ((bytes = read(client_socketfd, buffer, BUF_SIZE)) > 0)
    {
        /*
         * Déchiffrement de Nawako ! Attention tout peut péter !
         */
        //char key[] = {'C'}; //Can be any chars, and any size array

        // char decrypted[strlen(buffer)];
        char* output = NULL;
        output = malloc(BUF_SIZE);
        memcpy(output, buffer, BUF_SIZE);
        encryptDecrypt(output, buffer);

        printf("Client: %s\n", buffer);
        printf("Client: %s\n", output);
    	memset(buffer, 0, sizeof(bytes));
        // =====================================================
    }

    if (bytes == 0)
    {
        printf("Client disconnected...\n");
        fflush(stdout);
    }
    if (bytes == -1)
        print_error("Error while reading from client");

	close(client_socketfd);
	return 0;
}



int 	server(int port)
{
    if (port < 1025 || port > 65535)
    {
        print_error("The port is invalid, it must be between 1025 and 65535");
        exit(1);
    }

    // Thread id
	pthread_t tid;
	int socketfd = create_socket(port);
	int *client_socketfd = NULL;
	
	struct sockaddr_in client;
	socklen_t client_len = sizeof(client);

	printf("Server running...\n");

	// accept() pause the process until a connection arrives
	// client will store the address (in network byte order) and the port of the client
	// Returns the socket file descriptor of the client, 0 if an error occurs 
	while (1)
	{
		// client_socketfd must contain the client socket descriptor on other place in the memory
		client_socketfd = malloc(sizeof(int));
		*client_socketfd = accept(socketfd, (struct sockaddr*) &client, &client_len);
		
		// Each threads must have its own descriptor that it'll handle
		// So we pass in the newly connected client socket descriptor
        if (pthread_create(&tid, NULL, &connection_handler, (void*) client_socketfd) < 0)
            print_error("Could not create thread to handle connection");
	}

	printf("Shutting down server...");
	close(socketfd);

	return 0;
}


int 	create_socket(int port)
{
    // Struct describing the socket of the server and the client
    struct sockaddr_in server;

	// socket() creates a socket able to start a connection
	// SOCK_STREAM == TCP
	// We let the third parameter to 0 to let the kernel determine the protocol 
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0)
        print_error("Error while creating server socket");

    // Initiate the structure to 0
	memset(&server, 0, sizeof(server));

	// Code for the address family, AF_INET == IPv4
	server.sin_family = AF_INET;

	//  Set the server to listen to any interface
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	
	// Set the port number and convert it to network byte order
	server.sin_port = htons(port);

	// Connect the socket
	if (bind(socketfd, (struct sockaddr *) &server, sizeof(server)) < 0)
		print_error("Error while binding the server socket");

	// Set socket to be passing, pending client connections (max 10)
	listen(socketfd, 10);

	return socketfd;
}



/**
 * Utility functions
 */

void 	print_error(const char *msg)
{
    perror(msg);
    exit(1);
}


void	print_usage()
{
    printf("\nUSAGE");
    printf("\n\t./crypt-eight [port] [\"client\" [host] | \"serveur\"]");

    printf("\n\nEXAMPLE");
    printf("\n\t./crypt-eight 3333 client 127.0.0.1");
    
    printf("\n\nDESCRIPTION");
    printf("\n\tInitialize a server for a FTP connection and listen connection in <port>.\n\n");
    exit(1);
}


int     str_to_int(char *str)
{
    if (str == '\0')
        return 0;

    int result = 0, sign = 1, i = 0;

    if (str[0] == '-')
    {
        sign = -1;
        ++i;
    }

    while (str[i] != '\0')
    {
        if (!is_number(str[i]))
            return 0;
        result = result * 10 + str[i] - '0';
        ++i;
    }

    return (sign * result);
}


int		is_number(char c)
{
    return (c >= '0' && c <= '9');
}
