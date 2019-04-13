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

struct node * head=NULL;
int empty=1;
int existing_topic=0;
int res;

void print_list(){
	if(head==NULL) empty=1;
	if(!empty){
		printf("Printing the current list of topics.\n");
		struct node * aux1=head;
		printf("TOPIC: %s\n", aux1->topic);
		printf("ADDRESS: %s\n", aux1->addr);
		while(aux1->next!=NULL){
			printf("TOPIC: %s\n", aux1->next->topic);
			printf("ADDRESS: %s\n", aux1->next->addr);
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
	char topic[128]="";
	char text[1024]="";
	struct sockaddr_in broker_addr, editor_addr;
	struct sockaddr_storage addr;

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

	while(1){

	struct node * aux1=head;

	struct node * aux2=malloc(sizeof(struct node));
	strcpy(aux2->topic, "");
	aux2->next=NULL;


	existing_topic=0;

	action_rcv=0;

	printf("Waiting for action\n");
	 if((sc=accept(sd,(struct sockaddr *)&editor_addr,&size))<0){
		 perror("Error on accepting connection.\n");
		 exit(0);
	 }

	 while(action_rcv!=3){
		 if(action_rcv==0){
				if(readLine(sc, action, 1024)<0){
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
												printf("Unknown action received\n");
				 								action_rcv=3;
				 			}
					}

		 if(action_rcv==1){
			 printf("Waiting for topic\n");
			 if(readLine(sc, topic, 128)<0){
					perror("Error on receiving.\n");
					exit(0);
				}
			 //topic received
			 printf("TOPIC: %s\n",topic);
						 if(action_type==0){//PUBLISH action
							 if(!empty){
								 aux1=head;
								 while(aux1->next!=NULL){
									 if(!strcmp(aux1->topic,topic)){
										 printf("Found a subscriber to the received topic\n");
										 struct sockaddr_in sub_addr;
										 int st;
										 if((st=socket(AF_INET,SOCK_STREAM,6))==-1){ //st for send text
									 		printf("Error in creating socket.\n");
											}
											int v=1;
									 	 	setsockopt(st,SOL_SOCKET, SO_REUSEADDR, (char*) &v, sizeof(int));

											bzero((char *) &sub_addr, sizeof(sub_addr));

											memcpy(&(sub_addr.sin_addr), aux2->addr, sizeof(aux2->addr));
											sub_addr.sin_family=AF_INET;
											sub_addr.sin_port=htons(port_n);

											printf("Got here!\n");
											if(connect(st, (struct sockaddr *) &sub_addr, sizeof(sub_addr))==-1){
												printf("Error in the connection to the subscriptor\n");
												exit(0);
											}
											printf("Got after connect!\n");

											close(st);

										 //Deliver text to the subscriptor of that node of the list.
									 }
									 aux1=aux1->next;
								 }
								 if(!strcmp(aux1->topic,topic)){
									 printf("Found a subscriber to the received topic\n");
									 struct sockaddr_in sub_addr;
									 int st;
									 if((st=socket(AF_INET,SOCK_STREAM,6))==-1){ //st for send text
										printf("Error in creating socket.\n");
										}
										int v=1;
										setsockopt(st,SOL_SOCKET, SO_REUSEADDR, (char*) &v, sizeof(int));

										bzero((char *) &sub_addr, sizeof(sub_addr));

										memcpy(&(sub_addr.sin_addr), aux2->addr, sizeof(aux2->addr));
										sub_addr.sin_family=AF_INET;
										sub_addr.sin_port=htons(port_n);

										printf("Got here!\n");
										if(connect(st, (struct sockaddr *) &sub_addr, sizeof(sub_addr))==-1){
											printf("Error in the connection to the subscriptor\n");
											exit(0);
										}
										printf("Got after connect!\n");

										close(st);

									 //Deliver text to the subscriptor of that node of the list (for the last element of the list).
								 }
							 }

						 action_rcv=2;
					 	}
					 	else if(action_type==1){//SUBSCRIBE
			 			 	strcpy(aux2->topic, topic);
							getpeername(sc,(struct sockaddr*)&addr,&size);
							struct sockaddr_in *s=(struct sockaddr_in*)&addr;
							inet_ntop(AF_INET,&s->sin_addr,aux2->addr,sizeof aux2->addr);
							int return_value=0;
							if(empty){
								head=aux2;
								return_value=0;
								empty=0;
							}
							else{
								if(strcmp(head->addr,aux2->addr)==0&&strcmp(head->topic,aux2->topic)==0){
									return_value=1;
								}
								else{

									while(aux1->next!=NULL&&(strcmp(aux1->addr,aux2->addr)!=0||strcmp(aux1->topic,aux2->topic)!=0)){
										aux1=aux1->next;
									}
									if(strcmp(aux1->addr,aux2->addr)==0&&strcmp(aux1->topic,aux2->topic)==0){ //SUBSCRIPTION already EXISTS
										return_value=1;

									}
									else{
									//Appending a new tuple topic/subscriber to the list.
									aux1->next=aux2;

								}
							}
						}
							empty=0;

							if(send(sc, &return_value, sizeof(int),0)==-1){
								printf("Error on sending.\n");
								exit(0);
							}
							action_rcv=3;
						}
						else{//UNSUBSCRIBE

							strcpy(aux2->topic, topic);
							getpeername(sc,(struct sockaddr*)&addr,&size);
							struct sockaddr_in *s=(struct sockaddr_in*)&addr;
							inet_ntop(AF_INET,&s->sin_addr,aux2->addr,sizeof aux2->addr);
							if(empty){
								res=1;//TOPIC NOT FOUND
							}
							else{
								if(!strcmp(head->topic,topic)&&!strcmp(head->addr,aux2->addr)){//CHECKING HEAD
									head=head->next;
									res=0;
								}
								else{
									while(aux1!=NULL&&(strcmp(aux1->topic,topic)!=0||strcmp(aux1->addr,aux2->addr)!=0)){
										aux2=aux1;
										aux1=aux1->next;
									}
									if(aux1==NULL){
										res=1;//TOPIC NOT FOUND
									}
									else {
									aux2->next=aux1->next;//change pointers
									res=0;
									}
								}
						}
							if(send(sc, &res, sizeof(int),0)==-1){
								printf("Error on sending.\n");
								exit(0);
							}
						action_rcv=3;
						}
			}

		 else if(action_rcv==2){
			 //topic received
			 action_rcv=3;
			 printf("Waiting for text\n");
			 if(readLine(sc, text, 1024)<0){
 				 perror("Error on receiving.\n");
 				 exit(0);
 			 }
			 printf("TEXT: %s\n",text);
		 }

		 if(action_rcv==3){
			 print_list();
			 printf("--------------------------\n");
		 }
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
