#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "node_t.h"

struct node * head=NULL;
int empty=1;
int existing_topic=0;
int res;

void print_topics(){
	if(!empty){
		printf("Printing the current list of topics.\n");
		struct node * aux1=head;
		printf("TOPIC: %s\n", aux1->topic);
		while(aux1->next!=NULL){
			printf("TOPIC: %s\n", aux1->next->topic);
			aux1=aux1->next;
		}
	}
}

void print_usage() {
	    printf("Usage: broker -p port \n");
}

int main(int argc, char *argv[]) {
	int  option = 0;
	int val, sc,sd;
	unsigned int size;
	char port[256]= "";
	char action[1024]= "";
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
	int action_rcv=0;
	int action_type=0;


	struct node * aux2=malloc(sizeof(struct node));
	strcpy(aux2->topic, "");
	aux2->next=NULL;

	while(1){

	struct node * aux1=head;
	existing_topic=0;

	action_rcv=0;


	 if((sc=accept(sd,(struct sockaddr *)&editor_addr,&size))<0){
		 perror("Error on accepting connection.\n");
		 exit(0);
	 }
	 printf("Waiting for action\n");
	 while(action_rcv!=3){
			if(action_rcv==1) printf("Waiting for topic\n");
			else if(action_rcv==2)printf("Waiting for text\n");
			if(recv(sc, action, 1024,0)<0){
				 perror("Error on receiving.\n");
				 exit(0);
			 }

			 			if(!strcmp(action,"PUBLISH")){
			 					 			 action_rcv=1;
			 								 action_type=0;
			 					 			 printf("ACTION: %s\n",action);
			 			}
			 			else if(!strcmp(action,"SUBSCRIBE")){
			 					 			 action_rcv=1;
			 								 action_type=1;
			 					 			 printf("ACTION: %s\n",action);
			 			}
			 			else if(!strcmp(action,"UNSUBSCRIBE")){
			 					 			 action_rcv=1;
			 								 action_type=2;
			 					 			 printf("ACTION: %s\n",action);
			 			}
			 			else{
			 								action_rcv=3;
			 			}

		 if(action_rcv==1){
			 if(recv(sc, action, 1024,0)<0){
					perror("Error on receiving.\n");
					exit(0);
				}
			 //topic received
			 printf("TOPIC: %s\n",action);
						 if(action_type==0){//PUBLISH action
							 if(!empty){
								 while(aux1->next!=NULL){

									 if(!strcmp(aux1->topic,action)){
										 //Deliver text to the subscriptor of that node of the list.
									 }
									 aux1=aux1->next;
								 }
								 if(!strcmp(aux1->topic,action)){
									 //Deliver text to the subscriptor of that node of the list.
								 }
							 }
						 action_rcv=3;//3 if topic not found
					 	}
					 	else if(action_type==1){//SUBSCRIBE
			 			 	strcpy(aux2->topic, action);
							getpeername(sc,aux2->addr,&size);
							int return_value=0;
							if(empty){
								head=aux2;
								empty=0;
								return_value=0;
							}
							else{
								while(aux1->next!=NULL){
									aux1=aux1->next;
								}
								//Appending a new tuple topic/subscriber to the list.

								aux1->next=aux2;
								}

								if(send(sc, &return_value, sizeof(int),0)==-1){
									printf("Error on sending.\n");
									exit(0);
								}
							action_rcv=2;
						}
						else{//UNSUBSCRIBE
							if(empty){
								res=1;//TOPIC NOT FOUND
							}
							else{
								while(aux1->next!=NULL){
									if(!strcmp(aux1->topic,action)){
										existing_topic=1;
									}
									aux1=aux1->next;
								}
								if(!strcmp(aux1->topic,action)){
									existing_topic=1;
								}
								if(!existing_topic){
								 aux1->next=aux2;
							 }
							}
						action_rcv=2;
						}
			}

		 else if(action_rcv==2){
			 //topic received
			 action_rcv=3;
			 printf("TEXT: %s\n",action);
		 }


		 print_topics();
 }
/*
	res=0;


	if(send(sc, &res, sizeof(int),0)==-1){
		printf("Error on sending.\n");
		exit(0);
	}
*/
}
	close(sd);
	return 0;
}
