//
//  client.c
//  projectoRCI
//
//  Created by João Valente on 07/03/2017.
//  Copyright © 2017 João Valente. All rights reserved.
//


#include "Defs.h"

#define BUFFERSIZE 1024
#define MAXIMUM_MESSAGE_SIZE 140
#define NUMBER_OF_SERVER_TRIES 2

int main(int argc, char *argv[]) {
    
    fd_set rfds;
    struct timeval tv;
    int select_ret_val, n, id_fd = -1, msg_fd = -1, waiting_messages = -1;
    struct hostent *hostptr;
    char input_buffer[BUFFERSIZE], *message, *n_string, *server_list_buffer, *messages_list;
    ServerListNode * server_list_head = NULL;
    struct sockaddr_in id_serveraddr;
    
    //IP Address
    char *siip = "undefined";
    
    //Default IP Port
    int sipt = 59000;
    
    //Check if the user used the right number of arguments
    if(argc > 5)
    {
        printf("Too many arguments. rmb should be started with 'rmb [-i siip] [-p sipt]'");
        
        return 0;
    }
    //If the user used any arguments
    else if(argc != 1)
    {
        //If the user only uses one argument
        if (argc == 3 && (strncmp(argv[1], "-i", 2) == 0 || strncmp(argv[3], "-p", 2) == 0))
        {
            if(strncmp(argv[3], "-p", 2))
            {
                //Get a port number from the arguments
                sipt = atoi(argv[2]);
                
            }
            else if(strncmp(argv[3], "-i", 2))
            {
                //Get an IPv4 from the arguments
                siip = argv[4];
            }
        }
        //If the user uses two arguments
        else if(argc == 5 && strncmp(argv[1], "-i", 2) == 0 && strncmp(argv[3], "-i", 2))
        {
            //Get a port number from the arguments
            sipt = atoi(argv[4]);
            
            //Get an IPv4 from the arguments
            siip = argv[2];
        }
        else
        {
            printf("Argument error: rmb should be started with 'rmb [-i siip] [-p sipt]'");
            
            return 0;
        }
    }
    
    //If the user did not define an ID server we need to define it ourselves
    if(strcmp(siip, "undefined") == 0)
    {
        if((hostptr = gethostbyname("tejo.tecnico.ulisboa.pt")) == NULL)
        {
            printf("Error getting the default ID server. Exiting...\n");
            exit(1);
        }
        else
        {
            siip = hostptr->h_addr_list[0];
        }
    }
    
    id_serveraddr = SetupIDServerAddress(siip, sipt);
    
    //Create a new socket to send a show_servers request from
    if((id_fd = CreateSocket()) == -1)
    {
        printf("Unable to create socket. Exiting...");
        exit(0);
    }
    
    //Create a new socket to send the msgserv requests
    if((msg_fd = CreateSocket()) == -1)
    {
        printf("Unable to create socket. Exiting...");
        exit(0);
    }
    
    
    //Make an initial request to the server to get the server list
    if(RequestServerList(id_fd, id_serveraddr) == -1)
    {
        printf("Unable to fetch the server list from the identities server.");
    }
    
    //Enter infinite loop
    while(1)
    {
        //Wait up to five seconds.
        tv.tv_sec = 5;
        tv.tv_usec = 0;
        
        //Clears sets of watched file descriptors
        FD_ZERO(&rfds);
        
        //Watch stdin to see when it has input.
        FD_SET(0, &rfds);
        
        //Wtatch the id socket
        FD_SET(id_fd, &rfds);
        
        //Wtatch the msg socket
        FD_SET(msg_fd, &rfds);
        
        //Run select and get it's return value
        select_ret_val = select(msg_fd + 1, &rfds, NULL, NULL, &tv);
        
        //Check select return value for errors
        if (select_ret_val == -1)
        {
            printf("Error on select()\n");
            exit(1);
        }
        //We have data from the file descriptor and can treat it
        else if(select_ret_val)
        {
            /*Find out the fd we are treating*/
            
            //We are treating user input
            if (FD_ISSET(0, &rfds))
            {
                //Check for user input errors
                if(fgets(input_buffer, BUFFERSIZE, stdin) != NULL)
                {
                    /*Check if user input is valid*/
                    
                    //User wants to list the available servers
                    if(strncmp(input_buffer, "show_servers\n", 13) == 0)
                    {
                        //Make a request to the server to refresh the server list
                        if(RequestServerList(id_fd, id_serveraddr) == -1)
                        {
                            printf("Unable to fetch the server from the identities server, can't show server right now.\n");
                        }
                    }
                    //User wants to send a message
                    else if(strncmp(input_buffer, "publish ", 8) == 0)
                    {
                        //If the message is just a paragraph we don't send it
                        if(strncmp(&input_buffer[8], "\n", 1) != 0)
                        {
                            //We want to get the message from the buffer, we don't need to check the  lenght because it wouldn't get here otherwise
                            message = input_buffer + 8;
                            
                            //Check if the user message lenght does not exceed MAXIMUM_MESSAGE_SIZE
                            if (strlen(message) < MAXIMUM_MESSAGE_SIZE - 1)
                            {
                                //Check if there are message servers available
                                if(server_list_head != NULL)
                                {
                                    printf("Relaying your message: %s\n", message);
                                    
                                    if(SendMessageToServer(msg_fd, server_list_head, message) == -1)
                                    {
                                        printf("Error sending message\n");
                                    }
                                    else
                                    {
                                        printf("Message sent successfuly: %s\n", message);
                                    }
                                }
                                else
                                {
                                    printf("No message servers available. Type 'show_servers' to update the server list\n");
                                }
                            }
                            else
                            {
                                printf("Number of characters exceeded, messages can only have 140 characters\n");
                            }
                        }
                        else
                        {
                            printf("Use 'publish message' to send a message\n");
                        }
                    }
                    //User wants to show the n latest messages
                    else if(strncmp(input_buffer, "show_latest_messages ", 21) == 0)
                    {
                        //If the message is just a paragraph we don't send it
                        if(strncmp(&input_buffer[21], "\n", 1) != 0)
                        {
                            //We want to get the message from the buffer, we don't need to check the  lenght because it wouldn't get here otherwise
                            n_string = input_buffer + 21;
                            
                            //Get the n latest messages int from the string that was in the buffer
                            n = atoi(n_string);
                            
                            if(n > 0)
                            {
                                //Check if there are message servers available
                                if(server_list_head != NULL)
                                {
                                    printf("Getting the last %d messages\n", n);
                                    
                                    if(RequestMessagesFromServer(msg_fd, server_list_head, n) == -1)
                                    {
                                        printf("Error getting messages\n");
                                    }
                                    else
                                    {
                                        //Made the first try to contact the server
                                        waiting_messages = 1;
                                    }
                                }
                                else
                                {
                                    printf("No message servers available. Type 'show_servers' to update the server list\n");
                                }
                            }
                            else
                            {
                                 printf("'show_latest_messages n', n needs to be bigger than 0.\n");
                            }
                        }
                        else
                        {
                            printf("Use 'show_latest_messages n' to show the latest n messages\n");
                        }
                    }
                    //User wants to exit the program
                    else if(strncmp(input_buffer, "exit\n", 5) == 0)
                    {
                        printf("Closing connections and exiting...\n");
                        
                        FreeServerList(server_list_head);
                        
                        //Close sockets
                        close(id_fd);
                        close(msg_fd);
                    }
                    else
                    {
                        printf("Invalid command\n");
                    }
                }
                else
                {
                    printf("Input error\n");
                }
                
            }
            //If there is new info arriving in the id socket
            else if (FD_ISSET(id_fd, &rfds))
            {
                //Reset the waiting for new messages flag
                waiting_messages = -1;
                
                //Receive the buffer from the server
                if((server_list_buffer = ReceiveServerList(id_fd, id_serveraddr)) != NULL)
                {
                    printf("%s\n", server_list_buffer);
                    
                    //Frees the old server list
                    FreeServerList(server_list_head);
                    
                    //Create a new server list with the information we got from the identity server
                    server_list_head = CreateServerList(server_list_buffer);
                
                    free(server_list_buffer);
                }
                else
                {
                    printf("Error receiving data from server\n");
                }

            }
            //If there is new info arriving in the msg socket
            else if (FD_ISSET(msg_fd, &rfds))
            {
                //Receive the buffer from the server
                messages_list = ReceiveMessagesFromServer(msg_fd, server_list_head);
                
                //Test for errors
                if(messages_list != NULL)
                {
                    //The message list is empty
                    if(strcmp(messages_list, "MESSAGES\n") == 0)
                    {
                        printf("Server has no messages to show\n");
                    }
                    else
                    {
                        printf("%s\n", messages_list);
                    }
                    
                    free(messages_list);
                    
                    //Reset the waiting_messages parameter
                    waiting_messages = -1;
                }
                else
                {
                    printf("Error receiving data from server\n");
                }
                
            }
        }
        //Waited too much time
        else
        {
            //We are waiting fpr the swever to send us the last n messages
            if(waiting_messages != -1)
            {
                //We only change message server after trying to contact the server twice
                if(waiting_messages == NUMBER_OF_SERVER_TRIES)
                {
                    server_list_head = ChangeDefaultServer(server_list_head);
                    
                    //Reset the number of attempts
                    waiting_messages = 0;
                    
                    if(GetServerNameFromNode(server_list_head) != NULL)
                    {
                        printf("Server not responding, changing messages server to %s\n", GetServerNameFromNode(server_list_head));
                    }
                }
                
                //Try to get the messages again the the new default server if we haven't runned out of servers
                if(server_list_head != NULL)
                {
                    if(RequestMessagesFromServer(msg_fd, server_list_head, n) == -1)
                    {
                        printf("Error getting messages\n");
                    }
                    else
                    {
                        if(waiting_messages > 0)
                            printf("No answer, contacting server again...\n");
                        
                        //Increase the number of attempts
                        waiting_messages++;
                    }
                }
                //Resest the waiting_messages flag
                else
                {
                    printf("No message servers available. Type 'show_servers' to update the server list\n");
                    
                    waiting_messages = -1;
                }
            }
        }
    }
    
    return 0;
}
