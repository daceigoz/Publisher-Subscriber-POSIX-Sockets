/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "publications.h"
#include <stdio.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>

#ifndef SIG_PF
#define SIG_PF void(*)(int)
#endif

int
_init_publication_1 (void  *argp, void *result, struct svc_req *rqstp)
{
	return (init_publication_1_svc(result, rqstp));
}

int
_get_publication_1 (char * *argp, void *result, struct svc_req *rqstp)
{
	return (get_publication_1_svc(*argp, result, rqstp));
}

int
_set_publication_1 (set_publication_1_argument *argp, void *result, struct svc_req *rqstp)
{
	return (set_publication_1_svc(argp->arg1, argp->arg2, result, rqstp));
}

static void
publications_1(struct svc_req *rqstp, register SVCXPRT *transp)
{
	union {
		char *get_publication_1_arg;
		set_publication_1_argument set_publication_1_arg;
	} argument;
	union {
		int init_publication_1_res;
		char *get_publication_1_res;
		int set_publication_1_res;
	} result;
	bool_t retval;
	xdrproc_t _xdr_argument, _xdr_result;
	bool_t (*local)(char *, void *, struct svc_req *);

	switch (rqstp->rq_proc) {
	case NULLPROC:
		(void) svc_sendreply (transp, (xdrproc_t) xdr_void, (char *)NULL);
		return;

	case INIT_PUBLICATION:
		_xdr_argument = (xdrproc_t) xdr_void;
		_xdr_result = (xdrproc_t) xdr_int;
		local = (bool_t (*) (char *, void *,  struct svc_req *))_init_publication_1;
		break;

	case GET_PUBLICATION:
		_xdr_argument = (xdrproc_t) xdr_wrapstring;
		_xdr_result = (xdrproc_t) xdr_wrapstring;
		local = (bool_t (*) (char *, void *,  struct svc_req *))_get_publication_1;
		break;

	case SET_PUBLICATION:
		_xdr_argument = (xdrproc_t) xdr_set_publication_1_argument;
		_xdr_result = (xdrproc_t) xdr_int;
		local = (bool_t (*) (char *, void *,  struct svc_req *))_set_publication_1;
		break;

	default:
		svcerr_noproc (transp);
		return;
	}
	memset ((char *)&argument, 0, sizeof (argument));
	if (!svc_getargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		svcerr_decode (transp);
		return;
	}
	retval = (bool_t) (*local)((char *)&argument, (void *)&result, rqstp);
	if (retval > 0 && !svc_sendreply(transp, (xdrproc_t) _xdr_result, (char *)&result)) {
		svcerr_systemerr (transp);
	}
	if (!svc_freeargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		fprintf (stderr, "%s", "unable to free arguments");
		exit (1);
	}
	if (!publications_1_freeresult (transp, _xdr_result, (caddr_t) &result))
		fprintf (stderr, "%s", "unable to free results");

	return;
}

int
main (int argc, char **argv)
{
	register SVCXPRT *transp;

	pmap_unset (PUBLICATIONS, PUBLICATIONSVER);

	transp = svcudp_create(RPC_ANYSOCK);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create udp service.");
		exit(1);
	}
	if (!svc_register(transp, PUBLICATIONS, PUBLICATIONSVER, publications_1, IPPROTO_UDP)) {
		fprintf (stderr, "%s", "unable to register (PUBLICATIONS, PUBLICATIONSVER, udp).");
		exit(1);
	}

	transp = svctcp_create(RPC_ANYSOCK, 0, 0);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create tcp service.");
		exit(1);
	}
	if (!svc_register(transp, PUBLICATIONS, PUBLICATIONSVER, publications_1, IPPROTO_TCP)) {
		fprintf (stderr, "%s", "unable to register (PUBLICATIONS, PUBLICATIONSVER, tcp).");
		exit(1);
	}

	svc_run ();
	fprintf (stderr, "%s", "svc_run returned");
	exit (1);
	/* NOTREACHED */
}