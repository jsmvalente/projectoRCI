//
//  UDP.h
//  projectoRCI
//
//  Created by João Valente on 08/03/2017.
//  Copyright © 2017 João Valente. All rights reserved.
//

#ifndef UDP_h
#define UDP_h

#include "Defs.h"

#endif /* UDP_h */

int RequestServerList(int fd, struct sockaddr_in serveraddr);
int SendMessageToServer(int fd, ServerListNode * head, char* message);
int CreateSocket();
char* ReceiveServerList(int fd, struct sockaddr_in serveraddr);
char* ReceiveMessagesFromServer(int fd, ServerListNode * head);
int RequestMessagesFromServer(int msg_fd, ServerListNode * head, int number_messages);
struct sockaddr_in SetupIDServerAddress(char * id_server_ip, int id_server_port);
