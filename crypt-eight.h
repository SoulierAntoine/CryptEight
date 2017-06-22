#ifndef _CRYPT_EIGHT_H
#define _CRYPT_EIGHT_H


// Main functions header

int 		client(int port, char* host);
int 		server(int port);
int			create_socket(int port);
void 		*connection_handler(void *arg);
void	    xor_encrypt_decrypt(char *output, char *input, char* key);
char*		generate_key();
void	    generate_new_key(char* mutual_key, char* crypted_message, char* new_key)


// Utility functions header

void 		print_error(const char *msg);
void 		print_usage();
int     	str_to_int(char *str);
int			is_number(char c);
void	    check_buffer(char *buffer);
char* 		rand_string_alloc(size_t size);
char* 		rand_string(char *str, size_t size);


#endif
