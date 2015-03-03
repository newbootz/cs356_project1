/*
Jesus Galvan
Juan P. Mata jpm2873

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
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/signal.h>

int sockfd,n;
struct sockaddr_in servaddr, cliaddr;
char sendline[1000];

void alarmhand(int signal)
{
	printf("TIMEOUT FOR THIS PACKET\n\n\n");
	sendto(sockfd,sendline,strlen(sendline),0,(struct sockaddr *)&servaddr,sizeof(servaddr));
	
	alarm(2);
}
//void send_packet(int signal,int sockfd, char* sendline,struct sockaddr_in servaddr)
//{
//	sendto(sockfd,sendline,strlen(sendline),0,(struct sockaddr *)&servaddr,sizeof(servaddr));
//}

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
	char *temp = strchr(p, '-');
	return temp+=1;
}

// CITE: got this method from geeksforgeeks.org
// minor modification for use in my code
int valid_digit(char *ip_str)
{
    while (*ip_str) {
        if (*ip_str >= '0' && *ip_str <= '9')
            ++ip_str;
        else
        {
        	return 0;
 		}
    }
    return 1;
}

// CITE: got this method from geeksforgeeks.org
// minor modifications for use in my code
int is_valid_ip(char *ip_str)
{
    int i, num, dots = 0;
    char *ptr;
    char temp[20];
 
    if (ip_str == NULL)
    {
    	printf("Invalid IP Address\n");	
        return 0;
 	}
 	
 	i=0;
 	while(ip_str[i] != '\0')
    {
    	temp[i] = ip_str[i];
    	i++;
    }
    
    ptr = strtok(temp, ".");
 
    if (ptr == NULL)
    {
    	printf("Invalid IP Address\n");	
        return 0;
 	}
 
    while (ptr) {
 
        /* after parsing string, it must contain only digits */
        if (!valid_digit(ptr))
        {
    		printf("Invalid IP Address\n");	
        	return 0;
 		}
 
        num = atoi(ptr);
 
        /* check for valid IP */
        if (num >= 0 && num <= 255) {
            /* parse remaining string */
            ptr = strtok(NULL, ".");
            if (ptr != NULL)
                ++dots;
        } else{
    		printf("Invalid IP Address\n");	
        	return 0;
 		  }
    }
 
    /* valid IP string must contain 3 dots */
    if (dots != 3)
     {
    	printf("Invalid IP Address\n");	
        return 0;
 	}
    return 1;
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
	char * ip_address;
	int port_number = 12000;
	char * file_name;
	int number_messages;

//	int sockfd,n;
//	struct sockaddr_in servaddr, cliaddr;
//	char sendline[1000];
    char recvline[1000];
    char * recTemp;
    char nBuff[sizeof(int)*3+2];
	clock_t begin, end;
	double time_spent;
	//set up timer for wait time on packets
	struct timespec tim, tim2;
	tim.tv_sec = 1;
	tim.tv_sec = 5000000;

	// boolean check if all args given
	int a =0;
	int p = 0;
	int f = 0;
	int m = 0;

	memset(sendline,0,sizeof(sendline));

	for(i = 0; i < argc; ++i)
	{
		if(argv[i][0] == '-')
		{
			switch(tolower(argv[i][1]))
			{
				case 'a':
					ip_address = argv[i+1];
					if(is_valid_ip(ip_address) == 0){return 0;}
					ip_address = argv[i+1];
					printf("IP address: %s\n", ip_address);
					a = 1;
					break;
				case 'p':
					if(!valid_num(argv[i+1])){printf("Invalid Port Number!\n"); return 0;}
					port_number = atoi(argv[i+1]);
					if(port_number <= 0){printf("Invalid Port Number!\n");
						return 0;}
					printf("Port number: %d\n", port_number);
					p = 1;
					break;
				case 'f':
					file_name = argv[i+1];
					printf("File name: %s\n", file_name);
					f = 1;
					break;
				case 'n':
					if(!valid_num(argv[i+1])){printf("Invalid Value For -n!\n"); return 0;}
					number_messages = atoi(argv[i+1]);
					if(number_messages <= 0)
					{	printf("Invalid Value For -n!\n");
						return 0;}
					printf("Number of Messages: %d\n", number_messages);
					m = 1;

			}
		}
		
	}
	if(a !=1 || p !=1 || f !=1 || m !=1){ printf("Incorrect Arguments!\n");}
	/*printf("Before being clock\n");
	begin = clock();
	printf("After being clock\n");*/
	//reading bytes from file
	FILE *fileptr;
	char *buffer;
	long filelen;
	fileptr = fopen(file_name,"rb");
	if(fileptr == NULL)
	{	printf("Invalid File!\n");
		return 0;}
	//printf("After file open\n");
	fseek(fileptr,0,SEEK_END);
	filelen = ftell(fileptr);
	rewind(fileptr);

	buffer = (char *)malloc((filelen+1)*sizeof(char)); // Enough memory for file + \0
	fread(buffer, filelen, 1, fileptr); // Read in the entire file
	fclose(fileptr);
	//finished reading bytes from file
	//allocate array for storing RTT values
	double rtt_sum = 0.0;
	double rtt_min = 0.0;
	double rtt_max = 0.0;

	int packet_id = 0;
	//setup socket
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr=inet_addr(ip_address);
    servaddr.sin_port=htons(port_number);

	char* buffer_ptr = buffer;
	int reached_eof = 0;
//	printf("about to break file up into packets\n\n\n");
	
	for(i = 0; i < number_messages; i++)
	{
		
		begin = clock();
		//printf("Begin time: %ld\n", end);
		printf("\nNumber: %d\n\n", i+1);
		char* buffer_ptr = buffer;
		int reached_eof = 0;
//		printf("about to break file up into packets\n\n\n");

		while(!reached_eof)
		{
			//add HEADER to packet
			int offset = snprintf(sendline, sizeof(sendline), "%d|%lu", packet_id, filelen);
			//printf("%d|%lu\n", packet_id, filelen);
			//printf("offset: %d\n", offset);
			sendline[offset++] = '-';
			
			//keep looping until we fill up the packet with info
			//printf("\n\nmaking a packet number: %d\n\n\n", packet_id);
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

			//printf("finished packet number: %d\n\n\n", packet_id);
			offset = 0;
			

			//send the packet
		    sendto(sockfd,sendline,strlen(sendline),0,(struct sockaddr *)&servaddr,sizeof(servaddr));
	
		    //n=recvfrom(sockfd,recvline,1000,0,NULL,NULL);
		    //get packet id from received packet and compare to send
		    // packet id; If they are the same then print it
		    
			signal(SIGALRM,alarmhand);
			alarm(2);
		    	n=recvfrom(sockfd,recvline,1000,0,NULL,NULL);
			alarm(0);
		    	//fputs(recvline,stdout);
				if(getPacketID(recvline) == packet_id)
			    {
			    	recTemp = getPacketData(recvline);
			    	fputs(recTemp,stdout);
			    }

			//increase packet_id, lets fill up another packet
	//		return 0;
			packet_id+=1;
			memset(sendline,0,sizeof(sendline));
			memset(recvline,0,sizeof(recvline));
		}

		//printf("\nfinished sending file\n\n\n");
		end = clock();
		//printf("End time: %ld\n", end);
		time_spent = (double) (end - begin) / (CLOCKS_PER_SEC/1000);
		if(rtt_min == 0.0){rtt_min = time_spent;}
		if(time_spent < rtt_min)
		{
			rtt_min = time_spent;
		}
		if(rtt_max < time_spent)
		{
			rtt_max = time_spent;
		}
		rtt_sum+=time_spent;
		//time_spent = end - begin;
		printf("\nRTT: %fms\n", time_spent);
	}
	double avg_rtt = rtt_sum/number_messages;
	printf("AveRTT: %f ms MinRTT: %f ms MaxRTT: %f ms\n",avg_rtt,rtt_min,rtt_max);
	return 0;
}

