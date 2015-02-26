#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
int main ( int argc, char *argv[] )
{
	int i;
	int port_number;

	int sockfd,n;
	struct sockaddr_in servaddr, cliaddr;
	socklen_t len;
	char mesg[1000];

	for(i = 0; i < argc; ++i)
	{
		if(argv[i][0] == '-')
		{
			switch(tolower(argv[i][1]))
			{
				case 'p':
					port_number = atoi(argv[i+1]);
					printf("Port number: %d\n", port_number);
					break;
			}
		}
	}

   sockfd=socket(AF_INET,SOCK_DGRAM,0);
   bzero(&servaddr,sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
   servaddr.sin_port=htons(port_number);
   bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));


   for (;;)
   {
      len = sizeof(cliaddr);
      n = recvfrom(sockfd,mesg,1000,0,(struct sockaddr *)&cliaddr,&len);
      sendto(sockfd,mesg,n,0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
      printf("-------------------------------------------------------\n");
      mesg[n] = 0;
      printf("Received the following:\n");
      printf("%s",mesg);
      printf("-------------------------------------------------------\n");
   }
	return 0;
}