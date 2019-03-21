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
	    printf("Usage: broker -p port \n");
}

int main(int argc, char *argv[]) {
	int  option = 0;
	char port[256]= "";

	while ((option = getopt(argc, argv,"p:")) != -1) {
		switch (option) {
		    	case 'p' :
				strcpy(port, optarg);
		    		break;
		    	default:
				print_usage();
		    		exit(-1);
		    }
	}
	if (strcmp(port,"")==0){
		print_usage();
		exit(-1);
	}

	int sd;

	if((sd=socket(AF_INET,SOCK_STREAM,6))==-1){
		printf("Error in creating socket.\n");
		exit(0);
	}

	 struct sockaddr_in broker_addr;
	 bzero((char *) &broker_addr, sizeof(broker_addr));

	 int port_num=5001;
	 broker_addr.sin_family = AF_INET;
   broker_addr.sin_addr.s_addr = INADDR_ANY;
   broker_addr.sin_port = htons(port_num);

	 if(bind(sd, (struct sockaddr *)&broker_addr, sizeof(broker_addr))<0){
		 perror("Error on socket bind.\n");
		 exit(0);
	 }

	 if(listen(sd, 5)==-1){
		 perror("Error on socket listen.\n");
	 }




	printf("Port: %s\n", port);
	return 0;
}
