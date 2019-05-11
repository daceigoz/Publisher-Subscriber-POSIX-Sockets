/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _PUBLICATIONS_H_RPCGEN
#define _PUBLICATIONS_H_RPCGEN

#include <rpc/rpc.h>

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif


struct get_publication_1_argument {
	char *arg1;
	char *arg2;
};
typedef struct get_publication_1_argument get_publication_1_argument;

struct set_publication_1_argument {
	char *arg1;
	char *arg2;
};
typedef struct set_publication_1_argument set_publication_1_argument;

#define PUBLICATIONS 99
#define PUBLICATIONSVER 1

#if defined(__STDC__) || defined(__cplusplus)
#define INIT_PUBLICATION 1
extern  enum clnt_stat init_publication_1(int *, CLIENT *);
extern  bool_t init_publication_1_svc(int *, struct svc_req *);
#define GET_PUBLICATION 2
extern  enum clnt_stat get_publication_1(char *, char *, int *, CLIENT *);
extern  bool_t get_publication_1_svc(char *, char *, int *, struct svc_req *);
#define SET_PUBLICATION 3
extern  enum clnt_stat set_publication_1(char *, char *, int *, CLIENT *);
extern  bool_t set_publication_1_svc(char *, char *, int *, struct svc_req *);
extern int publications_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define INIT_PUBLICATION 1
extern  enum clnt_stat init_publication_1();
extern  bool_t init_publication_1_svc();
#define GET_PUBLICATION 2
extern  enum clnt_stat get_publication_1();
extern  bool_t get_publication_1_svc();
#define SET_PUBLICATION 3
extern  enum clnt_stat set_publication_1();
extern  bool_t set_publication_1_svc();
extern int publications_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_get_publication_1_argument (XDR *, get_publication_1_argument*);
extern  bool_t xdr_set_publication_1_argument (XDR *, set_publication_1_argument*);

#else /* K&R C */
extern bool_t xdr_get_publication_1_argument ();
extern bool_t xdr_set_publication_1_argument ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_PUBLICATIONS_H_RPCGEN */
