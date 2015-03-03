/*
Jesus Galvan
Juan P. Mata jpm2873
*/

#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

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

static int getFileLenPacket(char p[])
{
	char *temp = strchr(p, '|');
	temp+=1;
	char num[30];
	int len = -1;
	int i = 0;
	while(temp[i] != '-')
	{
		num[i] = temp[i]; 
		i++;
	}
	len = atoi(num);
	return len;
}

static char * getPacketData(char p[])
{
	char *temp = strchr(p, '-');
	return temp+=1;
}

int valid_num(char *num)
{
	int i =0;
    while (num[i] != '\0') {
        if (num[i] >= '0' && num[i] <= '9')
            i++;
        else
        	return 0;
    }
    return 1;
}

int main ( int argc, char *argv[] )
{
	int i;
	int port_number;

	int sockfd,n;
	struct sockaddr_in servaddr, cliaddr;
	socklen_t len;
	char mesg[1000];
	char * recTemp;
	int packet_id;
	int packet_used[9000] = { 0 };

	char *buffer;
	long filelen;
	int index;

	for(i = 0; i < argc; ++i)
	{
		if(argv[i][0] == '-')
		{
			switch(tolower(argv[i][1]))
			{
				case 'p':
					if(!valid_num(argv[i+1])){printf("Invalid Port Number!\n"); return 0;}
					port_number = atoi(argv[i+1]);
					if(port_number <= 0){printf("Invalid Port Number!\n");
						return 0;}
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
      //get packet number and convert to int
      packet_id = getPacketID(mesg);
      if(packet_id == 0)
      {
      	filelen = getFileLenPacket(mesg);
      	buffer = (char *)malloc((filelen+1)*sizeof(char)); // Enough memory for file + \0
      	memset(packet_used,0,sizeof(packet_used));
      }
      if(packet_used[packet_id] == 0)
      {
    	  recTemp = getPacketData(mesg);
    	  fputs(recTemp,stdout);
    	  //index = snprintf(buffer, sizeof(buffer), "%s", recTemp);
    	  packet_used[packet_id] = 1;
      } 
      
  	  //check cache for if we already have it
//  	  sleep(6);
      sendto(sockfd,mesg,n,0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
//      printf("-------------------------------------------------------\n");
      //mesg[n] = 0;
      memset(mesg,0,sizeof(mesg));
//      printf("Received the following:\n");
      //printf("%s",mesg);
//      printf("-------------------------------------------------------\n");
   }
	return 0;
}
