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
#include <pcap.h>

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
	char *buffer;
	unsigned int buffer_size;
	int bytesRcvd, totalBytesRcvd;
	socklen_t client;

	sckt = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

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

    
    buffer = (char *)calloc( 1, lSize+1 );
    status = read(sckt_client,buffer,lSize);
    if (status < 0){
    	printf("Cannot read from the socket\n");
    	exit(EXIT_FAILURE);
    }

    //printf("Message: %s\n",buffer);

	close(sckt);
	close(sckt_client);
	pthread_exit(NULL);
	exit(EXIT_SUCCESS);
}

void* tcp_client(void *whatever){

	struct sockaddr_in server_addr;
	int sckt, status, len;
	struct hostent *server;

	sckt = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

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

    len = strlen(h.buffer);

    status = write(sckt, h.buffer, len);
    if (status < 0){
    	printf("Couldn't write the message\n");
    	exit(EXIT_FAILURE);
    }

	close(sckt);
	pthread_exit(NULL);
	exit(EXIT_SUCCESS);
}

void my_callback(u_char *useless,const struct pcap_pkthdr* pkthdr,const u_char* packet) {
    static int count = 1;
    fprintf(stdout,"%d, ",count);
    if(count == 4)
        fprintf(stdout,"Come on baby sayyy you love me!!! ");
    if(count == 7)
        fprintf(stdout,"Tiiimmmeesss!! ");
    fflush(stdout);
    count++;
}

void* packetcapture(void *whatever) { 
    int i;
    char *dev; 
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* descr;
    const u_char *packet;
    struct pcap_pkthdr hdr;     /* pcap.h */
    struct ether_header *eptr;  /* net/ethernet.h */

    /* grab a device to peak into... */
    //dev = pcap_lookupdev(errbuf);
    dev = (char*)h.server;
    if(dev == NULL){
    	printf("%s\n",errbuf);
    	exit(EXIT_FAILURE);
    }
    /* open device for reading */
    descr = pcap_open_live(dev,BUFSIZ,0,-1,errbuf);

    if(descr == NULL){
    	printf("pcap_open_live(): %s\n",errbuf);
    	exit(EXIT_FAILURE);
    }

    pcap_loop(descr,10,my_callback,NULL);
    
    pthread_exit(NULL);
	exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]){
	
	int t1,t2,t3;
	int starttime, stoptime, startpcaptime, stoppcaptime;
	pthread_t thread1, thread2, thread3;

	h.server = gethostbyname(argv[1]);
	h.port = atoi(argv[2]);

	FILE *fp;

	fp = fopen("../txt/1b.txt", "rb");
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
	const char *message3 = "Thread 3";
	
	startpcaptime = GetTimeMs();
    t3 = pthread_create(&thread3, NULL, packetcapture, (void *)message3);
    if (t3) {
        printf("ERROR; thread tcp_server");
        return -1;
    }
    stoppcaptime = GetTimeMs();


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

    printf("Duration = %d us\n", stoptime - starttime);
    printf("Duration pcap = %d us\n", stoppcaptime - startpcaptime);

	pthread_exit(NULL);
	return 0;
}