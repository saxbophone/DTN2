/*
 * IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING. By
 * downloading, copying, installing or using the software you agree to
 * this license. If you do not agree to this license, do not download,
 * install, copy or use the software.
 * 
 * Intel Open Source License 
 * 
 * Copyright (c) 2004 Intel Corporation. All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 *   Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * 
 *   Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * 
 *   Neither the name of the Intel Corporation nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *  
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <strings.h>
#include <stdlib.h>
#include <sys/time.h>

#include "dtn_api.h"

const char *progname;

void
usage()
{
    fprintf(stderr, "usage: %s [-c count] [-i interval] tuple\n",
        progname);
    exit(1);
}

void doOptions(int argc, const char **argv);

int sleepVal = 1;
int count = 0;
char dest_tuple_str[DTN_MAX_TUPLE] = "";
char source_tuple_str[DTN_MAX_TUPLE] = "";
char replyto_tuple_str[DTN_MAX_TUPLE] = "";

int
main(int argc, const char** argv)
{
    int i;
    int ret;
    char b;
    dtn_handle_t handle;
    dtn_tuple_t source_tuple;
    dtn_reg_info_t reginfo;
    dtn_reg_id_t regid;
    dtn_bundle_spec_t ping_spec;
    dtn_bundle_spec_t reply_spec;
    dtn_bundle_payload_t ping_payload;
    dtn_bundle_payload_t reply_payload;
    int debug = 1;
    char demux[64];

    struct timeval start, end;
    
    doOptions(argc, argv);

    memset(&ping_spec, 0, sizeof(ping_spec));

    // open the ipc handle
    handle = dtn_open();
    if (handle == 0) {
        fprintf(stderr, "fatal error opening dtn handle: %s\n",
                strerror(errno));
        exit(1);
    }

    // make sure they supplied a valid destination tuple or
    // "localhost", in which case we just use the local daemon
    if (strcmp(dest_tuple_str, "localhost") == 0) {
        dtn_build_local_tuple(handle, &ping_spec.dest, "");

    } else {
        if (dtn_parse_tuple_string(&ping_spec.dest, dest_tuple_str)) {
            fprintf(stderr, "invalid destination tuple string '%s'\n",
                    dest_tuple_str);
            exit(1);
        }
    }
    
    // if the user specified a source tuple, register on it.
    // otherwise, build a local tuple based on the configuration of
    // our dtn router plus the demux string
    snprintf(demux, sizeof(demux), "/ping.%d", getpid());
    if (source_tuple_str[0] != '\0') {
        if (dtn_parse_tuple_string(&source_tuple, source_tuple_str)) {
            fprintf(stderr, "invalid source tuple string '%s'\n",
                    source_tuple_str);
            exit(1);
        }
    } else {
        dtn_build_local_tuple(handle, &source_tuple, demux);
    }

    // set the source tuple in the bundle spec
    if (debug) printf("source_tuple [%s %.*s]\n",
                      source_tuple.region,
                      (int) source_tuple.admin.admin_len, 
                      source_tuple.admin.admin_val);
    dtn_copy_tuple(&ping_spec.source, &source_tuple);
    
    // now parse the replyto tuple, if specified. otherwise just use
    // the source tuple
    if (replyto_tuple_str[0] != '\0') {
        if (dtn_parse_tuple_string(&ping_spec.replyto, replyto_tuple_str)) {
            fprintf(stderr, "invalid replyto tuple string '%s'\n",
                    replyto_tuple_str);
            exit(1);
        }
        
    } else {
        dtn_copy_tuple(&ping_spec.replyto, &source_tuple);
    }

    if (debug) printf("replyto_tuple [%s %.*s]\n",
                      ping_spec.replyto.region,
                      (int) ping_spec.replyto.admin.admin_len, 
                      ping_spec.replyto.admin.admin_val);
    
    // now create a new registration based on the source
    memset(&reginfo, 0, sizeof(reginfo));
    dtn_copy_tuple(&reginfo.endpoint, &source_tuple);
    reginfo.action = DTN_REG_ABORT;
    reginfo.regid = DTN_REGID_NONE;
    reginfo.timeout = 60 * 60;
    if ((ret = dtn_register(handle, &reginfo, &regid)) != 0) {
        fprintf(stderr, "error creating registration: %d (%s)\n",
                ret, dtn_strerror(dtn_errno(handle)));
        exit(1);
    }    
    if (debug) printf("dtn_register succeeded, regid 0x%x\n", regid);

    // bind the current handle to the new registration
    dtn_bind(handle, regid, &source_tuple);

    // check for any bundles queued for the registration
    if (debug) printf("checking for bundles already queued...\n");
    do {
        memset(&reply_spec, 0, sizeof(reply_spec));
        memset(&reply_payload, 0, sizeof(reply_payload));
        
        ret = dtn_recv(handle, &reply_spec,
                       DTN_PAYLOAD_MEM, &reply_payload, 0);

        if (ret == 0) {
            fprintf(stderr, "error: unexpected ping already queued... "
                    "discarding\n");
        } else if (dtn_errno(handle) != DTN_ETIMEOUT) {
            fprintf(stderr, "error: "
                    "unexpected error checking for queued bundles: %s\n",
                    dtn_strerror(dtn_errno(handle)));
            exit(1);
        }
    } while (ret == 0);
    
    // set the return receipt option
    ping_spec.dopts |= DOPTS_RETURN_RCPT;

    // fill in a payload of a single type code of 0x3 (echo request)
    b = 0x3;
    memset(&ping_payload, 0, sizeof(ping_payload));
    dtn_set_payload(&ping_payload, DTN_PAYLOAD_MEM, &b, 1);
    
    printf("PING [%s %.*s]...\n",
           ping_spec.dest.region,
           (int) ping_spec.dest.admin.admin_len, 
           ping_spec.dest.admin.admin_val);
    
    // loop, sending pings and getting replies.
    for (i = 0; count == 0 || i < count; ++i) {
        gettimeofday(&start, NULL);
        
        if ((ret = dtn_send(handle, &ping_spec, &ping_payload)) != 0) {
            fprintf(stderr, "error sending bundle: %d (%s)\n",
                    ret, dtn_strerror(dtn_errno(handle)));
            exit(1);
        }

        memset(&reply_spec, 0, sizeof(reply_spec));
        memset(&reply_payload, 0, sizeof(reply_payload));
        
        // now we block waiting for the echo reply
        if ((ret = dtn_recv(handle, &reply_spec,
                            DTN_PAYLOAD_MEM, &reply_payload, -1)) < 0)
        {
            fprintf(stderr, "error getting ping reply: %d (%s)\n",
                    ret, dtn_strerror(dtn_errno(handle)));
            exit(1);
        }
        gettimeofday(&end, NULL);

        printf("%d bytes from [%s %.*s]: time=%0.2f ms\n",
               reply_payload.dtn_bundle_payload_t_u.buf.buf_len,
               reply_spec.source.region,
               (int) reply_spec.source.admin.admin_len,
               reply_spec.source.admin.admin_val,
               ((double)(end.tv_sec - start.tv_sec) * 1000.0 + 
                (double)(end.tv_usec - start.tv_usec)/1000.0));
        
        sleep(sleepVal);
    }
    
    return 0;
}

void
doOptions(int argc, const char **argv)
{
    int c;

    progname = argv[0];

    while ( (c=getopt(argc, (char **) argv, "hc:i:d:s:r:")) !=EOF ) {
        switch (c) {
        case 'c':
            count = atoi(optarg);
            break;
        case 'i':
            sleepVal = atoi(optarg);
            break;
        case 'd':
            strcpy(dest_tuple_str, optarg);
            break;
        case 's':
            strcpy(source_tuple_str, optarg);
            break;
        case 'r':
            strcpy(replyto_tuple_str, optarg);
            break;
        case 'h':
            usage();
            break;
        default:
            break;
        }
    }

    if ((optind < argc) && (strlen(dest_tuple_str) == 0)) {
        strcpy(dest_tuple_str, argv[optind++]);
    }

    if (optind < argc) {
        fprintf(stderr, "unsupported argument '%s'\n", argv[optind]);
        exit(1);
    }

    if (dest_tuple_str[0] == '\0') {
        fprintf(stderr, "must supply a destination tuple (or 'localhost')\n");
        exit(1);
    }
}

