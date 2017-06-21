#ifndef _CRYPT_EIGHT_H
#define _CRYPT_EIGHT_H


// Main functions header

int 		client(int port, char* host);
int 		server(int port);
int			create_socket(int port);
void 		*connection_handler(void *arg);
// void        encryptDecrypt(char *input, char *output, char *key);
void encryptDecrypt(char *output, char *input);

// Utility functions header

void 		print_error(const char *msg);
void 		print_usage();
int     	str_to_int(char *str);
int			is_number(char c);


#endif
