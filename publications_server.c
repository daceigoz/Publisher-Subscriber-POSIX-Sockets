/*
This code will contain the functionality of the server side of the RPC, and will be the one to manage
the storage of the files containing the name "topic" and the content "text".
*/

 #include <unistd.h>
 #include <sys/stat.h>
 #include <fcntl.h>

 #include "publications.h"

bool_t
init_publication_1_svc(int *result, struct svc_req *rqstp) //This function will generate (if not existing) the directory for storage:
{

	printf("INIT\n");
	bool_t retval=1;

  //Generating the directory to store the files (if the call is an error, there would be no problem since the directory already exists).
	 mkdir("./publications", 0777);

	 printf("Initialization of the publication storage system was successful\n");
	 printf("--------------------------\n"); //Separator

	return retval;
}

bool_t
get_publication_1_svc(char *topic, char **result,  struct svc_req *rqstp) //This function will obtain the text from a given topic and write it into the input buffer:
{
	bool_t retval=1;

	printf("GET\n");

  //Allocating memory for the returning buffer:
	*result=(char *)malloc(1024);

	printf("The topic received was: %s\n", topic);

  //Defining the path of the file to be read from, which will have the name of the received topic:
	 char * file_path=(char *)malloc(strlen("./publications/")+strlen(topic));

	 sprintf(file_path, "%s%s", "./publications/",topic);
	 printf("Value of file path: %s\n", file_path);

	 //Creating a temporal buffer to store the read contents:
	 char temp_buf[1024];
	 bzero(temp_buf, sizeof(temp_buf));

   //Opening the file and getting its file descriptor:
	 int fd=open(file_path, O_RDONLY);
	 printf("Got after open\n");
	 if(fd==-1){
		 printf("Error opening the file or there is no file for the given topic.\n");
	 }
   //Using the file descriptor to read the file:
	 else{
		 if((read(fd, &temp_buf, 1024)==-1)){
			 printf("Error reading the file.\n");
		 }
		 printf("Value on the buffer: %s\n", temp_buf);
		 if(((close(fd)))==-1){
			 printf("Error closing the file descriptor.\n");
		 }
	 }

   //Copying the read values from the temporal buffer into the request buffer:
	 strcpy(*result, temp_buf);
	 printf("--------------------------\n"); //Separator

	return retval;
}

bool_t
set_publication_1_svc(char *topic, char *text, int *result,  struct svc_req *rqstp) //Function to generate a file with the topic and buffer:
{
	bool_t retval=1;

	printf("SET\n");

	 printf("The topic received was: %s\n", topic);
	 printf("The text received was: %s\n", text);

   //Defining the path of the file to be read from, which will have the name of the received topic:
	 char file_path[strlen("./publications/")+strlen(topic)];
	 sprintf(file_path, "%s%s", "./publications/",topic);
	 printf("Value of filepath: %s\n", file_path);
   //Opening the file:
	 int fd=open(file_path, O_CREAT|O_RDWR|O_TRUNC, 0777);
	 if(fd==-1){
		 printf("The file cannot be created.\n");
	 }
   //Writing into the file with an auxiliary buffer:
	 else{
		 char write_buf[1024];
		 bzero(write_buf, sizeof(write_buf));
		 strcpy(write_buf, text);
		 if(((write(fd, write_buf, 1024)))==-1){
			 printf("The topic cannot be written.\n");
		 }
		 if(((close(fd)))==-1){
			 printf("Error closing the file descriptor.\n");
		 }
	 }
	 printf("--------------------------\n"); //Separator
	return retval;
}

int
publications_1_freeresult (SVCXPRT *transp, xdrproc_t xdr_result, caddr_t result)
{
	xdr_free (xdr_result, result);

	return 1;
}
