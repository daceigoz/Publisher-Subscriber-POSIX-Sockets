/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "publications.h"


void
publications_1(char *host)
{
	CLIENT *clnt;
	enum clnt_stat retval_1;
	int result_1;
	enum clnt_stat retval_2;
	char *result_2;
	char *get_publication_1_arg1;
	enum clnt_stat retval_3;
	int result_3;
	char *set_publication_1_arg1;
	char *set_publication_1_arg2;

#ifndef	DEBUG
	clnt = clnt_create (host, PUBLICATIONS, PUBLICATIONSVER, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	/* DEBUG */

	retval_1 = init_publication_1(&result_1, clnt);
	if (retval_1 != RPC_SUCCESS) {
		clnt_perror (clnt, "call failed");
	}
	retval_2 = get_publication_1(get_publication_1_arg1, &result_2, clnt);
	if (retval_2 != RPC_SUCCESS) {
		clnt_perror (clnt, "call failed");
	}
	retval_3 = set_publication_1(set_publication_1_arg1, set_publication_1_arg2, &result_3, clnt);
	if (retval_3 != RPC_SUCCESS) {
		clnt_perror (clnt, "call failed");
	}
#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
}


int
main (int argc, char *argv[])
{
	char *host;

	if (argc < 2) {
		printf ("usage: %s server_host\n", argv[0]);
		exit (1);
	}
	host = argv[1];
	publications_1 (host);
exit (0);
}
