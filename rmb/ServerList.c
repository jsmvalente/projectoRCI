//
//  ServerList.c
//  projectoRCI
//
//  Created by João Valente on 08/03/2017.
//  Copyright © 2017 João Valente. All rights reserved.
//

#include "Defs.h"

struct serverlistnode
{
    //Server Name
    char *name;
    
    //Server IP
    char *ip;
    
    //Server UDP port
    int upt;
    
    //Server TCP port
    int tpt;
    
    struct serverlistnode * next;
};

int CheckIfServerListIsEmpty(ServerListNode *head)
{
    if (head == NULL)
    {
        
        return -1;
    }
    
    return 1;

}

//Returns the port number of a certaing node
int GetUDPPortFromNode(ServerListNode *node)
{
    return node->upt;
}

//Returns the address of a certaing node
char* GetUDPIPFromNode(ServerListNode *node)
{
    return node->ip;
}

//Add a new node to the serverList
ServerListNode* AddNodeToListHead(ServerListNode *head, char *name, char *ip, int upt, int tpt)
{
    //Create the new node
    ServerListNode * new = (ServerListNode *)malloc(sizeof(ServerListNode));
    VerifyMalloc( (void *) new );
    
    //Add the info anout the server
    new->name = name;
    new->ip = ip;
    new->tpt = tpt;
    new ->upt = upt;
    
    //Point the list head to this node and point the next to the old head
    new->next = head;
    head = new;
    
    return head;
}

//Puts a random server into the head of the list, that will be the default server
void ChooseActiveServerRandomly(ServerListNode *head, int number_servers)
{
    int default_server, i;
    ServerListNode * aux_prev = NULL, *aux=head;
    
    //Initialize random seed
    srand(time(0));
    
    //Get random number between 0 and number_servers - 1
    default_server = number_servers % rand();
    
    if(default_server == 0)
        return;
    else
    {
        for(i = 0; i == default_server; i++)
        {
            aux_prev = aux;
            aux = aux->next;
        }
        
        //Change the chosen server to the head of the list
        aux_prev->next = aux->next;
        aux->next = head;
        head = aux;
    }
}

//Create a new server list we information from the server
int CreateServerList(ServerListNode *head, char* server_list)
{
    //The server list string will come in the following format: 'SERVERS\n(name;ip;upt;tpt\n)*\n'
    
    char *servers_to_add, *name, *ip, *upt, *tpt;
    int counter = 0;
    int servers = 0;
    
    //If there are are servers to add
    if(server_list[8] != '\0')
    {
        //Get a new pointer to the beggining of the server list
        servers_to_add = server_list + 8;
        
        //While we still have servers to add to the list
        while (strlen(servers_to_add) > 0)
        {
            //Increase the number of servers
            servers += 1;
            
            //Get the size of the name
            while(servers_to_add[counter] != ';')
            {
                counter += 1;
            }
            
            //Get memory space for new name string
            name = (char*)malloc(counter+1);
            
            //Copy string
            strncpy(name, servers_to_add, counter);
            
            //Place the null terminator
            name[counter] = '\0';
            
            //Move the servers_to_add pointer forward
            servers_to_add += counter + 1;
            
            //Reset the counter
            counter = 0;
            
            //Get the size of the ip
            while(servers_to_add[counter] != ';')
            {
                counter += 1;
            }
            
            //Get memory space for ip string
            ip = (char*)malloc(counter+1);
            
            //Copy string
            strncpy(ip, servers_to_add, counter);
            
            //Place the null terminator
            ip[counter] = '\0';
            
            //Move the servers_to_add pointer forward
            servers_to_add += counter + 1;
            
            //Reset the counter
            counter = 0;
            
            //Get the size of the upt
            while(servers_to_add[counter] != ';')
            {
                counter += 1;
            }
            
            //Get memory space for upt string
            upt = (char*)malloc(counter+1);
            
            //Copy string
            strncpy(upt, servers_to_add, counter);
            
            //Place the null terminator
            upt[counter] = '\0';
            
            //Move the servers_to_add pointer forward
            servers_to_add += counter + 1;
            
            //Reset the counter
            counter = 0;
            
            //Get the size of the tpt
            while(servers_to_add[counter] != '\n')
            {
                counter += 1;
            }
            
            //Get memory space for upt string
            tpt = (char*)malloc(counter+1);
            
            //Copy string
            strncpy(tpt, servers_to_add, counter);
            
            //Place the null terminator
            tpt[counter] = '\0';
            
            //Move the servers_to_add pointer forward
            servers_to_add += counter + 1;
            
            //Reset the counter
            counter = 0;
            
            //Add the node to the server list
            AddNodeToListHead(head, name, ip, atoi(upt), atoi(tpt));
        }
        
        ChooseActiveServerRandomly(head, servers);
    }
    else
    {
        head = NULL;
    }
    
    return 1;
}

//Frees a server list
void FreeServerList(ServerListNode *head)
{
    ServerListNode * next;
    ServerListNode * aux;
    
    //Cycle from the first to the last element
    for(aux = head; aux != NULL; aux = next)
    {
        //Keep trace of the next node
        next = aux->next;
        
        //Free strings
        free(aux->name);
        free(aux->ip);
        
        //Free current node
        free(aux);
    }
    
    return;
}

//Changes default server by deleting the head node
void ChangeDefaultServer(ServerListNode *head)
{
    ServerListNode* aux = head;
    
    //Change the head of the list
    head = head->next;
    
    //Free the old list head
    free(aux->name);
    free(aux->ip);
    
    free(aux);

    return;
}
