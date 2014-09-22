#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h> 
#include <string.h>
#include <sys/time.h>
#include <ctime>

int GetTimeMs()
{
 /* Linux */
 struct timeval tv;

 gettimeofday(&tv, NULL);

 unsigned int ret = tv.tv_usec;
 /* Convert from micro seconds (10^-6) to milliseconds (10^-3) */
 ret /= 1000;

 /* Adds the seconds (10^0) after converting them to milliseconds (10^-3) */
 ret += (tv.tv_sec * 1000);

 return ret;
}

int main(int argc, char *argv[]){

	struct sockaddr_in server_addr;
	int sckt, status;	
	char *buffer, *rcvbuffer;
	unsigned int buffer_size;
	int bytesRcvd, totalBytesRcvd;
	socklen_t client;

	sckt = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(sckt == -1)
	{
	    printf("error opening socket\n");
	    return -1;
	}

	int port = atoi(argv[1]);
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //loopback interface
	server_addr.sin_family = AF_INET;

	if(bind(sckt, (struct sockaddr *)&server_addr,sizeof(struct sockaddr_in) ) == -1)
	{
	    printf("error binding socket\n");
	    return -1;
	}

	//keep listening
	listen(sckt,5);
	client = sizeof(client_addr);
    sckt_client = accept(sckt, (struct sockaddr *) &client_addr, &client);

    if (sckt_client < 0){
    	printf("Didn't accept\n");
    	return -1;
    } 

    buffer = (char*)malloc(255);
    status = read(sckt_client,buffer,255);
    if (status < 0){
    	printf("Cannot read from the socket\n");
    	return -1;
    }

    printf("Message: %s\n",buffer);

	close(sckt);
	close(sckt_client);
	return 0;
}