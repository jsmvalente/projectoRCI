#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include "message_functions.h"


#define MAXCHAR 10240

MESSAGE receive_message(char* buffer)
{
  MESSAGE msg;

  printf("Please insert message:\n");

  fgets(buffer, MAXCHAR, stdin);

  msg.text = malloc(strlen(buffer)+1);

  strcpy(msg.text, buffer);

  printf("%s\n", msg.text);

  return msg;
}

//Counts the number of a certain character in a string.
int strcount(char *s, char ch)
{
  int i, conta;

  for(i=conta=0 ; s[i]!='\0' ; i++)
  {
    if(s[i]==ch)
    {
      conta++;
    }
  }

  return conta;
}

//Counts the number of servers in the servlist.
int server_count(char *serverlist)
{
  int n=0;

  n = strcount(serverlist, '\n') - 2; //One of them is our server, and another is just a '\0' too much in the final.

  return n;
}

//Inserts the servers retrieved from ID server in the servlist.
void insert_server(SERVER *servlist, char *buffer, char *server_name)
{
  int serverindex=0;
  char *newbuffer;

  newbuffer = strtok(buffer, "\n");
  newbuffer = strtok(NULL, "\n");

  while(newbuffer != NULL)
  {
    sscanf(newbuffer, "%[^;]; %[^;]; %d; %d", servlist[serverindex].name, servlist[serverindex].ip, &servlist[serverindex].udp_port, &servlist[serverindex].tcp_port);
    servlist[serverindex].connect = 1;

    //Checks if it's not itself!
    if(strcmp(server_name, servlist[serverindex].name) != 0)
    {
      serverindex++;
    }

    newbuffer = strtok(NULL, "\n");
  }
}

//Connects to each server on the server list (servlist).
void connect_server(SERVER *servlist, int *fd, int num_servers)
{
  int i, n, errno;
  struct sockaddr_in serveraddr;

  for(i=0 ; i<num_servers ; i++)
  {
    fd[i] = socket(AF_INET, SOCK_STREAM, 0);
    if(fd[i] == -1)
    {
      exit(-1);
    }

    memset((void*)&serveraddr,(int) '\0', sizeof(serveraddr));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(servlist[i].ip);
    serveraddr.sin_port = htons((u_short) servlist[i].tcp_port);

    n = connect(fd[i], (struct sockaddr*) &serveraddr, sizeof(serveraddr));
    if(n == -1)
    {
      printf("Error\n");
      fprintf(stderr, "%s\n", strerror(errno));
      servlist[i].connect = 0;
    }
  }
}

//Puts the received messages from the servers connected in connect_server in the message list (msg[]).
void receivemessages(MESSAGE *msg, char *readbuffer, int *message_index)
{
  char *newbuffer, buffer[MAXCHAR];
  int n;

  newbuffer = strtok(readbuffer, "\n");
  newbuffer = strtok(NULL, "\n");

  while(newbuffer != NULL)
  {
    sscanf(newbuffer, "%d; %s", &msg[*message_index].time_message, buffer);

    msg[*message_index].text = (char *)malloc(sizeof(char)*(n+1));
    strcpy(msg[*message_index].text, buffer);

    n = strlen(buffer);

    *message_index = *message_index +1;
    newbuffer = strtok(NULL, "\n");
  }
}

//Creates the message to send to the client when requested.
void clientmessage(char *buffer_client, MESSAGE *msg, int n_wanted_msg, int message_index, int logic_timer)
{
  int i;
  char *message;

  strcpy(buffer_client, "MESSAGES\n");

  if(logic_timer != 0)
  {
    for(i = message_index - n_wanted_msg; i < message_index; i++)
    {
      if(i >= 0)
      {
        strcat(buffer_client, msg[i].text);
        strcat(buffer_client, "\n");
      }
    }
  }
}


//Inserts the message retrieved from client PUBLISH in the message list (msg[]).
void insertmessage(char *buffer_client, MESSAGE *msg, int *logic_timer, int *message_index)
{
  int message_size;

  *logic_timer = *logic_timer +1;

  message_size = strlen(buffer_client) - 8 + 1;

  msg[*message_index].text = (char*)malloc(sizeof(char)*(message_size));

  strcpy(msg[*message_index].text, buffer_client+8);

  msg[*message_index].time_message = *logic_timer;

  *message_index = *message_index +1;
}

//Creates the message to send to the server client connected by tcp.
void tcpmessage(char *readbuffer, MESSAGE *msg, int message_index, int logic_timer)
{
  int i;
  char trash[MAXCHAR];

  strcpy(readbuffer, "SMESSAGES\n");

  if(logic_timer != 0)
  {
    for(i = 0; i < message_index; i++)
    {
      sprintf(trash, "%d", msg[i].time_message);
      strcat(readbuffer, trash);
      strcat(readbuffer, ";");
      strcat(readbuffer, msg[i].text);
      strcat(readbuffer, "\n");
    }
  }
}

//Counts the number of servers connected through TCP in the server list (servlist[]);
void count_connected(SERVER *servlist, int *num_connected, int num_servers)
{
  int i;

  for(i=0; i < num_servers; i++)
  {
    if(servlist[i].connect == 1)
    {
      *num_connected = *num_connected +1;
    }
  }

}
