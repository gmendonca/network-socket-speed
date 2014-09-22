#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h> 
#include <string.h>

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

	struct sockaddr_in server_addr, client_addr;
	int sckt_server, sckt_client, status;
	socklen_t client;

	char *buffer;

	/* ---------------- Process 1 - Server ----------------*/
	sckt_server = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(sckt_server == -1)
	{
	    printf("error opening socket\n");
	    return -1;
	}

	int port = atoi(argv[1]);
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //loopback interface
	server_addr.sin_family = AF_INET;

	if(bind(sckt_server, (struct sockaddr *)&server_addr,sizeof(struct sockaddr_in) ) == -1)
	{
	    printf("error binding socket\n");
	    return -1;
	}

	//keep listening
	listen(sckt_server,5);

	/* ---------------- Process 2 - Client ----------------*/

	sckt_client = socket();
	
	if (connect(sckt,(struct sockaddr *) &server_addr,sizeof(server_addr)) < 0) {
		printf("Couldn't connect\n");
    	return -1;
	}

	close(sckt);
	close(sckt_client);
	return 0;
}