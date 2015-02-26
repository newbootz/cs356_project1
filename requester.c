/*
a client program and a server program. 
All communication must be done in UDP. 
Each program is detailed below. 
At a high-level, your code will take an input file, 
break it down into packets, and then send 
the file “n” times to the server. The server will 
echo the file back to the client each of the “n” times. 


———————Client Program———————
Name your main file requester.c. 
The program should support the following input:
-a <IP address>The IP address of the server response program
-p <port> The port number of the server response program
-f <text file>File, containing ASCII text, whose contents will be sent to the server
-n <number messages>The number of times the file will be sent to the serverThese 
input parameters must be entered, and can be entered in any order. 
Be sure that your code handles incorrect/insufficient input and outputs errors 
when appropriate.
*/

#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>


int main ( int argc, char *argv[] )
{
	int i;
	char * ip_address;
	int port_number = 12000;
	char * file_name;
	int number_messages;

	int sockfd,n;
	struct sockaddr_in servaddr, cliaddr;
	char sendline[1000];
    char recvline[1000];

	for(i = 0; i < argc; ++i)
	{
		if(argv[i][0] == '-')
		{
			switch(tolower(argv[i][1]))
			{
				case 'a':
					ip_address = argv[i+1];
					printf("IP address: %s\n", ip_address);
					break;
				case 'p':
					port_number = atoi(argv[i+1]);
					printf("Port number: %d\n", port_number);
					break;
				case 'f':
					file_name = argv[i+1];
					printf("File name: %s\n", file_name);
					break;
				case 'n':
					number_messages = atoi(argv[i+1]);
					printf("Number of Messages: %d\n", number_messages);

			}
		}
		
	}
	sendline = file_name;
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr=inet_addr(ip_address);
    servaddr.sin_port=htons(port_number);

    sendto(sockfd,sendline,strlen(sendline),0,(struct sockaddr *)&servaddr,sizeof(servaddr));
    n=recvfrom(sockfd,recvline,10000,0,NULL,NULL);
    fputs(recvline,stdout);
	return 0;
}