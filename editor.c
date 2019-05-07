#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "lines.h"
#include "node_t.h"

void print_usage() {
	    printf("Usage: editor -h host -p port -t \"topic\" -m \"text\"\n");
}

int main(int argc, char *argv[]) {
	int  option = 0;
	char host[256]= "";
	char port[256]= "";
	char topic[128]="";
	char text[1024]="";

	char * action= "PUBLISH";
	int sd;
	struct hostent *hp;
	struct sockaddr_in broker_addr;

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
	//We create the socket
	if((sd=socket(AF_INET,SOCK_STREAM,6))==-1){
		printf("Error in creating socket.\n");
		exit(0);
	}

	bzero((char*)&broker_addr, sizeof(broker_addr));
	hp=gethostbyname(host);//Convert the host address into the correct address structure

	//The whole adress structure is made
	memcpy(&(broker_addr.sin_addr),hp->h_addr,hp->h_length);
	broker_addr.sin_family=AF_INET;
	broker_addr.sin_port=htons(port_n);

	//The editor is connected to the broker
	if(connect(sd, (struct sockaddr *) &broker_addr, sizeof(broker_addr))==-1){
		printf("Error in the connection to the server <%s>:<%d>\n",host,port_n);
		exit(0);
	}
	printf("Connected to the server.\n");
	//The action, topic and text are sent
	if(writeLine(sd, action, strlen(action))==-1){
		printf("Error on sending the action.\n");
		exit(0);
	}

	if(writeLine(sd, topic, strlen(topic))==-1){
		printf("Error on sending the topic.\n");
		exit(0);
	}

	if(writeLine(sd, text, strlen(text))==-1){
		printf("Error on sending the text.\n");
		exit(0);
	}

	close(sd);


	return 0;
}
