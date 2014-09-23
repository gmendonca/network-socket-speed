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
#include <pthread.h>

#define NUM_THREADS 2

struct Host{
	struct hostent *server;
	int port;
	char *buffer;
};

Host h;

long lSize;

int GetTimeMs()
{
 /* Linux */
 struct timeval tv;

 gettimeofday(&tv, NULL);

 unsigned int ret = tv.tv_usec;

 /* Adds the seconds (10^0) after converting them to milliseconds (10^-3) */
 ret += (tv.tv_sec * 1000000);

 return ret;
}

void* tcp_server(void *whatever){

	struct sockaddr_in server_addr, client_addr;
	int sckt, sckt_client, status;
	int starttime, stoptime;
	char *buffer;
	socklen_t client;
	int iSetOption = 1;

	sckt = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	setsockopt(sckt, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption, sizeof(iSetOption));

	if(sckt == -1)
	{
	    printf("error opening socket\n");
	    exit(EXIT_FAILURE);
	}

	server_addr.sin_port = htons(h.port);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //loopback interface
	server_addr.sin_family = AF_INET;

	if(bind(sckt, (struct sockaddr *)&server_addr,sizeof(struct sockaddr_in) ) == -1)
	{
	    printf("error binding socket\n");
	    exit(EXIT_FAILURE);
	}

	//keep listening
	listen(sckt,5);
	client = sizeof(client_addr);
    sckt_client = accept(sckt, (struct sockaddr *) &client_addr, &client);

    if (sckt_client < 0){
    	printf("Didn't accept\n");
    	exit(EXIT_FAILURE);
    } 

    starttime = GetTimeMs();
    buffer = (char *)calloc( 1, lSize+1 );
    status = read(sckt_client,buffer,lSize);
    if (status < 0){
    	printf("Cannot read from the socket\n");
    	exit(EXIT_FAILURE);
    }
    stoptime = GetTimeMs();

    //printf("Message: %s\n",buffer);
    printf("Duration server read = %d us\n", stoptime - starttime);

    starttime = GetTimeMs();
    status = write(sckt_client,buffer,lSize);
	if (status < 0){
	  	printf("Couldn't write the message\n");
	   	exit(EXIT_FAILURE);
	}
	stoptime = GetTimeMs();
    printf("Duration server write back = %d us\n", stoptime - starttime);

	close(sckt);
	close(sckt_client);
	pthread_exit(NULL);
	exit(EXIT_SUCCESS);
}

void* tcp_client(void *whatever){

	struct sockaddr_in server_addr;
	int sckt, status, len;
	int starttime, stoptime;
	struct hostent *server;
	int iSetOption = 1;

	sckt = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	setsockopt(sckt, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption, sizeof(iSetOption));

	if(sckt == -1)
	{
	    printf("error opening socket\n");
	    exit(EXIT_FAILURE);
	}

    if (h.server == NULL) {
        fprintf(stderr,"Cannot connect to the server\n");
        exit(EXIT_FAILURE);
    }

	server_addr.sin_port = htons(h.port);
	memcpy(&server_addr.sin_addr.s_addr, h.server->h_addr, h.server->h_length);
	server_addr.sin_family = AF_INET;

	if (connect(sckt,(struct sockaddr *) &server_addr,sizeof(server_addr)) < 0) {
		printf("Couldn't connect\n");
    	exit(EXIT_FAILURE);
	}

	starttime = GetTimeMs();
	status = write(sckt, h.buffer, lSize);
	if (status < 0){
	  	printf("Couldn't write the message\n");
	   	exit(EXIT_FAILURE);
	}
	stoptime = GetTimeMs();	    

    printf("Duration client write = %d us\n", stoptime - starttime);

    //sleep(2);

    starttime = GetTimeMs();
    char *message = (char *)calloc( 1, lSize+1 );
    status = read(sckt,message,lSize);
    if (status < 0){
    	printf("Cannot read from the socket\n");
    	exit(EXIT_FAILURE);
    }
    stoptime = GetTimeMs();

    printf("Duration client read back = %d us\n", stoptime - starttime);
    //printf("%s\n", message);

	close(sckt);
	pthread_exit(NULL);
	exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]){
	
	int t1,t2;
	int starttime, stoptime;
	pthread_t thread1, thread2;

	h.server = gethostbyname(argv[1]);
	h.port = atoi(argv[2]);

	FILE *fp;

	if(strcmp(argv[3], "1b") == 0)fp = fopen("../txt/1b.txt", "rb");
	else if(strcmp(argv[3], "1kb") == 0)fp = fopen("../txt/1kb.txt", "rb");
	else if(strcmp(argv[3], "64kb") == 0)fp = fopen("../txt/64kb.txt", "rb");
	else if(strcmp(argv[3], "1mb") == 0)fp = fopen("../txt/1mb.txt", "rb");
	else fp = fopen("../txt/alice.txt", "rb");

    if (!fp) {
        fprintf(stderr, "Failed to load file.\n");
        return -1;
    }

	fseek( fp , 0L , SEEK_END);
	lSize = ftell( fp );
	rewind( fp );

	/* allocate memory for entire content */
	h.buffer = (char *)calloc( 1, lSize+1 );
	if( !h.buffer ){
		fclose(fp);
		printf("Couldn't allocate memory\n");
		return -1;
	}

	/* copy the file into the buffer */
	if( fread( h.buffer , lSize, 1 , fp) != true){
		fclose(fp);
		free(h.buffer);
		printf("Couldn't copy in buffer\n");
		return -1;
	}

	fclose(fp);

	const char *message1 = "Thread 1";
	const char *message2 = "Thread 2";

	starttime = GetTimeMs();
	t1 = pthread_create(&thread1, NULL, tcp_server, (void *)message1);
    if (t1) {
        printf("ERROR; thread tcp_server");
        return -1;
    }

    t2 = pthread_create(&thread2, NULL, tcp_client, (void *)message2);
    if (t2) {
        printf("ERROR; thread tcp_client");
        return -1;
    }
	stoptime = GetTimeMs();

    //printf("Duration= %d us\n", stoptime - starttime);

	pthread_exit(NULL);
	return 0;
}