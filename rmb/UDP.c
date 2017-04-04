//
//  UDP.c
//  projectoRCI
//
//  Created by João Valente on 08/03/2017.
//  Copyright © 2017 João Valente. All rights reserved.
//

#include "Defs.h"

//Creates a new socket
int CreateSocket()
{
    int fd = 0;
    
    fd = socket(AF_INET, SOCK_DGRAM,0);
    
    if(fd == -1)
    {
        return -1;
    }
    
    return fd;
}

//Sends a message to the default message server
int SendMessageToServer(int fd, ServerListNode * head, char* message)
{
    int addrlen, port, ret;
    struct sockaddr_in serveraddr;
    char relay[148], *ip;
    
    //Get IP and port
    ip = GetUDPIPFromNode(head);
    port = GetUDPPortFromNode(head);
    
    memset((void*)&serveraddr,(int) '\0', sizeof(serveraddr));
    
    //Setup the server address
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(ip);
    serveraddr.sin_port = htons((u_short)port);
    
    addrlen = sizeof(serveraddr);
    
    //Get the message we have to realy oto the server into the "relay" string
    strcpy(relay,  "PUBLISH ");
    strcat(relay, message);
    
    //Send the message to the server
    ret = sendto(fd, relay, strlen(relay)+1, 0, (struct sockaddr*)&serveraddr,addrlen);
    
    return 1;
}

//Resquests that the server sends the last number_messages messages
int RequestMessagesFromServer(int msg_fd, ServerListNode * head, int number_messages)
{
    int addrlen, ret, port;
    struct sockaddr_in serveraddr;
    char relay[100], nmessages_string[87], *ip;
    
    //Get IP and port
    ip = GetUDPIPFromNode(head);
    port = GetUDPPortFromNode(head);
    
    memset((void*)&serveraddr,(int) '\0', sizeof(serveraddr));
    
    //Setup the server address
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(ip);
    serveraddr.sin_port = htons((u_short)port);
    
    addrlen = sizeof(serveraddr);
    
    //Get the message we have to realy oto the server into the "relay" string
    strcpy(relay, "GET_MESSAGES ");
    sprintf(nmessages_string, "%d", number_messages);
    strcat(relay, nmessages_string);
    
    //Send the message to the server
    ret = sendto(msg_fd, relay, strlen(relay)+1, 0, (struct sockaddr*)&serveraddr,addrlen);
    
    return 1;
}

struct sockaddr_in SetupIDServerAddress(char * id_server_ip, int id_server_port)
{
    struct sockaddr_in serveraddr;
    
    memset((void*)&serveraddr,(int) '\0', sizeof(serveraddr));
    
    //Setup the server address
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = ((struct in_addr *)id_server_ip)->s_addr;
    serveraddr.sin_port = htons((u_short)id_server_port);
    
    return serveraddr;
}
//Sends a request to the client asking for a new server list
int RequestServerList(int fd, struct sockaddr_in serveraddr)
{
    int addrlen;
    char *msg = "GET_SERVERS";
    
    //Get the lenght of the address
    addrlen = sizeof(serveraddr);
    
    //Send the message
    sendto(fd, msg, strlen(msg)+1, 0, (struct sockaddr*)&serveraddr,addrlen);

    
    return 1;
}

//Receives the buffer from the server
char* ReceiveServerList(int fd, struct sockaddr_in serveraddr)
{
    int addrlen, ret;
    
    char buffer[1024];
    //Dynamically allocate a string
    char * ret_string = (char*)malloc(sizeof(char)*1024);

    //Get the lenght of the address
    addrlen = sizeof(serveraddr);
    
    ret = recvfrom(fd, buffer, sizeof(buffer),0, (struct sockaddr*)&serveraddr,&addrlen);
    
    if(ret == -1)
    {
        printf("Error receiving data from server\n");
        perror("");
        return NULL;
    }
    else
    {
        //Copy the contents from the server to the allocated strings
        strncpy(ret_string, buffer, ret);
        
        //Make sure the string is ended
        ret_string[ret] = '\0';
    }
    
    return ret_string;
}

//Receives the buffer from the server
char* ReceiveMessagesFromServer(int fd, ServerListNode * head)
{
    int addrlen, ret;
    struct sockaddr_in serveraddr;
    
    char buffer[1024];
    //Dynamically allocate a string
    char * ret_string = (char*)malloc(sizeof(char)*1024);
    
    memset((void*)&serveraddr,(int) '\0', sizeof(serveraddr));
    
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(GetUDPIPFromNode(head));
    serveraddr.sin_port = htons((u_short)GetUDPPortFromNode(head));
    
    addrlen = sizeof(serveraddr);
    
    ret = recvfrom(fd, buffer, sizeof(buffer),0, (struct sockaddr*)&serveraddr,&addrlen);
    
    if(ret == -1)
    {
        printf("Error receiving data from server\n");
        perror("");
        return NULL;
    }
    
    
    //Copy the contents from the server to the allocated strings
    strncpy(ret_string, buffer, ret);
    
    //Make sure the string is ended
    ret_string[ret] = '\0';
    
    return ret_string;
}
