
#include "BundleCommand.h"
#include "bundling/Bundle.h"
#include "bundling/BundleEvent.h"
#include "bundling/BundleForwarder.h"

BundleCommand::BundleCommand() : AutoCommandModule("bundle") {}

const char*
BundleCommand::help_string()
{
    return("bundle inject <source> <dest> <payload> <length?>");
}

int
BundleCommand::exec(int argc, const char** argv, Tcl_Interp* interp)
{
    // need a subcommand
    if (argc < 2) {
        wrong_num_args(argc, argv, 1, 2, INT_MAX);
        return TCL_ERROR;
    }

    const char* cmd = argv[1];

    if (strcmp(cmd, "inject") == 0) {
        // bundle inject <source> <dest> <payload> <length?>
        if (argc < 5 || argc > 6) {
            wrong_num_args(argc, argv, 2, 5, 6);
            return TCL_ERROR;
        }
        
        Bundle* b = new Bundle();
        b->source_.set_tuple(argv[2]);
        b->replyto_.set_tuple(argv[2]);
        b->custodian_.set_tuple(argv[2]);
        b->dest_.set_tuple(argv[3]);
        
        size_t len = strlen(argv[4]);
        size_t total = len;

        if (argc == 5) {
            b->payload_.set_data(argv[4], len);
        } else {
            char* end;
            total = strtoul(argv[5], &end, 10);

            if (*end != 0) {
                resultf("invalid length parameter %s", argv[5]);
                return TCL_ERROR;
            }
            
            if (total == 0) {
                resultf("invalid zero length parameter");
                return TCL_ERROR;
            }
            
            b->payload_.set_length(total);
            b->payload_.append_data(argv[4], len);
        }

        log_debug("inject %d byte bundle %s->%s", total, argv[2], argv[3]);
        BundleForwarder::post(new BundleReceivedEvent(b));
        return TCL_OK;
    } else {
        resultf("unknown bundle subcommand %s", cmd);
        return TCL_ERROR;
    }
}

