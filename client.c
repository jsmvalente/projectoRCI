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

	hostptr=gethostbyname("tejo.tecnico.ulisboa.pt");
}
