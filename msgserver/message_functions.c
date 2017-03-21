#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "message_functions.h"

#define MAXCHAR 1024

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
