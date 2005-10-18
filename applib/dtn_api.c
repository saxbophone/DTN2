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
 * 	Redistributions of source code must retain the above copyright
 * 	notice, this list of conditions and the following disclaimer.
 * 
 * 	Redistributions in binary form must reproduce the above
 * 	copyright notice, this list of conditions and the following
 * 	disclaimer in the documentation and/or other materials
 * 	provided with the distribution.
 * 
 * 	Neither the name of the Intel Corporation nor the names of its
 * 	contributors may be used to endorse or promote products
 * 	derived from this software without specific prior written
 * 	permission.
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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>

#include "dtn_api.h"
#include "dtn_ipc.h"

/**
 * Open a new connection to the router.
 * Returns the new handle on success, 0 on error.
 */
dtn_handle_t
dtn_open()
{
    dtnipc_handle_t* handle;

    handle = (dtnipc_handle_t *) malloc(sizeof(struct dtnipc_handle));
    if (!handle)
        return 0;
    
    if (dtnipc_open(handle) != 0) {
        free(handle);
        return 0;
    }

    xdr_setpos(&handle->xdr_encode, 0);
    xdr_setpos(&handle->xdr_decode, 0);

    return (dtn_handle_t)handle;
}

/**
 * Close an open dtn handle.
 */
int
dtn_close(dtn_handle_t handle)
{
    // XXX/matt: (a) this error code is meaningless, as we return -1 no
    // matter what dtnipc_close returns; (b) it's inconsistent with
    // all other functions to return -1 as a "success" code
    dtnipc_close((dtnipc_handle_t *)handle);
    free(handle);
    return -1;
}

/**
 * Get the error associated with the given handle.
 */
int
dtn_errno(dtn_handle_t handle)
{
    return ((dtnipc_handle_t*)handle)->err;
}

/**
 * Get a string value associated with the dtn error code.
 */
char*
dtn_strerror(int err)
{
    switch(err) {
    case DTN_SUCCESS: 	return "success";
    case DTN_EINVAL: 	return "invalid argument";
    case DTN_EXDR: 	return "error in xdr routines";
    case DTN_ECOMM: 	return "error in ipc communication";
    case DTN_ECONNECT: 	return "error connecting to server";
    case DTN_ETIMEOUT: 	return "operation timed out";
    case DTN_ESIZE: 	return "payload too large";
    case DTN_ENOTFOUND: return "not found";
    case DTN_EINTERNAL: return "internal error";
    }

    return "(unknown error)";
}

/**
 * Build an appropriate local endpoint id by appending the specified
 * service tag to the daemon's preferred administrative endpoint id.
 */
int
dtn_build_local_eid(dtn_handle_t h,
                    dtn_endpoint_id_t* local_eid,
                    const char* tag)
{
    int status;
    dtnipc_handle_t* handle = (dtnipc_handle_t*)h;
    XDR* xdr_encode = &handle->xdr_encode;
    XDR* xdr_decode = &handle->xdr_decode;
    struct dtn_service_tag_t service_tag;

    // validate the tag length
    size_t len = strlen(tag) + 1;
    if (len > DTN_MAX_ENDPOINT_ID) {
        handle->err = DTN_EINVAL;
        return -1;
    }

    // pack the request
    memcpy(&service_tag.tag[0], tag, len);
    if (!xdr_dtn_service_tag_t(xdr_encode, &service_tag)) {
        handle->err = DTN_EXDR;
        return -1;
    }

    // send the message
    if (dtnipc_send(handle, DTN_LOCAL_EID) != 0) {
        return -1;
    }

    // get the reply
    if (dtnipc_recv(handle, &status) < 0) {
        return -1;
    }

    // handle server-side errors
    if (status != DTN_SUCCESS) {
        handle->err = status;
        return -1;
    }

    // unpack the response
    memset(local_eid, 0, sizeof(*local_eid));
    if (!xdr_dtn_endpoint_id_t(xdr_decode, local_eid)) {
        handle->err = DTN_EXDR;
        return -1;
    }
    
    return 0;
}

/**
 * Create or modify a dtn registration.
 */
int
dtn_register(dtn_handle_t h,
             dtn_reg_info_t *reginfo,
             dtn_reg_id_t* newregid)
{
    int status;
    dtnipc_handle_t* handle = (dtnipc_handle_t*)h;
    XDR* xdr_encode = &handle->xdr_encode;
    XDR* xdr_decode = &handle->xdr_decode;
    
    // pack the request
    if (!xdr_dtn_reg_info_t(xdr_encode, reginfo)) {
        handle->err = DTN_EXDR;
        return -1;
    }

    // send the message
    if (dtnipc_send(handle, DTN_REGISTER) != 0) {
        return -1;
    }

    // get the reply
    if (dtnipc_recv(handle, &status) < 0) {
        return -1;
    }

    // handle server-side errors
    if (status != DTN_SUCCESS) {
        handle->err = status;
        return -1;
    }

    // unpack the response
    if (!xdr_dtn_reg_id_t(xdr_decode, newregid)) {
        handle->err = DTN_EXDR;
        return -1;
    }

    return 0;
}

/**
 * Remove a dtn registration.
 */
int
dtn_unregister(dtn_handle_t h, dtn_reg_id_t regid)
{
    // XXX/demmer implement me
    dtnipc_handle_t* handle = (dtnipc_handle_t*)h;
    handle->err = DTN_EINTERNAL;
    return -1;
}

/**
 * Modify an existing registration.
 */
int
dtn_change_registration(dtn_handle_t h,
                        dtn_reg_id_t regid,
                        dtn_reg_info_t *reginfo)
{
    // XXX/demmer implement me
    dtnipc_handle_t* handle = (dtnipc_handle_t*)h;
    handle->err = DTN_EINTERNAL;
    return -1;
}

/**
 * Associate a registration id with the current ipc channel. This must
 * be called before calling dtn_recv to inform the daemon of which
 * registrations the application is interested in.
 */
int
dtn_bind(dtn_handle_t h, dtn_reg_id_t regid)
{
    dtnipc_handle_t* handle = (dtnipc_handle_t*)h;
    XDR* xdr_encode = &handle->xdr_encode;
    
    // pack the request
    if (!xdr_dtn_reg_id_t(xdr_encode, &regid)) {
        handle->err = DTN_EXDR;
        return -1;
    }

    // send the message
    if (dtnipc_send_recv(handle, DTN_BIND) < 0) {
        return -1;
    }

    return 0;
}

/**
 * Send a bundle either from memory or from a file.
 */
int
dtn_send(dtn_handle_t h,
         dtn_bundle_spec_t* spec,
         dtn_bundle_payload_t* payload)
{
    dtnipc_handle_t* handle = (dtnipc_handle_t*)h;
    XDR* xdr_encode = &handle->xdr_encode;

    // pack the arguments
    if ((!xdr_dtn_bundle_spec_t(xdr_encode, spec)) ||
        (!xdr_dtn_bundle_payload_t(xdr_encode, payload))) {
        handle->err = DTN_EXDR;
        return -1;
    }

    // send the message
    if (dtnipc_send_recv(handle, DTN_SEND) < 0) {
        return -1;
    }

    return 0;
}

/**
 * Blocking receive for a bundle
 */
int
dtn_recv(dtn_handle_t h,
         dtn_bundle_spec_t* spec,
         dtn_bundle_payload_location_t location,
         dtn_bundle_payload_t* payload,
         dtn_timeval_t timeout)
{
    dtnipc_handle_t* handle = (dtnipc_handle_t*)h;
    XDR* xdr_encode = &handle->xdr_encode;
    XDR* xdr_decode = &handle->xdr_decode;

    // zero out the spec and payload structures
    memset(spec, 0, sizeof(*spec));
    memset(payload, 0, sizeof(*payload));

    // pack the arguments
    if ((!xdr_dtn_bundle_payload_location_t(xdr_encode, &location)) ||
        (!xdr_dtn_timeval_t(xdr_encode, &timeout)))
    {
        handle->err = DTN_EXDR;
        return -1;
    }

    // send the message
    if (dtnipc_send_recv(handle, DTN_RECV) < 0) {
        return -1;
    }

    // unpack the bundle
    if (!xdr_dtn_bundle_spec_t(xdr_decode, spec) ||
        !xdr_dtn_bundle_payload_t(xdr_decode, payload))
    {
        handle->err = DTN_EXDR;
        return -1;
    }
    
    return 0;
}


/*************************************************************
 *
 *                     Utility Functions
 *
 *************************************************************/

/*
 * Copy the contents of one eid into another.
 */
void
dtn_copy_eid(dtn_endpoint_id_t* dst, dtn_endpoint_id_t* src)
{
    memcpy(dst->uri, src->uri, DTN_MAX_ENDPOINT_ID);
}

/*
 * Parse a string into an endpoint id structure, validating that it is
 * in fact a valid endpoint id (i.e. a URI).
 */
int
dtn_parse_eid_string(dtn_endpoint_id_t* eid, const char* str)
{
    char *s;
    size_t len;

    len = strlen(str) + 1;

    // check string length
    if (len > DTN_MAX_ENDPOINT_ID) {
        return DTN_ESIZE;
    }
    
    // make sure there's a colon
    s = strchr(str, ':');
    if (!s) {
        return DTN_EINVAL;
    }

    // XXX/demmer make sure the scheme / ssp are comprised only of
    // legal URI characters
    memcpy(&eid->uri[0], str, len);

    return 0;
}

/*
 * Sets the value of the given payload structure to either a memory
 * buffer or a file location.
 *
 * Returns: 0 on success, DTN_ESIZE if the memory location is
 * selected and the payload is too big.
 */
int
dtn_set_payload(dtn_bundle_payload_t* payload,
                dtn_bundle_payload_location_t location,
                char* val, int len)
{
    payload->location = location;

    if (location == DTN_PAYLOAD_MEM && len > DTN_MAX_BUNDLE_MEM) {
        return DTN_ESIZE;
    }
    
    switch (location) {
    case DTN_PAYLOAD_MEM:
        payload->dtn_bundle_payload_t_u.buf.buf_val = val;
        payload->dtn_bundle_payload_t_u.buf.buf_len = len;
        break;
    case DTN_PAYLOAD_FILE:
        payload->dtn_bundle_payload_t_u.filename.filename_val = val;
        payload->dtn_bundle_payload_t_u.filename.filename_len = len;
        break;
    }

    return 0;
}

