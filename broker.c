#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include "lines.h"
#include "node_t.h"
#include "rpc_functions.h"

struct node * head=NULL; //Header of the list of subscriptions stored.
int empty=1; //Variable defining whether the list is empty (1) of has contents (0)
int res; //Value to be returned on the thread, whose content will be 0 if it worked fine or a different value if a problem was encountered.


/*
Defining the control measures: 2 mutexes, one for copying the input data in a thread (is where the condition variable will be used)
and the other one for giving access to the data services of the broker to only one thread at a time.
*/
pthread_mutex_t mutex;
pthread_mutex_t mutex_msg;
int msg_not_copied=1;
pthread_cond_t cond_msg;

//Initializing the strings where received strings will be stored:
char port[256]= "";
char action[1024]= "";
char topic[128]="";
char text[1024]="";

int port_n;
struct sockaddr_storage addr;


//Function we made for printing the list of subscriptions in the broker:
void print_list(){
	if(head==NULL) empty=1;
	if(!empty){
		printf("\nPrinting the current list of subscriptions:\n");
		struct node * aux1=head;
		printf("TOPIC: %s\n", aux1->topic);
		printf("ADDRESS: %s\n", aux1->addr);
		printf("PORT: %d\n", aux1->port);
		while(aux1->next!=NULL){
			printf("TOPIC: %s\n", aux1->next->topic);
			printf("ADDRESS: %s\n", aux1->next->addr);
			printf("PORT: %d\n", aux1->next->port);
			aux1=aux1->next;
		}
	}
}


//Function to be executed as a thread whenever a connection to the broker is requested:
void * socketThread(void *arg){

		//Obtaining a copy of the socket used to avoid a concurrency problem:
		//we have used the same code regarding mutexes that we initially made for the POSIX queues project.

		pthread_mutex_lock(&mutex_msg);
	  int newSocket = *((int *)arg);
	  msg_not_copied = 0;
	  pthread_cond_signal(&cond_msg);
		pthread_mutex_unlock(&mutex_msg);

		//Now we will make use of another mutex to avoid having multiple threads interacting with the data structures at the same time:
		pthread_mutex_lock(&mutex);

		/*Resetting the values of the two auxiliary nodes of the list that we will use for different purposes:
		aux1 will be used for traversing the list for comparisons only while aux2 will be used to add and remove nodes, connecting their previous
		and next node.*/
		struct node * aux1=head;
		struct node * aux2=malloc(sizeof(struct node));
		strcpy(aux2->topic, "");
		strcpy(aux2->addr, "");
		aux2->next=NULL;

		int action_rcv=0; //Value used for structuring the message reciving loop.
		int action_type=0; //Will define which kind of action must be performed (PUBLISH, SUBSCRIBE or UNSUBSCRIBE)




		 while(action_rcv!=3){

			 //In this case no message has yet been received.
			 if(action_rcv==0){

				 //Obtaining the message containing the action to be performed with the data sent.
					if(readLine(newSocket, action, 1024)<0){
						 perror("Error on receiving.\n");
						 pthread_mutex_unlock(&mutex);
						 exit(0);
					 }
					 			//Setting the value of the action type depending on the action string received.
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
													printf("Unknown action received.\n");
					 								action_rcv=3;
					 			}
						}

			 if(action_rcv==1){

				 //Obtaining the first message, which will be the topic for any possible type of client.
				 if(readLine(newSocket, topic, 128)<0){
						perror("Error on receiving.\n");
						pthread_mutex_unlock(&mutex);
						exit(0);
					}
				 printf("TOPIC: %s\n",topic);



				 			////////////////////PUBLISH ACTION////////////////////
							 if(action_type==0){

								 //Now the text from the editor will be received.
								 if(readLine(newSocket, text, 1024)<0){
										perror("Error on receiving.\n");
										pthread_mutex_unlock(&mutex);
										exit(0);
									}

									printf("TEXT: %s\n",text);

									//Using the RPC to store the publication:
									set_rpc_publication(topic, text, "localhost");

									//Now the list will be traversed to find which susbscriptors must received the published message:
								 if(!empty){
									 aux1=head;
									 while(aux1->next!=NULL){
										 if(!strcmp(aux1->topic,topic)){

											 //Initiating a connection with the subscriber sending the data:
											 struct sockaddr_in sub_addr;
											 int st;
											 if((st=socket(AF_INET,SOCK_STREAM,0))==-1){ //st for send text
										 		printf("Error in creating socket.\n");
												pthread_mutex_unlock(&mutex);
												exit(0);
												}
												int v=1;
										 	 	setsockopt(st,SOL_SOCKET, SO_REUSEADDR, (char*) &v, sizeof(int));

												bzero(&sub_addr, sizeof(sub_addr));


												sub_addr.sin_addr.s_addr=inet_addr(aux1->addr);
												sub_addr.sin_family=AF_INET;
												sub_addr.sin_port=htons(aux1->port);

												if(connect(st, (struct sockaddr *) &sub_addr, sizeof(sub_addr))==-1){
													printf("Error in the connection to the subscriptor.\n");
													pthread_mutex_unlock(&mutex);
													exit(0);
												}
											/*	//Delivering the topic to the subscriber:
												if(send(st, &topic, sizeof(topic),0)==-1){
													printf("Error on sending the topic.\n");
													pthread_mutex_unlock(&mutex);
													exit(0);
												}*/
												//Delivering the text to the subscriber:
												if(send(st, &text, sizeof(text),0)==-1){
													printf("Error on sending the text.\n");
													pthread_mutex_unlock(&mutex);
													exit(0);
												}

												close(st);
										 }
										 aux1=aux1->next;
									 }
									 //This will be done for the last member of the list, since its next value will be NULL and will not be included in the previous loop:
									 //The contents are the exact same as in the loop above.
									 if(!strcmp(aux1->topic,topic)){

										 struct sockaddr_in sub_addr;
										 int st;
										 if((st=socket(AF_INET,SOCK_STREAM,0))==-1){ //st for send text
											printf("Error in creating socket.\n");
											pthread_mutex_unlock(&mutex);
											exit(0);
											}
											int v=1;
											setsockopt(st,SOL_SOCKET, SO_REUSEADDR, (char*) &v, sizeof(int));

											bzero(&sub_addr, sizeof(sub_addr));

											sub_addr.sin_addr.s_addr=inet_addr(aux1->addr);
											sub_addr.sin_family=AF_INET;
											sub_addr.sin_port=htons(aux1->port);

											if(connect(st, (struct sockaddr *) &sub_addr, sizeof(sub_addr))==-1){
												printf("Error in the connection to the subscriptor.\n");
												pthread_mutex_unlock(&mutex);
												exit(0);
											}

											if(send(st, &text, sizeof(text),0)==-1){
												printf("Error on sending text.\n");
												pthread_mutex_unlock(&mutex);
												exit(0);
											}

											close(st);

										 //Deliver text to the subscriptor of that node of the list (for the last element of the list).
									 }
								 }

							 action_rcv=3;
						 	}



							////////////////////SUBSCRIBE ACTION////////////////////
						 	else if(action_type==1){

								/*At this point the only remaining message to be receive by the subscriptor is the port where it will be
								listening for messages of the subscription topic*/
								char myport[128];
								int port=0;
								//Reading the port:
								if(readLine(newSocket, myport, 128)<0){
									 perror("Error on receiving.\n");
									 pthread_mutex_unlock(&mutex);
									 exit(0);
								 }

								 port=atoi(myport);

								 if(port<0){
									 printf("Error receiving the port of the subscriber.\n");
									 pthread_mutex_unlock(&mutex);
									 exit(0);
								 }

								 //Storing the data of the subscriber into an auxiliary node to store it in the list of subscriptions:
				 			 	strcpy(aux2->topic, topic);
								struct sockaddr_in *s=(struct sockaddr_in*)&addr;
								aux2->port=port;
								inet_ntop(AF_INET,&s->sin_addr,aux2->addr,sizeof aux2->addr);

								printf("IP of the subscriber: %s\n", aux2->addr);
								printf("Port of the subscriber: %d\n", port);

								//Storing the auxiliary node into the list:
								int return_value=0;
								if(empty){
									head=aux2;
									return_value=0;
									empty=0;
								}
								else{
									//we also check if there is an equivalent node, so that it is not included:
									if(strcmp(head->addr,aux2->addr)==0&&(strcmp(head->topic,aux2->topic)==0)){
										return_value=1;
									}
									else{

										while(aux1->next!=NULL&&(strcmp(aux1->addr,aux2->addr)!=0||strcmp(aux1->topic,aux2->topic)!=0)){
											aux1=aux1->next;
										}
										if(!strcmp(aux1->addr,aux2->addr)&&(strcmp(aux1->topic,aux2->topic)==0)){ //SUBSCRIPTION already EXISTS
											return_value=1;
										}
										else{
											//Finally appending the node to the list:
											aux1->next=aux2;

									}
								}
							}
								empty=0;

								//Returning the result of the SUBSCRIBE operation to the client:
								if(send(newSocket, &return_value, sizeof(int),0)==-1){
									printf("Error on sending.\n");
									pthread_mutex_unlock(&mutex);
									exit(0);
								}

								//Calling the RPC function for obtaining the last text from the subscribed topic:

								char get_buffer[1024];
								bzero(get_buffer, sizeof(get_buffer));
								get_rpc_publication(topic, get_buffer, "localhost");
								printf("Value of get_buffer: %s\n", get_buffer);
								if(get_buffer!=NULL){
								if(send(newSocket, &get_buffer, sizeof(get_buffer),0)==-1){
									printf("Error on sending.\n");
									pthread_mutex_unlock(&mutex);
									exit(0);
								}
							}
								action_rcv=3;
							}



							////////////////////UNSUBSCRIBE ACTION////////////////////
							else if(action_type==2){

								//Searching in the list to unsubscribe the client from the given topic:
								if(empty){
									res=1;//TOPIC NOT FOUND
								}
								else{
									if(!strcmp(head->topic,topic)&&(!strcmp(head->addr,aux2->addr))){//CHECKING HEAD
										head=head->next;
										res=0;
									}
									else{
										while(aux1!=NULL&&((strcmp(aux1->topic,topic)!=0||strcmp(aux1->addr,aux2->addr)!=0))){
											aux2=aux1;
											aux1=aux1->next;
										}
										if(aux1==NULL){
											res=1;//TOPIC NOT FOUND
										}
										else {
										aux2->next=aux1->next;//changing the pointers (next of the removed node is the next of its prev node)
										res=0;
										}
									}
							}
							//Finally the result of the UNSUBSCRIBE call is sent back to the client:
								if(send(newSocket, &res, sizeof(int),0)==-1){
									printf("Error on sending.\n");
									pthread_mutex_unlock(&mutex);
									exit(0);
								}
							action_rcv=3;
							}
				}

			 if(action_rcv==3){
				 print_list();
				 printf("--------------------------\n"); //Separator
			 }

	}
close(newSocket); //Closing the socket used for this connection.
pthread_mutex_unlock(&mutex); //Freeing the access to other threads to handle different calls.
pthread_exit(NULL);
}


void print_usage() {
	    printf("Usage: broker -p port \n");
}

int main(int argc, char *argv[]) {
	int  option = 0;
	int val, sc,sd;
	unsigned int size;
	pthread_t tid[60];
	int i = 0;

	struct sockaddr_in broker_addr;


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

	 port_n=atoi(port);
	 broker_addr.sin_family = AF_INET;
   	 broker_addr.sin_addr.s_addr = INADDR_ANY;
  	 broker_addr.sin_port = htons(port_n);

	 if(bind(sd, (struct sockaddr *)&broker_addr, sizeof(broker_addr))<0){
		 perror("Error on socket bind.\n");
		 exit(0);
	 }

	 if(listen(sd, 50)==-1){
		 perror("Error on socket listen.\n");
	 }

	size=sizeof(addr);

	//Initiating the mutexes and condition variables to be used before entering the main loop:

	pthread_mutex_init(&mutex, NULL);
  pthread_mutex_init(&mutex_msg, NULL);
	pthread_cond_init(&cond_msg, NULL);

	//Initializing the RPC service:
	init_service("localhost");

	while(1){

		msg_not_copied=1;

		 if((sc=accept(sd,(struct sockaddr *)&addr,&size))<0){
			 perror("Error on accepting connection.\n");
			 exit(0);
		 }

		 if(pthread_create(&tid[i], NULL, socketThread, &sc) != 0 )
		 printf("Failed to create thread.\n");

		 //Waiting until the thread has properly obtained the connected socket data:
		 pthread_mutex_lock(&mutex_msg);
		 while (msg_not_copied){
	      pthread_cond_wait(&cond_msg, &mutex_msg);
		    msg_not_copied = 0;
		    pthread_mutex_unlock(&mutex_msg);
		}

		 if( i >= 50){
			 i = 0;
				while(i < 50){
					pthread_join(tid[i++],NULL);
				}
				i = 0;
			}
		}
		close(sd);
		return 0;
}
