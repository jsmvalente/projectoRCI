#ifndef USER_FUNCTIONS_
#define USER_FUNCTIONS_

int identify_user_commands(char *command);
void get_arguments(int argc, char *argv[], char **server_name, char **ip_address, char **udp_port, char **tcp_port, char **id_serverip, char **id_serverport, int *n_messages, int *int_reg);

#endif
