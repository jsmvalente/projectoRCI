//
//  ServerList.h
//  projectoRCI
//
//  Created by João Valente on 08/03/2017.
//  Copyright © 2017 João Valente. All rights reserved.
//

#ifndef ServerList_h
#define ServerList_h

#include "Defs.h"

#endif /* ServerList_h */

typedef struct serverlistnode ServerListNode;
ServerListNode* AddNodeToListHead(ServerListNode *head, char *name, char *ip, int upt, int tpt);
ServerListNode * CreateServerList(char* server_list);
int GetUDPPortFromNode(ServerListNode *node);
char* GetUDPIPFromNode(ServerListNode *node);
char* GetServerNameFromNode(ServerListNode *node);
void FreeServerList(ServerListNode *head);
ServerListNode * ChangeDefaultServer(ServerListNode *head);
