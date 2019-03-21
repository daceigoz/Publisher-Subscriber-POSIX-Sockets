#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

void print_usage() {
	    printf("Usage: editor -h host -p port -t \"topic\" -m \"text\"\n");
}

int main(int argc, char *argv[]) {
	int  option = 0;
	char host[256]= "";
	char port[256]= "";
	char topic[256]= "";
	char text[1024]= "";
	int sd;
	struct hostent *hp;
	struct in_addr in;
	struct sockaddr_in server_addr;
	struct sockaddr *addr;
	int addr_s;

	while ((option = getopt(argc, argv,"h:p:t:m:")) != -1) {
		switch (option) {
		        case 'h' :
				strcpy(host, optarg);
		    		break;
		    	case 'p' :
				strcpy(port, optarg);
		    		break;
		    	case 't' :
				strcpy(topic, optarg);
		    		break;
		    	case 'm' :
				strcpy(text, optarg);
		    		break;
		    	default:
				print_usage();
		    		exit(-1);
		    }
	}
	if (strcmp(host,"")==0 || strcmp(port,"")==0 ||
		strcmp(topic,"")==0 || strcmp(text,"")==0){
		print_usage();
		exit(-1);
	}
	printf("Host: %s\n", host);
	printf("Port: %s\n", port);
	printf("Topic: %s\n", topic);
	printf("Text: %s\n", text);
	int port_n=atoi(port);

	if((sd=socket(AF_INET,SOCK_STREAM,6))==-1){
		printf("Error in creating socket.\n");
		exit(0);
	}

	bzero((char*)&server_addr, sizeof(server_addr));
	hp=gethostbyname(host);

	memcpy(&(server_addr.sin_addr),hp->h_addr,hp->h_length);
	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(port_n);






	return 0;
}
