#
# Basic configuration proc for test dtnd's
#

namespace eval dtn {

proc config {args} {
    conf::add dtnd * {source "dtnd-test-utils.tcl"}

    # defaults
    set storage_type berkeleydb
    set null_link true
    
    set i 0
    while {$i < [llength $args]} {
	set arg [lindex $args $i]
	switch -- $arg {
	    -storage_type  -
	    --storage_type {
		set storage_type [lindex $args [incr i]]
	    }
	    -no_null_link -
	    --no_null_link {
		set null_link false
	    }

	    default {
		error "unknown dtn::config argument '$arg'"
	    }
	}

	incr i
    }
    
    dtn::standard_manifest
    dtn::config_console
    dtn::config_api_server
    dtn::config_storage $storage_type
    if {$null_link} {
	dtn::config_null_link
    }

    dtn::config_dtntest
}

#
# Standard manifest
#
proc standard_manifest {} {
    global opt
    manifest::file daemon/dtnd dtnd
    manifest::file [file join $opt(base_test_dir) \
	    test-utils/dtnd-test-utils.tcl] dtnd-test-utils.tcl
    manifest::dir  bundles
    manifest::dir  db
}

#
# Utility proc to get the adjusted port number for various things
#
proc get_port {what id} {
    global net::portbase net::used_ports
    set dtn_portbase $net::portbase($id)
    
    switch -- $what {
	console { set port [expr $dtn_portbase + 0] }
	api	{ set port [expr $dtn_portbase + 1] }
	tcp	{ set port [expr $dtn_portbase + 2] }
	udp	{ set port [expr $dtn_portbase + 2] }
	dtntest	{ set port [expr $dtn_portbase + 3] }
	default { return -1 }
    }

    lappend net::used_ports($id) $port
    return $port
}

#
# Utility proc to determine whether or not the given convergence layer
# is bidirectional
#
proc is_bidirectional {cl} {
    switch -- $cl {
	tcp { return 1 }
    }
    
    return 0
}

#
# Create a new interface for the given convergence layer
#
proc config_interface {cl args} {
    global net::listen_addr
    foreach id [net::nodelist] {
	set addr $net::listen_addr($id)
	set port [dtn::get_port $cl $id]
	
	conf::add dtnd $id [eval list interface add ${cl}0 $cl \
		local_addr=$addr local_port=$port $args]
    }
}

#
# Configure the console server
#
proc config_console {} {
    global opt net::listen_addr
    foreach id [net::nodelist] {
	conf::add dtnd $id "console set addr $net::listen_addr($id)"
	conf::add dtnd $id "console set port [dtn::get_port console $id]"
	if {! $opt(xterm)} {
	    conf::add dtnd $id "console set stdio false"
	}	    
    }
}

#
# Set up the API server
#
proc config_api_server {} {
    foreach id [net::nodelist] {
	conf::add dtnd $id "api set local_addr $net::listen_addr($id)"
	conf::add dtnd $id "api set local_port [dtn::get_port api $id]"
    }
}

#
# Set up a null link on each host
#
proc config_null_link {} {
    foreach id [net::nodelist] {
	conf::add dtnd $id "link add null /dev/null ALWAYSON null"
    }
}

#
# Configure storage
#
proc config_storage {type} {
    conf::add dtnd * [subst {
storage set tidy_wait  0
storage set payloaddir bundles
storage set type       $type
storage set dbname     DTN
storage set dbdir      db
    }]
}

#
# Configure dtntest
#
proc config_dtntest {} {
    foreach id [net::nodelist] {
	conf::add dtntest $id \
		"console set stdio false"
	conf::add dtntest $id \
		"console set addr $net::listen_addr($id)"
	conf::add dtntest $id \
		"console set port [dtn::get_port dtntest $id]"
    }
}

# namespace dtn
}
