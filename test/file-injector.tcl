#
# Simple test code that scans a directory for new data and injects
# whatever it finds as a bundle.
#

#
# Starts up a file injector to scan the given directory and create
# new bundles from any files that are found.
#
# usage: file_injector <dir> <source> <dest> <-period ms>
#
proc file_injector_start {dir source dest args} {
    global file_injector_handles
    
    set period 2000
    
    if {$args != {}} {
	error "XXX/demmer implement optional args"
    }

    set after_handle [after 0 \
	    [list file_injector_scan $dir $source $dest $period]]
    set file_injector_handles($dir) $after_handle

    return $dir
}

#
# Stops a file injector on the given directory
#
proc file_injector_stop {dir} {
    global file_injector_handles

    if [info exists file_injector_handles($dir)] {
	after cancel $file_injector_handles($dir)
    } else {
	error "no injector running on dir $dir"
    }
}

#
# Internal proc that actually does the scanning
#
proc file_injector_scan {dir source dest period} {
    global file_injector_handles
    
    set files [glob -nocomplain -- "$dir/*"]

    log /file_injector DEBUG "scanning $dir... found [llength $files] files"
    foreach file $files {
	file stat $file file_stat
	
	log /file_injector DEBUG "stat type $file_stat(type)"
	
	if {$file_stat(type) != "file"} {
	    log /file_injector DEBUG \
		    "ignoring file $file type $file_stat(type)"
	    continue
	}

	if {$file_stat(size) == 0} {
	    log /file_injector DEBUG "ignoring empty file $file"
	    continue
	}

	log /file_injector DEBUG "reading payload from bundle file $file"
	set fd [open $file]
	set payload [read $fd]
	close $fd

	log /file_injector DEBUG "got [string length $payload] length payload"

	bundle inject $source $dest $payload

	file delete $file
    }

    # re-schedule the scan
    set after_handle [after $period \
	    [list file_injector_scan $dir $source $dest $period]]
    set file_injector_handles($dir) $after_handle
    return ""
}
