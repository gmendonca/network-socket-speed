#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char *argv[]){

	struct sockaddr_in server_addr, client_addr;
	int sckt, sckt_client;
	socklen_t client;


	sckt = socket(AF_INET, SOCK_STREAM, 0);

	if(sckt == -1)
	{
	    printf("error opening socket");
	    return -1;
	}

	int port = atoi(argv[1]);
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //loopback interface
	server_addr.sin_family = AF_INET;

	if(bind(sckt, (struct sockaddr *)&server_addr,sizeof(struct sockaddr_in) ) == -1)
	{
	    printf("error binding socket");
	    return -1;
	}

	//keep listening
	listen(sckt,5);
	client = sizeof(client_addr);
    sckt_client = accept(sckt, (struct sockaddr *) &client_addr, &client);


	close(sckt);
	return 0;
}