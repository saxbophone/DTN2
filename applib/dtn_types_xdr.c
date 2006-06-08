/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "dtn_types.h"
/*
 * IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
 * By downloading, copying, installing or using the software you agree
 * to this license. If you do not agree to this license, do not
 * download, install, copy or use the software.
 * 
 * Intel Open Source License 
 * 
 * Copyright (c) 2004 Intel Corporation. All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 *   Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * 
 *   Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the
 *   distribution.
 * 
 *   Neither the name of the Intel Corporation nor the names of its
 *   contributors may be used to endorse or promote products derived
 *   from * this software without specific prior written permission.
 *  
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * INTEL OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**********************************
 * This file defines the types used in the DTN client API. The structures are
 * autogenerated using rpcgen, as are the marshalling and unmarshalling
 * XDR routines.
 */

#include <limits.h>
#ifndef ARG_MAX
#define ARG_MAX _POSIX_ARG_MAX
#endif

#include <rpc/rpc.h>


/**********************************
 * Constants.
 * (Note that we use #defines to get the comments as well)
 */
#define DTN_MAX_ENDPOINT_ID 256 /* max endpoint_id size (bytes) */
#define DTN_MAX_PATH_LEN PATH_MAX /* max path length */
#define DTN_MAX_EXEC_LEN ARG_MAX /* length of string passed to exec() */
#define DTN_MAX_AUTHDATA 1024 /* length of auth/security data*/
#define DTN_MAX_REGION_LEN 64 /* 64 chars "should" be long enough */
#define DTN_MAX_BUNDLE_MEM 50000 /* biggest in-memory bundle is ~50K*/

/**
 * Specification of a dtn endpoint id, i.e. a URI, implemented as a
 * fixed-length char buffer. Note that for efficiency reasons, this
 * fixed length is relatively small (256 bytes). 
 * 
 * The alternative is to use the string XDR type but then all endpoint
 * ids would require malloc / free which is more prone to leaks / bugs.
 */

bool_t
xdr_dtn_endpoint_id_t (XDR *xdrs, dtn_endpoint_id_t *objp)
{
	register int32_t *buf;

	int i;
	 if (!xdr_opaque (xdrs, objp->uri, DTN_MAX_ENDPOINT_ID))
		 return FALSE;
	return TRUE;
}

/**
 * A registration cookie.
 */

bool_t
xdr_dtn_reg_id_t (XDR *xdrs, dtn_reg_id_t *objp)
{
	register int32_t *buf;

	 if (!xdr_u_int (xdrs, objp))
		 return FALSE;
	return TRUE;
}

/**
 * DTN timeouts are specified in seconds.
 */

bool_t
xdr_dtn_timeval_t (XDR *xdrs, dtn_timeval_t *objp)
{
	register int32_t *buf;

	 if (!xdr_u_int (xdrs, objp))
		 return FALSE;
	return TRUE;
}

/**
 * An infinite wait is a timeout of -1.
 */
#define DTN_TIMEOUT_INF ((dtn_timeval_t)-1)

/**
 * Specification of a service tag used in building a local endpoint
 * identifier.
 * 
 * Note that the application cannot (in general) expect to be able to
 * use the full DTN_MAX_ENDPOINT_ID, as there is a chance of overflow
 * when the daemon concats the tag with its own local endpoint id.
 */

bool_t
xdr_dtn_service_tag_t (XDR *xdrs, dtn_service_tag_t *objp)
{
	register int32_t *buf;

	int i;
	 if (!xdr_vector (xdrs, (char *)objp->tag, DTN_MAX_ENDPOINT_ID,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	return TRUE;
}

/**
 * Value for an unspecified registration cookie (i.e. indication that
 * the daemon should allocate a new unique id).
 */

/**
 * Registration delivery failure actions
 *     DTN_REG_DROP   - drop bundle if registration not active
 *     DTN_REG_DEFER  - spool bundle for later retrieval
 *     DTN_REG_EXEC   - exec program on bundle arrival
 */

bool_t
xdr_dtn_reg_failure_action_t (XDR *xdrs, dtn_reg_failure_action_t *objp)
{
	register int32_t *buf;

	 if (!xdr_enum (xdrs, (enum_t *) objp))
		 return FALSE;
	return TRUE;
}

/**
 * Registration state.
 */

bool_t
xdr_dtn_reg_info_t (XDR *xdrs, dtn_reg_info_t *objp)
{
	register int32_t *buf;

	 if (!xdr_dtn_endpoint_id_t (xdrs, &objp->endpoint))
		 return FALSE;
	 if (!xdr_dtn_reg_id_t (xdrs, &objp->regid))
		 return FALSE;
	 if (!xdr_dtn_reg_failure_action_t (xdrs, &objp->failure_action))
		 return FALSE;
	 if (!xdr_dtn_timeval_t (xdrs, &objp->expiration))
		 return FALSE;
	 if (!xdr_bool (xdrs, &objp->init_passive))
		 return FALSE;
	 if (!xdr_bytes (xdrs, (char **)&objp->script.script_val, (u_int *) &objp->script.script_len, DTN_MAX_EXEC_LEN))
		 return FALSE;
	return TRUE;
}

/**
 * Bundle priority specifier.
 *     COS_BULK      - lowest priority
 *     COS_NORMAL    - regular priority
 *     COS_EXPEDITED - important
 *     COS_RESERVED  - TBD
 */

bool_t
xdr_dtn_bundle_priority_t (XDR *xdrs, dtn_bundle_priority_t *objp)
{
	register int32_t *buf;

	 if (!xdr_enum (xdrs, (enum_t *) objp))
		 return FALSE;
	return TRUE;
}

/**
 * Bundle delivery option flags. Note that multiple options
 * may be selected for a given bundle.
 *     
 *     DOPTS_NONE           - no custody, etc
 *     DOPTS_CUSTODY        - custody xfer
 *     DOPTS_DELIVERY_RCPT  - end to end delivery (i.e. return receipt)
 *     DOPTS_RECEIVE_RCPT   - per hop arrival receipt
 *     DOPTS_FORWARD_RCPT   - per hop departure receipt
 *     DOPTS_CUSTODY_RCPT   - per custodian receipt
 *     DOPTS_DELETE_RCPT    - request deletion receipt
 */

bool_t
xdr_dtn_bundle_delivery_opts_t (XDR *xdrs, dtn_bundle_delivery_opts_t *objp)
{
	register int32_t *buf;

	 if (!xdr_enum (xdrs, (enum_t *) objp))
		 return FALSE;
	return TRUE;
}

/**
 * Bundle metadata.
 */

bool_t
xdr_dtn_bundle_spec_t (XDR *xdrs, dtn_bundle_spec_t *objp)
{
	register int32_t *buf;

	 if (!xdr_dtn_endpoint_id_t (xdrs, &objp->source))
		 return FALSE;
	 if (!xdr_dtn_endpoint_id_t (xdrs, &objp->dest))
		 return FALSE;
	 if (!xdr_dtn_endpoint_id_t (xdrs, &objp->replyto))
		 return FALSE;
	 if (!xdr_dtn_bundle_priority_t (xdrs, &objp->priority))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->dopts))
		 return FALSE;
	 if (!xdr_dtn_timeval_t (xdrs, &objp->expiration))
		 return FALSE;
	return TRUE;
}

/**
 * The payload of a bundle can be sent or received either in a file,
 * in which case the payload structure contains the filename, or in
 * memory where the struct has the actual data.
 *
 * Note that there is a limit (DTN_MAX_BUNDLE_MEM) on the maximum size
 * bundle payload that can be sent or received in memory.
 */

bool_t
xdr_dtn_bundle_payload_location_t (XDR *xdrs, dtn_bundle_payload_location_t *objp)
{
	register int32_t *buf;

	 if (!xdr_enum (xdrs, (enum_t *) objp))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_dtn_bundle_payload_t (XDR *xdrs, dtn_bundle_payload_t *objp)
{
	register int32_t *buf;

	 if (!xdr_dtn_bundle_payload_location_t (xdrs, &objp->location))
		 return FALSE;
	switch (objp->location) {
	case DTN_PAYLOAD_FILE:
		 if (!xdr_bytes (xdrs, (char **)&objp->dtn_bundle_payload_t_u.filename.filename_val, (u_int *) &objp->dtn_bundle_payload_t_u.filename.filename_len, DTN_MAX_PATH_LEN))
			 return FALSE;
		break;
	case DTN_PAYLOAD_MEM:
		 if (!xdr_bytes (xdrs, (char **)&objp->dtn_bundle_payload_t_u.buf.buf_val, (u_int *) &objp->dtn_bundle_payload_t_u.buf.buf_len, DTN_MAX_BUNDLE_MEM))
			 return FALSE;
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

/**
 * Type definition for a bundle identifier as returned from dtn_send.
 */

bool_t
xdr_dtn_bundle_id_t (XDR *xdrs, dtn_bundle_id_t *objp)
{
	register int32_t *buf;

	 if (!xdr_dtn_endpoint_id_t (xdrs, &objp->source))
		 return FALSE;
	 if (!xdr_u_int (xdrs, &objp->creation_secs))
		 return FALSE;
	 if (!xdr_u_int (xdrs, &objp->creation_subsecs))
		 return FALSE;
	return TRUE;
}

/**
 * Bundle authentication data. TBD
 */

bool_t
xdr_dtn_bundle_auth_t (XDR *xdrs, dtn_bundle_auth_t *objp)
{
	register int32_t *buf;

	 if (!xdr_bytes (xdrs, (char **)&objp->blob.blob_val, (u_int *) &objp->blob.blob_len, DTN_MAX_AUTHDATA))
		 return FALSE;
	return TRUE;
}
