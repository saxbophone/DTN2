/*
 * IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING. By
 * downloading, copying, installing or using the software you agree to
 * this license. If you do not agree to this license, do not download,
 * install, copy or use the software.
 * 
 * Intel Open Source License 
 * 
 * Copyright (c) 2006 Intel Corporation. All rights reserved. 
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

/*
 * dtncat: move stdin to bundles and vice-versa
 * resembles the nc (netcat) unix program
 * - kfall Apr 2006
 *
 *   Usage: dtncat [-options] -s EID -d EID
 *   dtncat -l -s EID -d EID
 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <strings.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#include "dtn_api.h"

char *progname;

// global options
int copies              = 1;    // the number of copies to send
int verbose             = 0;

// bundle options
int expiration          = 3600; // expiration timer (default one hour)
int delivery_receipts   = 0;    // request end to end delivery receipts
int forwarding_receipts = 0;    // request per hop departure
int custody             = 0;    // request custody transfer
int custody_receipts    = 0;    // request per custodian receipts
int receive_receipts    = 0;    // request per hop arrival receipt
int wait_for_report     = 0;    // wait for bundle status reports
int bundle_count	= -1;	// # bundles to receive (-l option)

#define DEFAULT_BUNDLE_COUNT	1
#define FAILURE_SCRIPT ""

#ifdef MIN
#undef MIN
#endif
#define MIN(a, b) ((a) < (b) ? (a) : (b))

// specified options for bundle eids
char * arg_replyto      = NULL;
char * arg_source       = NULL;
char * arg_dest         = NULL;
char * arg_receive         = NULL;

dtn_reg_id_t regid      = DTN_REGID_NONE;

void parse_options(int, char**);
dtn_endpoint_id_t * parse_eid(dtn_handle_t handle,
                              dtn_endpoint_id_t * eid,
                              char * str);
void print_usage();
void print_eid(FILE*, char * label, dtn_endpoint_id_t * eid);
int fill_payload(dtn_bundle_payload_t* payload);

FILE* info;	/* when -v option is used, write to here */
#define REG_EXPIRE (60 * 60)
char payload_buf[DTN_MAX_BUNDLE_MEM];

int from_bundles_flag;

void to_bundles();	// stdin -> bundles
void from_bundles();	// bundles -> stdout
void make_registration(dtn_reg_info_t*);

dtn_handle_t handle;
dtn_bundle_spec_t bundle_spec;
dtn_bundle_spec_t reply_spec;
dtn_bundle_payload_t primary_payload;
dtn_bundle_payload_t reply_payload;
dtn_bundle_id_t bundle_id;
struct timeval start, end;


int
main(int argc, char** argv)
{

    info = stderr;
    
    // force stdout to always be line buffered, even if output is
    // redirected to a pipe or file -- why? kf
    // setvbuf(stdout, (char *)NULL, _IOLBF, 0);
    
    parse_options(argc, argv);

    // open the ipc handle
    if (verbose)
	fprintf(info, "Opening connection to local DTN daemon\n");

    int err = dtn_open(&handle);
    if (err != DTN_SUCCESS) {
        fprintf(stderr, "%s: fatal error opening dtn handle: %s\n",
                progname, dtn_strerror(err));
        exit(EXIT_FAILURE);
    }

    if (gettimeofday(&start, NULL) < 0) {
		fprintf(stderr, "%s: gettimeofday(start) returned error %s\n",
			progname, strerror(errno));
		exit(EXIT_FAILURE);
    }

    if (from_bundles_flag)
	    from_bundles();
    else
	    to_bundles();

    dtn_close(handle);
    return (EXIT_SUCCESS);
}


/*
 * [bundles] -> stdout
 */

void
from_bundles()
{
    int total_bytes = 0, i, ret;
    char *buffer;
    char s_buffer[BUFSIZ];

    dtn_reg_info_t reg;
    dtn_endpoint_id_t local_eid;
    dtn_bundle_spec_t receive_spec;

    parse_eid(handle, &local_eid, arg_receive);

    memset(&reg, 0, sizeof(reg));
    dtn_copy_eid(&reg.endpoint, &local_eid);
    make_registration(&reg);

    if (verbose)
	    fprintf(info, "waiting to receive %d bundles using reg >%s<\n",
			    bundle_count, reg.endpoint.uri);

    // loop waiting for bundles
    for (i = 0; i < bundle_count; ++i) {
        size_t bytes = 0;
        u_int k;
        
	// read from network
        if ((ret = dtn_recv(handle, &receive_spec,
	        DTN_PAYLOAD_MEM, &primary_payload, -1)) < 0) {
            fprintf(stderr, "%s: error getting recv reply: %d (%s)\n",
                    progname, ret, dtn_strerror(dtn_errno(handle)));
            exit(EXIT_FAILURE);
        }

        bytes = primary_payload.dtn_bundle_payload_t_u.buf.buf_len;
        buffer =
	  (char *) primary_payload.dtn_bundle_payload_t_u.buf.buf_val;
        total_bytes += bytes;

	// write to stdout
	if (fwrite(buffer, 1, bytes, stdout) != bytes) {
            fprintf(stderr, "%s: error writing to stdout\n",
                    progname);
            exit(EXIT_FAILURE);
	}

        if (!verbose) {
            continue;
        }
        
        fprintf(info, "%d bytes from [%s]: transit time=%d ms\n",
               primary_payload.dtn_bundle_payload_t_u.buf.buf_len,
               receive_spec.source.uri, 0);

        for (k=0; k < primary_payload.dtn_bundle_payload_t_u.buf.buf_len; k++) {
            if (buffer[k] >= ' ' && buffer[k] <= '~')
                s_buffer[k%BUFSIZ] = buffer[k];
            else
                s_buffer[k%BUFSIZ] = '.';

            if (k%BUFSIZ == 0) // new line every 16 bytes
            {
                fprintf(info,"%07x ", k);
            }
            else if (k%2 == 0)
            {
                fprintf(info," "); // space every 2 bytes
            }
                    
            fprintf(info,"%02x", buffer[k] & 0xff);
                    
            // print character summary (a la emacs hexl-mode)
            if (k%BUFSIZ == BUFSIZ-1)
            {
                fprintf(info," |  %.*s\n", BUFSIZ, s_buffer);
            }
        }

        // print spaces to fill out the rest of the line
	if (k%BUFSIZ != BUFSIZ-1) {
            while (k%BUFSIZ != BUFSIZ-1) {
                if (k%2 == 0) {
                    fprintf(info," ");
                }
                fprintf(info,"  ");
                k++;
            }
            fprintf(info,"   |  %.*s\n",
              (int)primary_payload.dtn_bundle_payload_t_u.buf.buf_len%BUFSIZ, 
                   s_buffer);
        }
	fprintf(info,"\n");
    }
}

/*
 * stdout -> [bundles]
 */

void
to_bundles()
{

    int bytes, ret;
    dtn_reg_info_t reg_report; /* for reports, if reqd */
	
    // initialize bundle spec
    memset(&bundle_spec, 0, sizeof(bundle_spec));
    parse_eid(handle, &bundle_spec.dest, arg_dest);
    parse_eid(handle, &bundle_spec.source, arg_source);

    if (arg_replyto == NULL) {
        dtn_copy_eid(&bundle_spec.replyto, &bundle_spec.source);
    } else {
        parse_eid(handle, &bundle_spec.replyto, arg_replyto);
    }

    if (verbose) {
        print_eid(info, "source_eid", &bundle_spec.source);
        print_eid(info, "replyto_eid", &bundle_spec.replyto);
        print_eid(info, "dest_eid", &bundle_spec.dest);
    }

    if (wait_for_report) {
	// make a registration for incoming reports
        memset(&reg_report, 0, sizeof(reg_report));
	dtn_copy_eid(&reg_report.endpoint, &bundle_spec.replyto);
	make_registration(&reg_report);
    }
    
    // set the dtn options
    bundle_spec.expiration = expiration;
    
    if (delivery_receipts) {
        // set the delivery receipt option
        bundle_spec.dopts |= DOPTS_DELIVERY_RCPT;
    }

    if (forwarding_receipts) {
        // set the forward receipt option
        bundle_spec.dopts |= DOPTS_FORWARD_RCPT;
    }

    if (custody) {
        // request custody transfer
        bundle_spec.dopts |= DOPTS_CUSTODY;
    }

    if (custody_receipts) {
        // request custody transfer
        bundle_spec.dopts |= DOPTS_CUSTODY_RCPT;
    }

    if (receive_receipts) {
        // request receive receipt
        bundle_spec.dopts |= DOPTS_RECEIVE_RCPT;
    }
    

    if ((bytes = fill_payload(&primary_payload)) < 0) {
	fprintf(stderr, "%s: error reading bundle data\n",
	    progname);
	exit(EXIT_FAILURE);
    }

    memset(&bundle_id, 0, sizeof(bundle_id));
    if ((ret = dtn_send(handle, &bundle_spec, &primary_payload,
                        &bundle_id)) != 0) {
	fprintf(stderr, "%s: error sending bundle: %d (%s)\n",
	    progname, ret, dtn_strerror(dtn_errno(handle)));
	exit(EXIT_FAILURE);
    }

    if (verbose)
        fprintf(info, "Read %d bytes from stdin and wrote to bundles\n",
		bytes);

    if (wait_for_report) {
	memset(&reply_spec, 0, sizeof(reply_spec));
	memset(&reply_payload, 0, sizeof(reply_payload));
	
	// now we block waiting for any replies
	if ((ret = dtn_recv(handle, &reply_spec,
			    DTN_PAYLOAD_MEM, &reply_payload, -1)) < 0) {
	    fprintf(stderr, "%s: error getting reply: %d (%s)\n",
		    progname, ret, dtn_strerror(dtn_errno(handle)));
	    exit(EXIT_FAILURE);
	}
	if (gettimeofday(&end, NULL) < 0) {
	    fprintf(stderr, "%s: gettimeofday(end) returned error %s\n",
		    progname, strerror(errno));
	    exit(EXIT_FAILURE);
	}
    
	if (verbose)
	    fprintf(info, "got %d byte report from [%s]: time=%.1f ms\n",
	       reply_payload.dtn_bundle_payload_t_u.buf.buf_len,
	       reply_spec.source.uri,
	       ((double)(end.tv_sec - start.tv_sec) * 1000.0 + 
		(double)(end.tv_usec - start.tv_usec)/1000.0));
    }
}


void print_usage()
{
    fprintf(stderr, "To source bundles from stdin:\n");
    fprintf(stderr, "    usage: %s [opts] -s <source_eid> -d <dest_eid>\n",
            progname);
    fprintf(stderr, "To receive bundles to stdout:\n");
    fprintf(stderr, "    usage: %s [opts] -l <receive_eid>\n", progname);

    fprintf(stderr, "common options:\n");
    fprintf(stderr, " -v verbose\n");
    fprintf(stderr, " -h/H help\n");
    fprintf(stderr, "receive only options (-l option required):\n");

    fprintf(stderr, " -l <eid> receive bundles destined for eid (instead of sending)\n");
    fprintf(stderr, " -n <count> exit after count bundles received (-l option required)\n");
    fprintf(stderr, "send only options (-l option prohibited):\n");
    fprintf(stderr, " -s <eid|demux_string> source eid)\n");
    fprintf(stderr, " -d <eid|demux_string> destination eid)\n");
    fprintf(stderr, " -r <eid|demux_string> reply to eid)\n");
    fprintf(stderr, " -e <time> expiration time in seconds (default: one hour)\n");
    fprintf(stderr, " -i <regid> registration id for reply to\n");
    fprintf(stderr, " -c request custody transfer\n");
    fprintf(stderr, " -C request custody transfer receipts\n");
    fprintf(stderr, " -D request for end-to-end delivery receipt\n");
    fprintf(stderr, " -R request for bundle reception receipts\n");
    fprintf(stderr, " -F request for bundle forwarding receipts\n");
    fprintf(stderr, " -w wait for bundle status reports\n");
    
    return;
}

void
parse_options(int argc, char**argv)
{
    char c, done = 0;
    int lopts = 0, notlopts = 0;

    progname = argv[0];

    while (!done) {
        c = getopt(argc, argv, "l:vhHr:s:d:e:wDFRcCi:n:");
        switch (c) {
	case 'l':
	    from_bundles_flag = 1;
	    arg_receive = optarg;
	    break;
        case 'v':
            verbose = 1;
            break;
        case 'h':
        case 'H':
            print_usage();
            exit(EXIT_SUCCESS);
            return;
        case 'r':
            arg_replyto = optarg;
	    lopts++;
            break;
        case 's':
            arg_source = optarg;
	    notlopts++;
            break;
        case 'd':
            arg_dest = optarg;
	    notlopts++;
            break;
        case 'e':
            expiration = atoi(optarg);
	    notlopts++;
            break;
        case 'w':
            wait_for_report = 1;
	    notlopts++;
            break;
        case 'D':
            delivery_receipts = 1;
	    notlopts++;
            break;
        case 'F':
            forwarding_receipts = 1;
	    notlopts++;
            break;
        case 'R':
            receive_receipts = 1;
	    notlopts++;
            break;
        case 'c':
            custody = 1;
	    notlopts++;
            break;
        case 'C':
            custody_receipts = 1;
	    notlopts++;
            break;
        case 'i':
            regid = atoi(optarg);
	    notlopts++;
            break;
	case 'n':
	    bundle_count = atoi(optarg);
	    lopts++;
	    break;
        case -1:
            done = 1;
            break;
        default:
            // getopt already prints an error message for unknown
            // option characters
            print_usage();
            exit(EXIT_FAILURE);
        }
    }

    if (from_bundles_flag && (notlopts > 0)) {
	    fprintf(stderr, "%s: error: transmission options specified when using -l flag\n",
			progname);
        print_usage();
        exit(EXIT_FAILURE);
    }

    if (!from_bundles_flag && (lopts > 0)) {
	    fprintf(stderr, "%s: error: receive option specified but -l option not selected\n",
			progname);
        print_usage();
        exit(EXIT_FAILURE);
    }


#define CHECK_SET(_arg, _what)                                          \
    if (_arg == 0) {                                                    \
        fprintf(stderr, "%s: %s must be specified\n", progname, _what); \
        print_usage();                                                  \
        exit(EXIT_FAILURE);                                             \
    }
    
    if (!from_bundles_flag) {
    	    /* transmitting to bundles - no -l option */
	    CHECK_SET(arg_source,   "source eid");
	    CHECK_SET(arg_dest,     "destination eid");
    } else {
    	    /* receiving from bundles - -l option specified */
	    CHECK_SET(arg_receive,  "receive eid");
	    if (bundle_count == -1) {
		    bundle_count = DEFAULT_BUNDLE_COUNT;
	    }
    }
}

dtn_endpoint_id_t *
parse_eid(dtn_handle_t handle, dtn_endpoint_id_t* eid, char * str)
{
    // try the string as an actual dtn eid
    if (!dtn_parse_eid_string(eid, str)) {
        if (verbose)
		fprintf(info, "%s (literal)\n", str);
        return eid;
    } else if (!dtn_build_local_eid(handle, eid, str)) {
        // build a local eid based on the configuration of our dtn
        // router plus the str as demux string
        if (verbose) fprintf(info, "%s (local)\n", str);
        return eid;
    } else {
        fprintf(stderr, "invalid eid string '%s'\n", str);
        exit(EXIT_FAILURE);
    }
}

void
print_eid(FILE *dest, char *  label, dtn_endpoint_id_t * eid)
{
    fprintf(dest, "%s [%s]\n", label, eid->uri);
}

/*
 * read from stdin to get the payload
 */

int
fill_payload(dtn_bundle_payload_t* payload)
{

   unsigned char buf[BUFSIZ];
   unsigned char *p = (unsigned char*) payload_buf;
   unsigned char *endp = p + sizeof(payload_buf);
   size_t n, total = 0;
   size_t maxread = sizeof(buf);

   while (1) {
       maxread = MIN((int)sizeof(buf), (endp-p));
       if ((n = fread(buf, 1, maxread, stdin)) == 0)
	       break;
       memcpy(p, buf, n);
       p += n;
       total += n;
   }
   if (ferror(stdin))
       return (-1); 

   if (dtn_set_payload(payload, DTN_PAYLOAD_MEM, payload_buf, total) == DTN_ESIZE)
	   return (-1);
   return(total);
}

void
make_registration(dtn_reg_info_t* reginfo)
{
	int ret;

        // try to find an existing registration to use
        ret = dtn_find_registration(handle, &reginfo->endpoint, &regid);

        if (ret == 0) {

            // found the registration, bind it to the handle
            if (dtn_bind(handle, regid) != DTN_SUCCESS) {
                fprintf(stderr, "%s: error in bind (id=0x%x): %d (%s)\n",
                        progname, regid, ret, dtn_strerror(dtn_errno(handle)));
                exit(EXIT_FAILURE);
            }

            return;
            
        } else if (dtn_errno(handle) == DTN_ENOTFOUND) {
            // fall through

        } else {
            fprintf(stderr, "%s: error in dtn_find_registration: %d (%s)\n",
                    progname, ret, dtn_strerror(dtn_errno(handle)));
            exit(EXIT_FAILURE);
        }

        // create a new dtn registration to receive bundles
        reginfo->regid = regid;
        reginfo->expiration = REG_EXPIRE;
        reginfo->failure_action = DTN_REG_DROP;
	reginfo->script.script_val = FAILURE_SCRIPT;
	reginfo->script.script_len = strlen(reginfo->script.script_val) + 1;

	if ((ret = dtn_register(handle, reginfo, &regid)) != 0) {
            fprintf(stderr, "%s: error creating registration (id=0x%x): %d (%s)\n",
                    progname, regid, ret, dtn_strerror(dtn_errno(handle)));
            exit(EXIT_FAILURE);
        }
    
        if (verbose)
            fprintf(info, "dtn_register succeeded, regid 0x%x\n", regid);
}
