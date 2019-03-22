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
	int val, sc,sd,res;
	unsigned int size;
	char port[256]= "";
	int num [2];
	struct sockaddr_in broker_addr, editor_addr;

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


	printf("Port: %s\n", port);

	if((sd=socket(AF_INET,SOCK_STREAM,6))==-1){
		printf("Error in creating socket.\n");
		exit(0);
	}
	 
	 val=1;
	 setsockopt(sd,SOL_SOCKET, SO_REUSEADDR, (char*) &val, sizeof(int));
	 
	 bzero((char *) &broker_addr, sizeof(broker_addr));

	 int port_n=atoi(port);
	 broker_addr.sin_family = AF_INET;
   	 broker_addr.sin_addr.s_addr = INADDR_ANY;
  	 broker_addr.sin_port = htons(port_n);

	 if(bind(sd, (struct sockaddr *)&broker_addr, sizeof(broker_addr))<0){
		 perror("Error on socket bind.\n");
		 exit(0);
	 }

	 if(listen(sd, 5)==-1){
		 perror("Error on socket listen.\n");
	 }

	size=sizeof(editor_addr);
	
	while(1){

	printf("Waiting for connection\n");
	
	 if((sc=accept(sd,(struct sockaddr *)&editor_addr,&size))<0){
		 perror("Error on accepting connection.\n");
		 exit(0);
	 }
	if(recv(sc, (char *) num, 2*sizeof(int),0)<0){
		 perror("Error on receiving.\n");
		 exit(0);
	 }	
	res=num[2]+num[1];

	if(send(sc, &res, sizeof(int),0)==-1){
		printf("Error on sending.\n");
		exit(0);
	}

}
	close(sd);
	return 0;
}
