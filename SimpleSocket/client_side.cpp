#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h> 
#include <string.h>

int main(int argc, char *argv[]){

	struct sockaddr_in server_addr;
	int sckt, sckt_client, status;
	struct hostent *server;

	char *buffer;


	sckt = socket(AF_INET, SOCK_STREAM, 0);

	if(sckt == -1)
	{
	    printf("error opening socket\n");
	    return -1;
	}

	server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"Cannot connect to the server\n");
        return -1;
    }

    int port = atoi(argv[2]);
	server_addr.sin_port = htons(port);
	memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);
	server_addr.sin_family = AF_INET;

	if (connect(sckt,(struct sockaddr *) &server_addr,sizeof(server_addr)) < 0) {
		printf("Couldn't connect\n");
    	return -1;
	}

	buffer = (char*)malloc(255);
	memcpy(buffer, "test message", 12);

	status = write(sckt,buffer,strlen(buffer));
    if (status < 0){
    	printf("Couldn't write the message\n");
    	return -1;
    }

	close(sckt);
	return 0;
}