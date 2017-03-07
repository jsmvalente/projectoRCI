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
	int fd, newfd, n, ret, addrlen, nw;
	struct hostent *hostptr;
	struct sockaddr_in serveraddr;
	char buf[BUFFSIZE], *ptr;

	fd = socket(AF_INET, SOCK_STREAM,0);
	if(fd == -1)
	{
		exit(1);
	}

	memset((void*)&serveraddr, (int)'\0', sizeof(serveraddr));

	serveraddr.sin_family=AF_INET;
	serveraddr.sin_addr.s_addr=htonl(INADDR_ANY);
	serveraddr.sin_port=htons((u_short) PORT);

	ret = bind(fd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	
	if(ret == -1)
	{
		exit(1);
	}

	if(listen(fd, 5)==-1)
	{
		exit(1);
	}

	while(1)
	{
		addrlen = sizeof(serveraddr);

		if(newfd=accept(fd, (struct sockaddr*)&serveraddr, &addrlen)==-1)
		{
			exit(1);
		}

		while((n=read(newfd, buf, BUFFSIZE))!=0)
		{
			if(n==-1)
			{
				exit(1);
			}

			ptr = &buf[0];

			while(n>0)
			{
				if((nw=write(newfd, ptr, n))<=0)
				{
					exit(1);
				}

				n-=nw;
				ptr+=nw;

			}
		}

		close(newfd);

	}

	close(fd);
	exit(0);
}
