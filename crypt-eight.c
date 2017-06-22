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

// To generate random seed
#include <time.h>

// Definition of data types in system calls
#include <sys/types.h>

// Include a number of definitions of structured needed for sockets
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// Used for thread
#include <pthread.h>

#include "crypt-eight.h"

// Start KEY_SIZE must always be less than BUF_SIZE !
#define BUF_SIZE 1024
#define KEY_SIZE 5


/**
 * [main description]
 * @param  argc [description]
 * @param  argv [description]
 * @return      [description]
 */
int     main(int argc, char *argv[])
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
    srand(time(NULL));


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


/**
 * [client description]
 * @param  port [description]
 * @param  host [description]
 * @return      [description]
 */
int     client(int port, char* host)
{
    // For explanation regarding how to create sockets, see create_socket() function
    struct sockaddr_in server;
    char buffer[BUF_SIZE] = {0};
    int sockfd;
    int mutual_key_size = 1 + (2 * KEY_SIZE * sizeof(char));

    char* crypted_message = NULL;
    char *mutual_key = NULL;
    char *new_key = NULL;
    char *key = NULL;


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
    printf("%s\n", buffer);


    // Generate random key and send it to the server
    printf("Socket are connected, attempting establishing secure connection...\n");
    key = generate_key();
    if (key == NULL)
        print_error("Error while establishing secure connection");

    if (write(sockfd, key, KEY_SIZE) < 0)
        print_error("Erreur while sending key to the server.\n");
    free(key);


    // Get generated key from server
    mutual_key = malloc(mutual_key_size);
    if (mutual_key)
        printf("TODO");

    if (read(sockfd, mutual_key, sizeof(mutual_key)) < 0)
        print_error("Error while reading key of client");
    else
        printf("Connection is secure. Mutual key : %s\n", mutual_key);
    
    new_key = malloc(mutual_key_size);
    if (key == NULL)
        printf("TODO");
        
    crypted_message = malloc(BUF_SIZE);
        if (key == NULL)
            printf("TODO");
            


    // Connection loop
    while (1)
    {
        printf("You: ");


        // Write to buffer
        fflush(stdout);
        memset(&buffer, 0, sizeof(buffer));
        fgets(buffer, BUF_SIZE, stdin);
        check_buffer(buffer);

        // Encrypt what's been written
        memcpy(crypted_message, buffer, BUF_SIZE);
        xor_encrypt_decrypt(crypted_message, buffer, mutual_key);
        // printf("Crypted string: %s\n", output);
        /* for (int i = 0; i < strlen(output); ++i)
            printf("%d ", buffer[i]);
            // printf("%x ", buffer[i] & 0xff);
        printf("\n"); */


        // Generate new key from encrypted message
        // generate_new_key(mutual_key, crypted_message, new_key);
        // memcpy(mutual_key, new_key, mutual_key_size);

        // Send it to server
        if (write(sockfd, crypted_message, BUF_SIZE) < 0)
            print_error("Erreur while writing to the server.\n");
    }


    free(new_key);
    free(mutual_key);
    free(crypted_message);
    free(mutual_key);

    close(sockfd);
    return 0;
}


/**
 * [connection_handler description]
 * @param  arg [description]
 * @return     [description]
 */
void    *connection_handler(void *arg)
{
    int client_socketfd = *(int*) arg;
    char buffer[BUF_SIZE] = {0};
    int bytes = 0;

    char *client_key = NULL;
    char *key = NULL;
    char *mutual_key = NULL;


    // Send acknowledgment of connection
    if (write(client_socketfd, "Connection established", 22) < 0)
        print_error("Error writing to socket");


    // Get key from client
    client_key = malloc(KEY_SIZE * sizeof(char));
    if (client_key == NULL)
        print_error("Could not allocate memory for client key");
        
    if (read(client_socketfd, client_key, BUF_SIZE) < 0)
        print_error("Error while reading key of client");


    // Generate random key and concats it to client key
    key = generate_key();
    if (key == NULL)
        print_error("Error while establishing secure connection");

    mutual_key = malloc(BUF_SIZE * sizeof(char));
    if (mutual_key == NULL)
        print_error("Could not allocate memory for mutual key");

    strcpy(mutual_key, client_key);
    strcat(mutual_key, key);

    free(key);
    free(client_key);


    // Send mutual key to client
    if (write(client_socketfd, mutual_key, strlen(mutual_key)) < 0)
        print_error("Error sending back mutual key to client");

    printf("Connection is secure. Mutual key: %s\n", mutual_key);
    printf("Reading from client...\n");
    char* decrypted_message = NULL;
    decrypted_message = malloc(BUF_SIZE);



    // Reading loop
    while ((bytes = read(client_socketfd, buffer, BUF_SIZE)) > 0)
    {
        memcpy(decrypted_message, buffer, BUF_SIZE);

        xor_encrypt_decrypt(decrypted_message, buffer, mutual_key);

        printf("Client: %s\n", decrypted_message);
        memset(buffer, 0, sizeof(bytes));
    }

    if (bytes == 0)
    {
        printf("Client disconnected...\n");
        fflush(stdout);
    }
    if (bytes == -1)
        print_error("Error while reading from client");


    free(decrypted_message);
    free(mutual_key);

    close(client_socketfd);
    return 0;
}


/**
 * [xor_encrypt_decrypt description]
 * @param  output [description]
 * @param  input  [description]
 * @return        [description]
 */
void    xor_encrypt_decrypt(char *output, char *input, char* key)
{
    for(int i = 0; i < strlen(input); ++i)
        output[i] = input[i] ^ key[i % (sizeof(key) / sizeof(char))];

}


/**
 * [generate_key description]
 * @return [description]
 */
char*   generate_key()
{
    char *random_string = rand_string_alloc(KEY_SIZE + 1);

    return random_string;
}


/**
 * [generate_new_key description]
 * @param  mutual_key      [description]
 * @param  crypted_message [description]
 * @param  new_key         [description]
 * @return                 [description]
 */
void    generate_new_key(char* mutual_key, char* crypted_message, char* new_key)
{
    for (int i = 0; i < 10; ++i)
    {
        *new_key++ = *mutual_key++;
        *new_key++ = *crypted_message++;
    }

    *new_key = '\0';
}


/**
 * [server description]
 * @param  port [description]
 * @return      [description]
 */
int     server(int port)
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


int     create_socket(int port)
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


/**
 * Get rid of the carriage return and empty buffer if needed
 * @param  buffer [description]
 * @return        [description]
 */
void    check_buffer(char *buffer)
{
    char *backslash_n = strchr(buffer, '\n');
    if (backslash_n != NULL)
    {
        *backslash_n = '\0';
    } else {
        // Empty buffer
        int c = 0;
        while (c != '\n' && c != EOF)
            c = getchar();
    }
}


/**
 * [rand_string_alloc description]
 * @param  size [description]
 * @return      [description]
 */
char*   rand_string_alloc(size_t size)
{
    char *s = malloc(size + 1);
    if (s != NULL)
        rand_string(s, size);

    return s;
}


/**
 * [rand_string description]
 * @param  str  [description]
 * @param  size [description]
 * @return      [description]
 */
char    *rand_string(char *str, size_t size)
{
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    if (size)
    {
        --size;
        for (size_t n = 0; n < size; n++)
        {
            int key = rand() % (int) (sizeof charset - 1);
            str[n] = charset[key];
        }

        str[size] = '\0';
    }
    return str;
}


/**
 * [print_error description]
 * @param  msg [description]
 * @return     [description]
 */
void    print_error(const char *msg)
{
    perror(msg);
    exit(1);
}


/**
 * [print_usage description]
 */
void    print_usage()
{
    printf("\nUSAGE");
    printf("\n\t./crypt-eight [port] [\"client\" [host] | \"serveur\"]");

    printf("\n\nEXAMPLE");
    printf("\n\t./crypt-eight 3333 client 127.0.0.1");

    printf("\n\nDESCRIPTION");
    printf("\n\tInitialize a server for a FTP connection and listen connection in <port>.\n\n");
    exit(1);
}


/**
 * [str_to_int description]
 * @param  str [description]
 * @return     [description]
 */
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


/**
 * [is_number description]
 * @param  c [description]
 * @return   [description]
 */
int     is_number(char c)
{
    return (c >= '0' && c <= '9');
}
