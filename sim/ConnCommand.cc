/*
 *    Copyright 2005-2006 Intel Corporation
 * 
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 * 
 *        http://www.apache.org/licenses/LICENSE-2.0
 * 
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */


#include <stdlib.h>

#include "Connectivity.h"
#include "ConnCommand.h"
#include "Simulator.h"
#include "Topology.h"

namespace dtnsim {

ConnCommand::ConnCommand()
    : TclCommand("conn")
{
    bind_var(new oasys::StringOpt("type", &Connectivity::type_,
                                  "type", "Connectivity type."));
    
    add_to_help("up", "Take connection up XXX");
    add_to_help("down", "Take connection down XXX");
}

int
ConnCommand::exec(int argc, const char** argv, Tcl_Interp* tclinterp)
{
    (void)tclinterp;
    
    if (argc < 3) {
        wrong_num_args(argc, argv, 2, 4, INT_MAX);
        return TCL_ERROR;
    }
    
    // pull out the time
    char* end;
    double time = strtod(argv[1], &end);
    if (*end != '\0') {
        resultf("time value '%s' invalid", argv[1]);
        return TCL_ERROR;
    }

    const char* cmd = argv[2];

    Connectivity* conn = Connectivity::instance();

    if (!strcmp(cmd, "up") || !strcmp(cmd, "down")) {
        // conn <time> <up|down> <n1> <n2> <args>
        if (argc < 5) {
            wrong_num_args(argc, argv, 2, 5, INT_MAX);
            return TCL_ERROR;
        }

        const char* n1_name = argv[3];
        const char* n2_name = argv[4];

        if (strcmp(n1_name, "*") != 0 &&
            Topology::find_node(n1_name) == NULL)
        {
            resultf("invalid node or wildcard '%s'", n1_name);
            return TCL_ERROR;
        }
        
        if (strcmp(n2_name, "*") != 0 &&
            Topology::find_node(n2_name) == NULL)
        {
            resultf("invalid node or wildcard '%s'", n2_name);
            return TCL_ERROR;
        }
        
        ConnState* s = new ConnState();
        const char* invalid;
        if (! s->parse_options(argc - 5, argv + 5, &invalid)) {
            resultf("invalid option '%s'", invalid);
            delete s;
            return TCL_ERROR;
        }
        
        s->open_ = !strcmp(cmd, "up");

        Simulator::post(
            new SimConnectivityEvent(time, conn, n1_name, n2_name, s));

        return TCL_OK;

    } else {
        // dispatch to the connectivity module itself
        if (! conn->exec(argc - 3, argv + 3)) {
            resultf("conn: error handling command");
            return TCL_ERROR;
        }


        return TCL_OK;
    }
    
    resultf("conn: unsupported subcommand %s", cmd);
    return TCL_ERROR;
}


} // namespace dtnsim
