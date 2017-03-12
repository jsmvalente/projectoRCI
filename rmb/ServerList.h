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
int CreateServerList(ServerListNode *head, char* server_list);
int GetUDPPortFromNode(ServerListNode *node);
char* GetUDPIPFromNode(ServerListNode *node);
void FreeServerList(ServerListNode *head);
void ChangeDefaultServer(ServerListNode *head);
