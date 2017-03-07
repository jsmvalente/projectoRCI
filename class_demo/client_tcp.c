#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define PORT 58000
#define BUFFSIZE 1024

int main(void)
{
	int fd, newfd, n;
	struct hostent *hostptr;
	struct sockaddr_in serveraddr, clientadrr;
	int clientlen;
	char buf[BUFFSIZE];

	fd = socket(AF_INET, SOCK_STREAM,0);
	if(fd == -1)
	{
		printf("Tejo");
		exit(1);
	}

	hostptr=gethostbyname("tejo");

	memset((void*)&serveraddr, (int)'\0', sizeof(serveraddr));

	serveraddr.sin_family=AF_INET;
	serveraddr.sin_addr.s_addr=((struct in_addr *) (hostptr->h_addr_list[0]))->s_addr;
	serveraddr.sin_port=htons((u_short) PORT);

	n = connect(fd, (struct sockaddr*) &serveraddr, sizeof(serveraddr));
	if(n == -1)
	{
		printf("A tua mãe");
		exit(1);
	}

  /* get message line from the user */
    printf("Please enter msg: ");
    bzero(buf, BUFFSIZE);
    fgets(buf, BUFFSIZE, stdin);

    /* send the message line to the server */
    n = write(fd, buf, strlen(buf));
    if (n < 0) 
      error("ERROR writing to socket");

    /* print the server's reply */
    bzero(buf, BUFFSIZE);
    n = read(fd, buf, BUFFSIZE);
    if (n < 0) 
      error("ERROR reading from socket");
    printf("Echo from server: %s", buf);
   
    close(fd);
    return 0;
}
