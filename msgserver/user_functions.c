#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "user_functions.h"

int identify_user_commands(char *command)
{
  int n=0;

  if(!strcmp(command, "join\n"))
  {
    n=1;
  }
  else if(!strcmp(command, "show_servers\n"))
  {
    n=2;
  }
  else if(!strcmp(command, "show_messages\n"))
  {
    n=3;
   }
  else if(!strcmp(command, "exit\n"))
  {
    n=4;
  }
  else
  {
    n = 5;
    printf("Command not valid!\n");
    exit(0);
  }

  return n;
}

void get_arguments(int argc, char *argv[], char **server_name, char **ip_address, char **udp_port, char **tcp_port, char **id_serverip, char **id_serverport, int *n_messages, int *int_reg)
{
int opt, n;
int flag_count=0;

/*
if(argc>17)
{
  printf("Something is wrog!\n");
  exit(0);
}
*/

//Default values.
*id_serverip = "tejo.tecnico.ulisboa.pt";
*id_serverport = "59000";
*n_messages = 200;
*int_reg =  10;

//Searches for flags and sets the argument of each flag to the corresponding variable.
while((opt = getopt(argc, argv, "n:j:u:t:i:p:m:r:"))!= -1)
{
  switch(opt)
  {
    case 'n':
    {
      *server_name = malloc(strlen(argv[2])+1);
      *server_name = argv[2];
      flag_count++;
    }
    break;
    case 'j':
    {
      *ip_address = malloc(strlen(optarg)+1);
      *ip_address = optarg;
      flag_count++;
    }
    break;
    case 'u':
    {
      *udp_port = malloc(strlen(optarg)+1);
      *udp_port = optarg;
      flag_count++;
    }
    break;
    case 't':
    {
      *tcp_port = malloc(strlen(optarg)+1);
      *tcp_port = optarg;
      flag_count++;
    }
    break;
    case 'i':
    {
      if(flag_count != 4)
      {
        printf("Input Missing!\n");
        exit(0);
      }
      *id_serverip = malloc(strlen(optarg)+1);*server_name = argv[2];
      *id_serverip = optarg;
    }
    break;
    case 'p':
    {
      if(flag_count != 4)
      {
        printf("Input Missing!\n");
        exit(0);
      }
      *id_serverport = malloc(strlen(optarg)+1);
      *id_serverport = optarg;
    }
    break;
    case 'm':
    {
      if(flag_count != 4)
      {
        printf("Input Missing!\n");
        exit(0);
      }
      *n_messages = atoi(optarg);
    }
    break;
    case 'r':
    {
      if(flag_count != 4)
      {
        printf("Input Missing!\n");
        exit(0);
      }
      *int_reg = atoi(optarg);
    }
    break;
  }
}
}
