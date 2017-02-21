#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#define PORT 58000

int main(void)
{
	int fd = 0, ret = 0, addrlen, nread = 0;
	struct hostent *hostptr;
	struct sockaddr_in serveraddr, clientaddr;
	char msg[80], buffer[80];

	//UDP Socket
	fd = socket(AF_INET, SOCK_DGRAM,0);

	if(fd == -1)
	{
		exit(-1);
	}

	memset((void*)&serveraddr,(int)'\0', sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons((u_short)PORT);
	
	ret = bind(fd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));

	if(ret == -1)
	{
		exit(1);
	}

	while(1)
	{
		addrlen = sizeof(clientaddr);
		nread = recvfrom(fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientaddr, &addrlen);

		if(nread == -1)
		{
			exit(1);
		}

		printf("working: %s", buffer);
		
		ret = sendto(fd, buffer, nread, 0, (struct sockaddr*)&clientaddr, addrlen);

		if(ret == -1)
		{
			exit(1);
		}
	}
	
	close(fd);
	exit(0);
}
