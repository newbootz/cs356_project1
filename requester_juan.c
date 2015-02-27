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

static int getPacketID(char p[])
{
	char temp[20];
	int id = -1;
	int i = 0;
	while(p[i] != '|')
	{
		temp[i] = p[i]; 
		i++;
	}
	id = atoi(temp);
	return id;
}

static char * getPacketData(char p[])
{
	char *temp = strchr(p, '|');
	return temp+=1;
}

int main ( int argc, char *argv[] )
{
	int i, j;
	char * ip_address;
	int port_number = 12000;
	char * file_name;
	int number_messages;

	int sockfd,n;
	struct sockaddr_in servaddr, cliaddr;
	char sendline[1000];
    char recvline[1000];
    char * recTemp;
    char nBuff[sizeof(int)*3+2];

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
				default : 
					printf("Error: parameters provided incorrect\n");

			}
		}
		
	}
	//reading bytes from file
	FILE *fileptr;
	char *buffer;
	long filelen;
	fileptr = fopen(file_name,"rb");
	fseek(fileptr,0,SEEK_END);
	filelen = ftell(fileptr);
	rewind(fileptr);

	buffer = (char *)malloc((filelen+1)*sizeof(char)); // Enough memory for file + \0
	fread(buffer, filelen, 1, fileptr); // Read in the entire file
	fclose(fileptr);
	//finished reading bytes from file
	
	int packet_id = 0;
	//setup socket
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr=inet_addr(ip_address);
    servaddr.sin_port=htons(port_number);
	

    for(i =0; i < number_messages; i++){
		printf("\nNumber: %d\n\n\n", i+1);
		char* buffer_ptr = buffer;
		int reached_eof = 0;
		printf("about to break file up into packets\n\n\n");
		while(!reached_eof)
		{
			//add header to packet
			int offset = snprintf(sendline, sizeof(sendline), "%d", packet_id);
			//printf("offset: %d\n", offset);
			sendline[offset++] = '|';
			
			//keep looping until we fill up the packet with info
			printf("\n\nmaking a packet number: %d\n\n\n", packet_id);
			while(offset < 999 )
			{
				//make sure we haven't reached_eof
				if(*(buffer_ptr) != '\0')
				{
					//copy byte from buffer into sendline
					sendline[offset] = *(buffer_ptr);
					offset+=1;
					buffer_ptr+=1;
				}
				else
				{
					//reached end of the file, break out
					reached_eof = 1;
					sendline[offset++] = *(buffer_ptr);
					sendline[offset]= '\0';
					break;
				}
			}

			printf("finished packet number: %d\n\n\n", packet_id);
			offset = 0;
			

			//send the packet
		    sendto(sockfd,sendline,strlen(sendline),0,(struct sockaddr *)&servaddr,sizeof(servaddr));
	
		    n=recvfrom(sockfd,recvline,1000,0,NULL,NULL);
		    //get packet id from received packet and compare to send
		    // packet id; If they are the same then print it
		    
		    if(getPacketID(recvline) == packet_id)
		    {
		    	recTemp = getPacketData(recvline);
		    	fputs(recTemp,stdout);
		    }
		    
	
			packet_id+=1;
			memset(sendline,0,sizeof(sendline));
			memset(recvline,0,sizeof(recvline));
		}
		printf("\nfinished sending file number:%d\n\n\n",i +1);
	}
	return 0;
}





