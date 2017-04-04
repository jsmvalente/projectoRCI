#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#include "user_functions.h"
#include "message_functions.h"

#define max(A,B) ((A)>=(B)?(A):(B))
#define PORT  59000
#define MAXCHAR 1024

int main(int argc, char *argv[])
{
  //Declaration of the input variables.
  int udp_port, tcp_port, n_messages, int_reg, id_serverport;
  char *server_name, *ip_address, *id_serverip;

  //ID Server variables.
  int fd_id, addrlen;
  struct sockaddr_in serveraddr;
  struct hostent *hostptr;

  //UDP Server variables.
  int fd_client, clientaddrlen;
  struct sockaddr_in myserveraddr, clientaddr;

  //TCP Server variables.
  int fd_tcp, tcpclientlen, *newsockfd, connect_count=0;
  struct sockaddr_in tcpserveraddr, tcpclientaddr;

  //TCP client variables.
  int *fd, nread;
  char *readbuffer = NULL;
  SERVER *servlist;

  //Other variables.
  int n, i, select_ret_val, maxfd, id_socket=0, print_message=0, logic_timer=0, ret, t, n_wanted_msg, message_index=0, num_servers, createserver_flag=0, logic_timer_start=0, default_server, retwrite, num_connected, set_filed =0;
  char command[MAXCHAR], reg_message[MAXCHAR], buffer[MAXCHAR], buffer_client[MAXCHAR], trash[MAXCHAR], published_msg[MAXCHAR];

  //Test variables.
  int sizebuffer;

  //Declaration of timer concern variables.
  struct timespec before, now;
  struct timeval timer;

  //Declaration of variable for Select().
  fd_set rfds;

  //Declaration of the message array.
  MESSAGE *msg;

  //*****************************************************************************
  //*****************************************************************************
  //*****************************************************************************
  //End of variable declaration.
  //*****************************************************************************
  //*****************************************************************************
  //*****************************************************************************

  //Initialize random seed.
  srand(time(0));

  clock_gettime(CLOCK_REALTIME, &before);

  //Collects the arguments to the corresponding variable.
  get_arguments(argc, argv, &server_name, &ip_address, &udp_port, &tcp_port, &id_serverip, &id_serverport, &n_messages, &int_reg);

  //Sets the timer.
  timer.tv_sec = int_reg;

  //Declares de size of the message array in accordan to the maximum number of variables.
  msg = (MESSAGE*) malloc((sizeof(MESSAGE) * n_messages));
  for(i = 0; i<n_messages; i++)
  {
    msg[i].text = NULL;
  }

  //Registration message.
  strcpy(reg_message, "REG ");
  strcat(reg_message, server_name);
  strcat(reg_message, ";");
  strcat(reg_message, ip_address);
  strcat(reg_message, ";");
  sprintf(buffer, "%d", udp_port);
  strcat(reg_message, buffer);
  strcat(reg_message, ";");
  sprintf(buffer, "%d", tcp_port);
  strcat(reg_message, buffer);

  //File descriptor UDP to ID server.
  fd_id = socket(AF_INET, SOCK_DGRAM,0);
  if(fd_id == -1)
  {
    exit(-1);
  }

  if((hostptr = gethostbyname("tejo.tecnico.ulisboa.pt")) == NULL)
  {
    //Error
    exit(1);
  }

  //Initializing Socket structure.
  memset((void*)&serveraddr,(int) '\0', sizeof(serveraddr));

  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = ((struct in_addr *)(hostptr->h_addr_list[0]))->s_addr;
  serveraddr.sin_port = htons((u_short)id_serverport);

  //File descriptor UDP server.
  fd_client = socket(AF_INET, SOCK_DGRAM, 0);

  if(fd_client == -1)
  {
    exit(-1);
  }

  //Initializing Sockect structure.
  memset((void*)&myserveraddr,(int)'\0', sizeof(myserveraddr));

  myserveraddr.sin_family = AF_INET;
  myserveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  myserveraddr.sin_port = htons((u_short)udp_port);

  ret = bind(fd_client, (struct sockaddr*) &myserveraddr, sizeof(myserveraddr));

  if(ret == -1)
  {
    //error.
    printf("%s\n", strerror(errno));
    exit(1);
  }

  //File Descriptor for TCP.
  fd_tcp = socket(AF_INET, SOCK_STREAM, 0);

  if(fd_tcp == -1)
  {
    exit(-1);
  }

  //Initializing Socket structure.
  memset((void*)&tcpserveraddr,(int) '\0', sizeof(tcpserveraddr));

  myserveraddr.sin_family = AF_INET;
  myserveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  myserveraddr.sin_port = htons((u_short)tcp_port);

  //Binding the tcp server address.
  if( bind(fd_tcp, (struct sockaddr*) &tcpserveraddr, sizeof(tcpserveraddr)) <0)
  {
    //Error.
    exit(1);
  }

  //Listening for clients.
  if(listen(fd_tcp, 5)<0)
  {
    //Error.
    exit(1);
  }

  newsockfd = (int *)malloc(sizeof(int));
  tcpclientlen = sizeof(tcpclientaddr);

  //*****************************************************************************
  //*****************************************************************************
  //*****************************************************************************
  //Priting some information to use the program. Starting the application itself.
  //*****************************************************************************
  //*****************************************************************************
  //*****************************************************************************

  printf("MSGSERVER\nPor favor, escolha uma das seguintes operações e pressione 'enter':\n*'join'\n*'show_servers'\n*'show_messages'\n*'exit'\n");

  while(1)
  {
    clock_gettime(CLOCK_REALTIME, &now);
    t = now.tv_sec - before.tv_sec;

    //Send the registration message to the id server.
    if(t == int_reg && id_socket ==1 && print_message == 0)
    {
      clock_gettime(CLOCK_REALTIME, &before);

      addrlen = sizeof(serveraddr);
      sendto(fd_id, reg_message, strlen(reg_message)+1, 0, (struct sockaddr*) &serveraddr, addrlen);
    }

    //Clears sets of watched file descriptors
    FD_ZERO(&rfds);

    //Watch stdin to see when it has input.
    FD_SET(0, &rfds);
    maxfd = 0;

    if(id_socket == 1)
    {
      FD_SET(fd_id, &rfds);
      maxfd = max(maxfd, fd_id);

      FD_SET(fd_client, &rfds);
      maxfd = max(maxfd, fd_client);

      FD_SET(fd_tcp, &rfds);
      maxfd = max(maxfd, fd_tcp);
    }

    if(set_filed == 1)
    {
      for(i=0 ; i<num_servers ; i++)
      {
        FD_SET(fd[i], &rfds);
        maxfd = max(maxfd, fd[i]);
      }
    }

    //Run select and get it's return value
    select_ret_val = select(maxfd + 1, &rfds, (fd_set*)NULL, (fd_set*)NULL, &timer);

    //Check select return value for errors
    if (select_ret_val == -1)
    {
      printf("Error on select()\n");
      exit(1);
    }

    //Find out the fd we are treating
    //We are treating user input
    if (FD_ISSET(0, &rfds))
    {
      //Reads the user input.
      fgets(command, MAXCHAR, stdin);

      //Identifies the user command.
      n = identify_user_commands(command);

      //Join.
      if(n==1)
      {
        addrlen = sizeof(serveraddr);
        //Send the registration message to the id server.
        sendto(fd_id, reg_message, strlen(reg_message)+1, 0, (struct sockaddr*) &serveraddr, addrlen);

        //Send the "GET_SERVERS" message to the id server.
        sendto(fd_id, "GET_SERVERS", strlen("GET_SERVERS")+1, 0, (struct sockaddr*) &serveraddr, addrlen);

        //It can now receive information from the server.
        id_socket = 1;
        //Blocks printing on the screen.
        print_message = 0;
        //Creation of the server flag.
        createserver_flag = 1;
      }

      //show_servers.
      if(n==2)
      {
        addrlen = sizeof(serveraddr);
        //Send the "GET_SERVERS" message to the id server.
        sendto(fd_id, "GET_SERVERS", strlen("GET_SERVERS")+1, 0, (struct sockaddr*) &serveraddr, addrlen);
        //It can now receive information from the server.
        id_socket = 1;
        //Prints on the screen.
        print_message = 1;
      }

      //Exit.
      if(n==4)
      {
        //Closes the program.
        break;
      }
    }

    if (FD_ISSET(fd_id, &rfds))
    {
      addrlen = sizeof(serveraddr);
      recvfrom(fd_id, buffer, sizeof(buffer), 0, (struct sockaddr*)&serveraddr, &addrlen);

      //TCP servers connections and fd.
      if(createserver_flag == 1)
      {
        num_servers = server_count(buffer);

        //Alocates the memory of the server list.
        servlist = (SERVER *)malloc(num_servers*sizeof(SERVER));

        //Allocates the memory for the File Descriptors array for each server to connect.
        fd = (int *) malloc(sizeof(int)*num_servers);

        //Inserts the information respective to each server.
        insert_server(servlist, buffer, server_name);

        //Connects to each TCP server.
        connect_server(servlist, fd, num_servers);

        //Counts the successful connects.
        count_connected(servlist, &num_connected, num_servers);

        //Adds each new fd to select().
        for(i=0 ; i<num_servers; i++)
        {
          FD_SET(fd[i], &rfds);
          maxfd = max(maxfd, fd[i]);
        }

        if(num_servers != 0)
        {
          //Get random number between 0 and num_servers - 1).
          default_server = rand() % num_servers;

          if(num_connected != 0)
          {
            //Until it finds a connected server.
            while(servlist[default_server].connect == 0)
            {
              default_server = rand() % num_servers;
            }

            retwrite = write(fd[default_server], "SGET_MESSAGES\n", MAXCHAR);
          }

          logic_timer_start = 1;
        }

        set_filed = 1;
        createserver_flag = 0;
      }

      //Controls printing the list of servers on screen.
      if(print_message == 1)
      {
        printf("%s\n", buffer);
        print_message = 0;
      }
    }

    if (FD_ISSET(fd_client, &rfds))
    {
      clientaddrlen = sizeof(clientaddr);
      recvfrom(fd_client, buffer_client, sizeof(buffer_client), 0, (struct sockaddr*)&clientaddr, &clientaddrlen);
      printf("%s\n", buffer_client);

      if(strncmp(buffer_client, "GET_MESSAGES", 12) == 0)
      {
        sscanf(buffer_client, "%s %d", trash, &n_wanted_msg);

        clientmessage(buffer_client, msg, n_wanted_msg, message_index,logic_timer);

        clientaddrlen = sizeof(clientaddr);
        sendto(fd_client, buffer_client, strlen(buffer_client)+1, 0, (struct sockaddr*) &clientaddr, clientaddrlen);
      }
      else if(strncmp(buffer_client, "PUBLISH", 7) == 0)
      {
        insertmessage(buffer_client, msg, &logic_timer, &message_index);


        //Builds the message to deliver to the other servers.
        strcpy(buffer_client, "SMESSAGES\n");
        sprintf(trash, "%d", msg[message_index-1].time_message);
        strcat(buffer_client, trash);
        strcat(buffer_client, ";");
        strcat(buffer_client, msg[message_index-1].text);

        //Sends the received message to the other servers.
        for(i=0 ; i<num_servers ; i++)
        {
          write(fd[i], buffer_client, MAXCHAR);
        }
        for(i=0 ; i ; i++)
        {
          write(newsockfd[i], buffer_client, MAXCHAR);
        }
      }
    }

    if(FD_ISSET(fd_tcp, &rfds))
    {
      if(connect_count == 0)
      {
        newsockfd = (int*)malloc(sizeof(int));
      }
      else
      {
        newsockfd = (int*)realloc(newsockfd, sizeof(int)+sizeof(int)*connect_count);
      }

      newsockfd[connect_count] = accept(fd_tcp, (struct sockaddr*) &tcpclientaddr, &tcpclientlen);

      if(newsockfd <0)
      {
        //Error.
        exit(1);
      }

      connect_count++;
    }

    if(connect_count != 0)
    {
    for(i=0 ; i<connect_count ; i++)
    {
      if(FD_ISSET(newsockfd[i], &rfds))
      {
        nread = read(fd[i], readbuffer, 0);
        if(nread == -1)
        {
          printf("Error\n");
          exit(1);
        }

        printf("%s\n", readbuffer);

        //Puts each message in the message array.
        if(strncmp(readbuffer, "SGET_MESSAGES", 12) == 0)
        {
          tcpmessage(readbuffer, msg, message_index, logic_timer);
          write(fd[i], readbuffer, MAXCHAR);
        }
        else if(strncmp(readbuffer, "SMESSAGES", 9) == 0)
        {
          receivemessages(msg, readbuffer, &message_index);
          logic_timer = max(logic_timer, msg[message_index-1].time_message) + 1;
        }
      }
    }
    }

    for(i=0 ; i<num_servers ; i++)
    {
      if(FD_ISSET(fd[i], &rfds) && num_connected != 0)
      {
        nread = read(fd[i], readbuffer, MAXCHAR);
        if(nread == -1)
        {
          printf("Error\n");
          exit(1);
        }

        //Puts each message in the message array.
        if(strncmp(readbuffer, "SMESSAGES", 9) == 0)
        {
          receivemessages(msg, readbuffer, &message_index);

          if(message_index == 0)
          {
            printf("No messages fetched!\n");
          }
          else
          {
            if(logic_timer_start == 1)
            {
              logic_timer = msg[message_index-1].time_message;

              logic_timer_start = 0;
            }
            else
            {
              logic_timer = max(logic_timer, msg[message_index-1].time_message) + 1;
            }
          }
        }
      }
    }
  }

  return 0;
}
