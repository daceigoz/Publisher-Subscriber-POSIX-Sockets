#include <unistd.h>

void init_service(char *host);
void get_rpc_publication(char * topic, char * buffer, char * host);
void set_rpc_publication(char * topic, char * text, char * host);
