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

#include <oasys/debug/Log.h>
#include <oasys/io/NetUtils.h>
#include <oasys/tclcmd/ConsoleCommand.h>
#include <oasys/tclcmd/TclCommand.h>
#include <oasys/util/Getopt.h>
#include <oasys/util/OptParser.h>

#include <dtn_api.h>
#include <EndpointIDOpt.h>

typedef std::map<int, dtn_handle_t> HandleMap;

struct State : public oasys::Singleton<State> {
    State() : handle_num_(0) {}
        
    HandleMap handles_;
    int handle_num_;
};

template <> State* oasys::Singleton<State>::instance_ = 0;

//----------------------------------------------------------------------
class DTNOpenCommand : public oasys::TclCommand {
public:
    DTNOpenCommand() : TclCommand("dtn_open") {}
    int exec(int argc, const char **argv,  Tcl_Interp* interp)
    {
        (void)argc;
        (void)argv;
        (void)interp;

        if (argc != 1) {
            wrong_num_args(argc, argv, 1, 1, 1);
            return TCL_ERROR;
        }

        dtn_handle_t h = dtn_open();
        if (h == NULL) {
            resultf("can't connect to dtn daemon");
            return TCL_ERROR;
        }

        int n = State::instance()->handle_num_++;
        State::instance()->handles_[n] = h;

        resultf("%d", n);
        return TCL_OK;
    }
};

//----------------------------------------------------------------------
class DTNCloseCommand : public oasys::TclCommand {
public:
    DTNCloseCommand() : TclCommand("dtn_close") {}
    int exec(int argc, const char **argv,  Tcl_Interp* interp)
    {
        (void)argc;
        (void)argv;
        (void)interp;

        if (argc != 2) {
            wrong_num_args(argc, argv, 1, 2, 2);
            return TCL_ERROR;
        }

        int n = atoi(argv[1]);
        HandleMap::iterator iter = State::instance()->handles_.find(n);
        if (iter == State::instance()->handles_.end()) {
            resultf("invalid dtn handle %d", n);
            return TCL_ERROR;
        }

        dtn_handle_t h = iter->second;
        dtn_close(h);

        return TCL_OK;
    }
};

//----------------------------------------------------------------------
oasys::EnumOpt::Case FailureActionCases[] = {
    {"drop",  DTN_REG_DROP},
    {"defer", DTN_REG_DEFER},
    {"exec",  DTN_REG_EXEC},
    {0, 0}
};

class DTNRegisterCommand : public oasys::TclCommand {
public:
    oasys::OptParser parser_;

    struct RegistrationOpts {
        dtn_endpoint_id_t endpoint_;
        int               failure_action_;
        u_int             expiration_;
        std::string       script_;
    };
    
    RegistrationOpts opts_;

    void init_opts() {
        memset(&opts_.endpoint_, 0, sizeof(opts_.endpoint_));
        opts_.failure_action_ = DTN_REG_DROP;
        opts_.expiration_ = 0;
        opts_.script_ = "";
    }

    DTNRegisterCommand() : TclCommand("dtn_register")
    {
        parser_.addopt(new dtn::EndpointIDOpt("endpoint", &opts_.endpoint_));
        parser_.addopt(new oasys::EnumOpt("failure_action",
                                          FailureActionCases,
                                          &opts_.failure_action_));
        parser_.addopt(new oasys::UIntOpt("expiration", &opts_.expiration_));
        parser_.addopt(new oasys::StringOpt("script", &opts_.script_));
    }
    
    int exec(int argc, const char **argv,  Tcl_Interp* interp)
    {
        (void)argc;
        (void)argv;
        (void)interp;

        // need at least cmd, handle, endpoint, and expiration
        if (argc < 4) {
            wrong_num_args(argc, argv, 1, 4, INT_MAX);
            return TCL_ERROR;
        }

        int n = atoi(argv[1]);
        HandleMap::iterator iter = State::instance()->handles_.find(n);
        if (iter == State::instance()->handles_.end()) {
            resultf("invalid dtn handle %d", n);
            return TCL_ERROR;
        }

        dtn_handle_t h = iter->second;

        init_opts();
        const char* invalid = 0;
        if (! parser_.parse(argc - 2, argv + 2, &invalid)) {
            resultf("invalid option '%s'", invalid);
            return TCL_ERROR;
        }

        if (opts_.endpoint_.uri[0] == 0) {
            resultf("must set endpoint id");
            return TCL_ERROR;
        }
        
        if (opts_.expiration_ == 0) {
            resultf("must set expiration");
            return TCL_ERROR;
        }
        
        dtn_reg_info_t reginfo;
        memset(&reginfo, 0, sizeof(reginfo));

        dtn_copy_eid(&reginfo.endpoint, &opts_.endpoint_);
        reginfo.failure_action =
            (dtn_reg_failure_action_t)opts_.failure_action_;
        reginfo.expiration = opts_.expiration_;
        reginfo.script.script_len = opts_.script_.length();
        reginfo.script.script_val = (char*)opts_.script_.c_str();

        dtn_reg_id_t regid = 0;
        
        int ret = dtn_register(h, &reginfo, &regid);
        if (ret != DTN_SUCCESS) {
            resultf("error in dtn_register: %s",
                    dtn_strerror(dtn_errno(h)));
            return TCL_ERROR;
        }

        resultf("%u", regid);
        return TCL_OK;
    }
};

//----------------------------------------------------------------------
class DTNUnregisterCommand : public oasys::TclCommand {
public:
    DTNUnregisterCommand() : TclCommand("dtn_unregister") {}
    int exec(int argc, const char **argv,  Tcl_Interp* interp)
    {
        (void)interp;

        if (argc != 3) {
            wrong_num_args(argc, argv, 1, 3, 3);
            return TCL_ERROR;
        }

        int n = atoi(argv[1]);
        HandleMap::iterator iter = State::instance()->handles_.find(n);
        if (iter == State::instance()->handles_.end()) {
            resultf("invalid dtn handle %d", n);
            return TCL_ERROR;
        }

        dtn_handle_t h = iter->second;

        dtn_reg_id_t regid = atoi(argv[2]);

        int err = dtn_unregister(h, regid);
        if (err != DTN_SUCCESS) {
            resultf("error in dtn_unregister: %s",
                    dtn_strerror(dtn_errno(h)));
            return TCL_ERROR;
        }
        
        return TCL_OK;
    }
};

//----------------------------------------------------------------------
oasys::EnumOpt::Case PriorityCases[] = {
    {"bulk",      COS_BULK},
    {"normal",    COS_NORMAL},
    {"expedited", COS_EXPEDITED},
    {0, 0}
};

class DTNSendCommand : public oasys::TclCommand {
public:
    struct SendOpts {
        dtn_endpoint_id_t source_;
        dtn_endpoint_id_t dest_;
        dtn_endpoint_id_t replyto_;
        int    priority_;
        bool   custody_xfer_;
        bool   receive_rcpt_;
        bool   custody_rcpt_;
        bool   forward_rcpt_;
        bool   delivery_rcpt_;
        bool   deletion_rcpt_;
        u_int  expiration_;
        char   payload_data_[DTN_MAX_BUNDLE_MEM];
        size_t payload_data_len_;
        char   payload_file_[DTN_MAX_PATH_LEN];
        size_t payload_file_len_;
    };

    // we use a single parser and options struct for the command for
    // improved efficiency
    oasys::OptParser parser_;
    SendOpts opts_;

    void init_opts() {
        memset(&opts_, 0, sizeof(opts_));
        opts_.expiration_ = 5 * 60;
    }

    DTNSendCommand() : TclCommand("dtn_send")
    {
        parser_.addopt(new dtn::EndpointIDOpt("source", &opts_.source_));
        parser_.addopt(new dtn::EndpointIDOpt("dest", &opts_.dest_));
        parser_.addopt(new dtn::EndpointIDOpt("replyto", &opts_.replyto_));
        parser_.addopt(new oasys::EnumOpt("priority", PriorityCases,
                                          &opts_.priority_));
        parser_.addopt(new oasys::BoolOpt("custody_xfer",
                                          &opts_.custody_xfer_));
        parser_.addopt(new oasys::BoolOpt("receive_rcpt",
                                          &opts_.receive_rcpt_));
        parser_.addopt(new oasys::BoolOpt("custody_rcpt",
                                          &opts_.custody_rcpt_));
        parser_.addopt(new oasys::BoolOpt("forward_rcpt",
                                          &opts_.forward_rcpt_));
        parser_.addopt(new oasys::BoolOpt("delivery_rcpt",
                                          &opts_.delivery_rcpt_));
        parser_.addopt(new oasys::BoolOpt("deletion_rcpt",
                                          &opts_.deletion_rcpt_));
        parser_.addopt(new oasys::UIntOpt("expiration",
                                          &opts_.expiration_));
        parser_.addopt(new oasys::CharBufOpt("payload_data",
                                             opts_.payload_data_,
                                             &opts_.payload_data_len_,
                                             sizeof(opts_.payload_data_)));
        parser_.addopt(new oasys::CharBufOpt("payload_file",
                                             opts_.payload_file_,
                                             &opts_.payload_file_len_,
                                             sizeof(opts_.payload_file_)));
    }
    
    int exec(int argc, const char **argv,  Tcl_Interp* interp)
    {
        (void)argc;
        (void)argv;
        (void)interp;

        // need at least the command, handle, source, dest, and payload
        if (argc < 5) {
            wrong_num_args(argc, argv, 1, 5, INT_MAX);
            return TCL_ERROR;
        }
        
        int n = atoi(argv[1]);
        HandleMap::iterator iter = State::instance()->handles_.find(n);
        if (iter == State::instance()->handles_.end()) {
            resultf("invalid dtn handle %d", n);
            return TCL_ERROR;
        }
        
        dtn_handle_t h = iter->second;
        
        // now parse all the options
        init_opts();
        const char* invalid = 0;
        if (! parser_.parse(argc - 2, argv + 2, &invalid)) {
            resultf("invalid option '%s'", invalid);
            return TCL_ERROR;
        }

        // validate that source, dest, and some payload was specified
        if (opts_.source_.uri[0] == 0) {
            resultf("must set source endpoint id");
            return TCL_ERROR;
        }
        if (opts_.dest_.uri[0] == 0) {
            resultf("must set dest endpoint id");
            return TCL_ERROR;
        }
        if (opts_.payload_data_len_ == 0 && opts_.payload_file_len_ == 0) {
            resultf("must set payload");
            return TCL_ERROR;
        }

        dtn_bundle_spec_t spec;
        memset(&spec, 0, sizeof(spec));
        dtn_copy_eid(&spec.source, &opts_.source_);
        dtn_copy_eid(&spec.dest,   &opts_.dest_);
        if (opts_.replyto_.uri[0] != 0) {
            dtn_copy_eid(&spec.replyto, &opts_.replyto_);
        }
        spec.priority = (dtn_bundle_priority_t)opts_.priority_;
        if (opts_.custody_xfer_)  spec.dopts |= DOPTS_CUSTODY;
        if (opts_.receive_rcpt_)  spec.dopts |= DOPTS_RECEIVE_RCPT;
        if (opts_.custody_rcpt_)  spec.dopts |= DOPTS_CUSTODY_RCPT;
        if (opts_.forward_rcpt_)  spec.dopts |= DOPTS_FORWARD_RCPT;
        if (opts_.delivery_rcpt_) spec.dopts |= DOPTS_DELIVERY_RCPT;
        if (opts_.deletion_rcpt_) spec.dopts |= DOPTS_DELETE_RCPT;
        spec.expiration = opts_.expiration_;

        dtn_bundle_payload_t payload;
        memset(&payload, 0, sizeof(payload));
        if (opts_.payload_data_len_ != 0) {
            dtn_set_payload(&payload, DTN_PAYLOAD_MEM,
                            opts_.payload_data_, opts_.payload_data_len_);
        } else {
            dtn_set_payload(&payload, DTN_PAYLOAD_FILE,
                            opts_.payload_file_, opts_.payload_file_len_);

        }

        dtn_bundle_id_t id;
        memset(&id, 0, sizeof(id));
        
        int ret = dtn_send(h, &spec, &payload, &id);
        if (ret != DTN_SUCCESS) {
            resultf("error in dtn_send: %s",
                    dtn_strerror(dtn_errno(h)));
            return TCL_ERROR;
        }

        resultf("%s,%u.%u",
                id.source.uri, id.creation_secs, id.creation_subsecs);
        return TCL_OK;
    }
};

//----------------------------------------------------------------------
class DTNBindCommand : public oasys::TclCommand {
public:
    DTNBindCommand() : TclCommand("dtn_bind") {}
    
    int exec(int argc, const char **argv,  Tcl_Interp* interp)
    {
        (void)interp;

        if (argc != 3) {
            wrong_num_args(argc, argv, 1, 3, 3);
            return TCL_ERROR;
        }

        int n = atoi(argv[1]);
        HandleMap::iterator iter = State::instance()->handles_.find(n);
        if (iter == State::instance()->handles_.end()) {
            resultf("invalid dtn handle %d", n);
            return TCL_ERROR;
        }

        dtn_handle_t h = iter->second;

        dtn_reg_id_t regid = atoi(argv[2]);

        int err = dtn_bind(h, regid);
        if (err != DTN_SUCCESS) {
            resultf("error in dtn_bind: %s",
                    dtn_strerror(dtn_errno(h)));
            return TCL_ERROR;
        }
        
        return TCL_OK;
    }
};

//----------------------------------------------------------------------
class DTNUnbindCommand : public oasys::TclCommand {
public:
    DTNUnbindCommand() : TclCommand("dtn_unbind") {}
    
    int exec(int argc, const char **argv,  Tcl_Interp* interp)
    {
        (void)interp;

        if (argc != 3) {
            wrong_num_args(argc, argv, 1, 3, 3);
            return TCL_ERROR;
        }

        int n = atoi(argv[1]);
        HandleMap::iterator iter = State::instance()->handles_.find(n);
        if (iter == State::instance()->handles_.end()) {
            resultf("invalid dtn handle %d", n);
            return TCL_ERROR;
        }

        dtn_handle_t h = iter->second;

        dtn_reg_id_t regid = atoi(argv[2]);

        int err = dtn_unbind(h, regid);
        if (err != DTN_SUCCESS) {
            resultf("error in dtn_unbind: %s",
                    dtn_strerror(dtn_errno(h)));
            return TCL_ERROR;
        }
        
        return TCL_OK;
    }
};

//----------------------------------------------------------------------
class DTNRecvCommand : public oasys::TclCommand {
public:
    oasys::OptParser parser_;

    struct RecvOpts {
        bool   payload_mem_;
        bool   payload_file_;
        u_int  timeout_;
    };
    
    RecvOpts opts_;

    void init_opts() {
        memset(&opts_, 0, sizeof(opts_));
    }

    DTNRecvCommand() : TclCommand("dtn_recv")
    {
        parser_.addopt(new oasys::BoolOpt("payload_mem", &opts_.payload_mem_));
        parser_.addopt(new oasys::BoolOpt("payload_file", &opts_.payload_file_));
        parser_.addopt(new oasys::UIntOpt("timeout", &opts_.timeout_));
    }
    
    int exec(int argc, const char **argv,  Tcl_Interp* interp)
    {
        (void)argc;
        (void)argv;
        (void)interp;

        // need at least cmd, handle, payload, and timeout
        if (argc < 3) {
            wrong_num_args(argc, argv, 1, 3, INT_MAX);
            return TCL_ERROR;
        }

        int n = atoi(argv[1]);
        HandleMap::iterator iter = State::instance()->handles_.find(n);
        if (iter == State::instance()->handles_.end()) {
            resultf("invalid dtn handle %d", n);
            return TCL_ERROR;
        }

        dtn_handle_t h = iter->second;

        init_opts();

        const char* invalid = 0;
        if (! parser_.parse(argc - 2, argv + 2, &invalid)) {
            resultf("invalid option '%s'", invalid);
            return TCL_ERROR;
        }

        if (opts_.payload_mem_ == false && opts_.payload_file_ == false) {
            resultf("must set payload location");
            return TCL_ERROR;
        }

        dtn_bundle_spec_t spec;
        memset(&spec, 0, sizeof(spec));

        dtn_bundle_payload_t payload;
        memset(&payload, 0, sizeof(payload));
        
        int err = dtn_recv(h, &spec,
                           opts_.payload_mem_ ? DTN_PAYLOAD_MEM : DTN_PAYLOAD_FILE,
                           &payload, opts_.timeout_);
        if (err != DTN_SUCCESS) {
            resultf("error in dtn_recv: %s",
                    dtn_strerror(dtn_errno(h)));
            return TCL_ERROR;
        }
        
        dtn_free_payload(&payload);
        
        // XXX/demmer should fill in the return with something useful, no?
        
        return TCL_OK;
    }
};

//----------------------------------------------------------------------
class ShutdownCommand : public oasys::TclCommand {
public:
    ShutdownCommand() : TclCommand("shutdown") {}
    static void call_exit(void* clientData);
    int exec(int argc, const char **argv,  Tcl_Interp* interp);
};

void
ShutdownCommand::call_exit(void* clientData)
{
    (void)clientData;
    oasys::TclCommandInterp::instance()->exit_event_loop();
}

//----------------------------------------------------------------------
int
ShutdownCommand::exec(int argc, const char **argv, Tcl_Interp* interp)
{
    (void)argc;
    (void)argv;
    (void)interp;
    Tcl_CreateTimerHandler(0, ShutdownCommand::call_exit, 0);
    return TCL_OK;
}

//----------------------------------------------------------------------
int
main(int argc, char** argv)
{
    oasys::TclCommandInterp* interp;
    oasys::ConsoleCommand* console_cmd;
    std::string conf_file;
    bool conf_file_set = false;
    bool daemon = false;

    oasys::Log::init();
    
    oasys::TclCommandInterp::init("dtn-test");
    interp = oasys::TclCommandInterp::instance();
    
    oasys::Getopt::addopt(
        new oasys::StringOpt('c', "conf", &conf_file, "<conf>",
                             "set the configuration file", &conf_file_set));

    oasys::Getopt::addopt(
        new oasys::BoolOpt('d', "daemon", &daemon,
                           "run as a daemon"));
    
    int remainder = oasys::Getopt::getopt(argv[0], argc, argv);
    if (remainder != argc) 
    {
        fprintf(stderr, "invalid argument '%s'\n", argv[remainder]);
        oasys::Getopt::usage("dtn-test");
        exit(1);
    }

    console_cmd = new oasys::ConsoleCommand();
    interp->reg(console_cmd);
    interp->reg(new DTNOpenCommand());
    interp->reg(new DTNCloseCommand());
    interp->reg(new DTNRegisterCommand());
    interp->reg(new DTNUnregisterCommand());
    interp->reg(new DTNBindCommand());
    interp->reg(new DTNUnbindCommand());
    interp->reg(new DTNSendCommand());
    interp->reg(new DTNRecvCommand());
    interp->reg(new ShutdownCommand());

    if (conf_file_set) {
        interp->exec_file(conf_file.c_str());
    }
    
    log_notice("/dtn-test", "dtn-test starting up...");
    
    if (console_cmd->port_ != 0) {
        log_notice("/dtn-test", "starting console on %s:%d",
                   intoa(console_cmd->addr_), console_cmd->port_);
        interp->command_server("dtn-test", console_cmd->addr_, console_cmd->port_);
    }

    if (daemon || (console_cmd->stdio_ == false)) {
        oasys::TclCommandInterp::instance()->event_loop();
    } else {
        oasys::TclCommandInterp::instance()->command_loop("dtn-test");
    }

    log_notice("/dtn-test", "dtn-test shutting down...");
    delete State::instance();
    oasys::TclCommandInterp::shutdown();
    oasys::Log::shutdown();
}
