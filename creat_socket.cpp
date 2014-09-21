#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]){

	struct sockaddr_in sin;

	int sckt = socket(AF_INET, SOCK_STREAM, 0);

	if(sckt == -1)
	{
	    printf("error opening socket");
	    return -1;
	}

	sin.sin_port = htons(1345);
	sin.sin_addr.s_addr = inet_addr("127.0.0.1");
	sin.sin_family = AF_INET;

	if(bind(sckt, (struct sockaddr *)&sin,sizeof(struct sockaddr_in) ) == -1)
	{
	    printf("error binding socket");
	    return -1;
	}

	return 0;
}