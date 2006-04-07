/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include <memory.h> /* for memset */
#include "gateway_prot.h"

/* Default timeout can be changed using clnt_control() */
static struct timeval TIMEOUT = { 25, 0 };

void *
bamboo_dht_proc_null_2(void *argp, CLIENT *clnt)
{
	static char clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, BAMBOO_DHT_PROC_NULL,
		(xdrproc_t) xdr_void, (caddr_t) argp,
		(xdrproc_t) xdr_void, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return ((void *)&clnt_res);
}

bamboo_stat *
bamboo_dht_proc_put_2(bamboo_put_args *argp, CLIENT *clnt)
{
	static bamboo_stat clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, BAMBOO_DHT_PROC_PUT,
		(xdrproc_t) xdr_bamboo_put_args, (caddr_t) argp,
		(xdrproc_t) xdr_bamboo_stat, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

bamboo_get_res *
bamboo_dht_proc_get_2(bamboo_get_args *argp, CLIENT *clnt)
{
	static bamboo_get_res clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, BAMBOO_DHT_PROC_GET,
		(xdrproc_t) xdr_bamboo_get_args, (caddr_t) argp,
		(xdrproc_t) xdr_bamboo_get_res, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}
