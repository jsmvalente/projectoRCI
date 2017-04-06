#ifndef MESSAGE_FUNCTIONS_
#define MESSAGES_FUNCTIONS_

#define MAXCHAR 10240
typedef struct MESSAGES
{
  char *text;
  int time_message;
} MESSAGE;

typedef struct SERVERS
{
    char name[MAXCHAR];
    char ip[MAXCHAR];
    int udp_port;
    int tcp_port;
    int connect;
} SERVER;

MESSAGE receive_message(char* buffer);
int strcount(char *s, char ch);
int server_count(char *serverlist);
void insert_server(SERVER *servlist, char *buffer, char *server_name);
void connect_server(SERVER *servlist, int *fd, int num_servers);
void receivemessages(MESSAGE *msg, char *readbuffer, int *message_index);
void clientmessage(char *buffer_client, MESSAGE *msg, int n_wanted_msg, int message_index, int logic_timer);
void insertmessage(char *buffer_client, MESSAGE *msg, int *logic_timer, int *message_index);
void tcpmessage(char *readbuffer, MESSAGE *msg, int message_index, int logic_timer);
void count_connected(SERVER *servlist, int *num_connected, int num_servers);

#endif
