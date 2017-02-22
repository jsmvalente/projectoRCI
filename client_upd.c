#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#define PORT 58000

int main(void)
{
	int fd = 0, ret = 0, addrlen, nread = 0;
	struct hostent *hostptr;
	struct sockaddr_in serveraddr, clientaddr;
	char msg[80], buffer[80];

	printf("What's the message you want to send?\n");
	scanf("%s", msg);

	//UDP Socket
	fd = socket(AF_INET, SOCK_DGRAM,0);

	if(fd == -1)
	{
		exit(-1);
	}

	if((hostptr = gethostbyname("tejo.tecnico.ulisboa.pt")) == NULL)
	{
		//Error
		exit(1);
	}

	memset((void*)&serveraddr,(int) '\0', sizeof(serveraddr));

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = ((struct in_addr *)(hostptr->h_addr_list[0]))->s_addr;
	serveraddr.sin_port = htons((u_short)PORT);

	addrlen = sizeof(serveraddr);

	sendto(fd, msg, strlen(msg)+1, 0, (struct sockaddr*)&serveraddr,addrlen);

	recvfrom(fd, buffer, sizeof(buffer),0, (struct sockaddr*)&serveraddr,&addrlen);

	printf("They sent: %s\n", buffer);

	close (fd);

	exit(0);
}
