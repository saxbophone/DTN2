
#include "APIServer.h"
#include "bundling/Bundle.h"
#include "io/UDPClient.h"
#include "io/NetUtils.h"
#include "routing/BundleRouter.h"

APIServer::APIServer()
{
    sock_ = new UDPClient();
    sock_->set_logfd(false);
    sock_->logpathf("/apisrv/sock");
    
    buflen_ = DTN_MAX_API_MSG;
    buf_ = (char*)malloc(buflen_);

    // note that we skip four bytes for the ipc typecode when setting
    // up the xdr decoder
    xdr_encode_ = new XDR;
    xdr_decode_ = new XDR;
    xdrmem_create(xdr_encode_, buf_, DTN_MAX_API_MSG, XDR_ENCODE);
    xdrmem_create(xdr_decode_, buf_ + sizeof(u_int32_t),
                  DTN_MAX_API_MSG, XDR_DECODE);
}

MasterAPIServer::MasterAPIServer()
    : APIServer(),
      Thread(), Logger("/apisrv/master")
{
    log_debug("MasterAPIServer::init (port %d)", DTN_API_HANDSHAKE_PORT);
    sock_->bind(htonl(INADDR_LOOPBACK), DTN_API_HANDSHAKE_PORT);
}

void
MasterAPIServer::run()
{
    in_addr_t addr;
    u_int16_t port;
    u_int32_t handshake;

    while (1) {
        int cc = sock_->recvfrom(buf_, buflen_, 0, &addr, &port);
        if (cc <= 0) {
            log_err("error in recvfrom: %s", strerror(errno));
            continue;
        }
        
        log_debug("got %d byte message from %s:%d", cc, intoa(addr), port);

        if (cc != sizeof(handshake)) {
            log_err("unexpected %d message from %s:%d "
                    "(expected %d byte handshake)",
                    cc, intoa(addr), port, sizeof(handshake));
            continue;
        }
        
        memcpy(&handshake, buf_, sizeof(handshake));
        
        if (handshake != DTN_OPEN) {
            log_warn("unexpected handshake type code 0x%x on master thread",
                     handshake);
            continue;
        }

        // otherwise create the client api server thread
        ClientAPIServer* server = new ClientAPIServer(addr, port);
        server->start();

        // XXX/demmer store these in a table so they can be queried
    }
}

ClientAPIServer::ClientAPIServer(in_addr_t remote_host,
                                 u_int16_t remote_port)
    : APIServer(),
      Thread(DELETE_ON_EXIT), Logger("/apisrv")
{
    log_debug("APIServer::init on port %d (remote %s:%d)",
              DTN_API_SESSION_PORT, intoa(remote_host), remote_port);

    sock_->bind(htonl(INADDR_LOOPBACK), DTN_API_SESSION_PORT);
    sock_->connect(remote_host, remote_port);
}


void
ClientAPIServer::run()
{
    // first and foremost, send the handshake response
    u_int32_t handshake = DTN_OPEN;
    if (sock_->send((char*)&handshake, sizeof(handshake), 0) < 0)
    {
        log_err("error sending handshake: %s", strerror(errno));
        return;
    }
    
    while (1) {
        int cc = sock_->recv(buf_, buflen_, 0);
        if (cc <= 0) {
            log_err("error in recvfrom: %s", strerror(errno));
            continue;
        }
        
        log_debug("got %d byte message", cc);
        
        xdr_setpos(xdr_decode_, 0);
        
        u_int32_t type;
        memcpy(&type, buf_, sizeof(type));

#define DISPATCH(_type, _fn)                    \
        case _type:                             \
            if (_fn() != 0) {                   \
                return;                         \
            }                                   \
            break;

        switch(type) {
            DISPATCH(DTN_GETINFO, handle_getinfo);
            DISPATCH(DTN_SEND,    handle_send);

        default:
            log_err("unknown message type code 0x%x", type);
        }
        
#undef DISPATCH
    }
}

int
ClientAPIServer::handle_getinfo()
{
    dtn_info_request_t request;
    dtn_info_response_t response;
    
    // unpack the request
    if (!xdr_dtn_info_request_t(xdr_decode_, &request))
    {
        log_err("error in xdr unpacking arguments");
        return -1;
    }

    // blank the response
    memset(&response, 0, sizeof(response));

    response.request = request;

    switch(request) {
    case DTN_INFOREQ_INTERFACES: {
        dtn_tuple_t* local_tuple =
            &response.dtn_info_response_t_u.interfaces.local_tuple;

        strncpy(local_tuple->region,
                BundleRouter::local_tuple_.region().c_str(),
                DTN_MAX_REGION_LEN);
        
        local_tuple->admin.admin_val =
            (char*)BundleRouter::local_tuple_.admin().c_str();
        
        local_tuple->admin.admin_len =
            BundleRouter::local_tuple_.admin().length();

        break;
    }
        
    case DTN_INFOREQ_CONTACTS:
        NOTIMPLEMENTED;
        break;
    case DTN_INFOREQ_ROUTES:
        NOTIMPLEMENTED;
        break;
    }
    
    /* send the return code */
    xdr_setpos(xdr_encode_, 0);
    if (!xdr_dtn_info_response_t(xdr_encode_, &response)) {
        log_err("internal error in xdr");
        return -1;
    }
    
    int len = xdr_getpos(xdr_encode_);
    if (sock_->send(buf_, len, 0) != len) {
        log_err("error sending response code");
        return -1;
    }

    return 0;
}

int
ClientAPIServer::handle_send()
{
    Bundle* b;
    long ret;
    dtn_bundle_spec_t spec;
    dtn_bundle_payload_t payload;
    
    /* Unpack the arguments */
    if (!xdr_dtn_bundle_spec_t(xdr_decode_, &spec) ||
        !xdr_dtn_bundle_payload_t(xdr_decode_, &payload))
    {
        log_err("error in xdr unpacking arguments");
        return -1;
    }

    b = new Bundle();
    
//     b->source_.assign(spec.source.tuple.tuple_val, spec.source.tuple.tuple_len);
//     b->dest_.assign(spec.dest.tuple.tuple_val, spec.dest.tuple.tuple_len);
//     b->replyto_.assign(spec.replyto.tuple.tuple_val, spec.replyto.tuple.tuple_len);

    b->custodian_.assign(b->source_); // XXX/demmer use my address?
    
    switch (spec.priority) {
#define COS(cos) case cos: b->priority_ = cos;
        COS(COS_BULK);
        COS(COS_NORMAL);
        COS(COS_EXPEDITED);
        COS(COS_RESERVED);
#undef COS
    default:
        log_err("invalid priority level %d", (int)spec.priority);
        ret = DTN_INVAL;
        goto done;
    };


    ret = DTN_SUCCESS;

    /* send the return code */
 done:
    xdr_setpos(xdr_encode_, 0);
    if (!xdr_putlong(xdr_encode_, &ret)) {
        log_err("internal error in xdr");
        return -1;
    }

    int len = xdr_getpos(xdr_encode_);
    if (sock_->send(buf_, len, 0) != len) {
        log_err("error sending response code");
        return -1;
    }

    return 0;
}

