#!/usr/bin/tclsh

#
#    Copyright 2005-2006 Intel Corporation
# 
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
# 
#        http://www.apache.org/licenses/LICENSE-2.0
# 
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#



# usage: test-utils/run-tcl-tests.tcl <test group> <run-dtn.tcl options?>
if {[llength $argv] < 1} {
    puts "test-utils/run-tcl-tests.tcl <test group> options..."
    puts ""
    puts "Required:"
    puts "    <test group> Test group to run"
    puts ""
    exit
}

# the basic test group
set tests(basic) {
    "alwayson-links.tcl"	""
    "api-leak-test.tcl"		""
    "bidirectional.tcl"         ""
    "bundle-status-reports.tcl"	""
    "custody-transfer.tcl"      ""
    "dtn-ping.tcl"		""
    "flood-router.tcl"		""
    "inflight-expiration.tcl"	""
    "ipc-version-test.tcl"	""
    "loopback-link.tcl"		""
    "many-bundles.tcl"		""
    "multipath-forwarding.tcl"	""
    "ondemand-links.tcl"	""
    "no-duplicate-send.tcl"	""
    "reactive-fragmentation.tcl" ""
    "send-one-bundle.tcl"	"-cl tcp -length 0"
    "send-one-bundle.tcl"	"-cl tcp -length 5 -segmentlen 1"
    "send-one-bundle.tcl"	"-cl tcp"
    "send-one-bundle.tcl"	"-cl udp -length 0"
    "send-one-bundle.tcl"	"-cl udp"
    "storage.tcl"		""
    "storage.tcl"		"-storage_type filesysdb"
    "tcp-bogus-link.tcl"	""
    "version-mismatch.tcl"	""
#    "writeblocked.tcl"		""
}

# the performance test group
set tests(perf) {
    "dtn-perf.tcl"		""
    "dtn-perf.tcl"		"-payload_len 50B"
    "dtn-perf.tcl"		"-payload_len 5MB  -file_payload"
    "dtn-perf.tcl"		"-payload_len 50MB -file_payload"
    "dtn-perf.tcl"		"-storage_type filesysdb"
    "dtn-perf.tcl"		"-payload_len 50B -storage_type filesysdb"
    "dtn-perf.tcl"		"-payload_len 5MB  -file_payload -storage_type filesysdb"
    "dtn-perf.tcl"		"-payload_len 50MB -file_payload -storage_type filesysdb"
}

# check test group
set group [lindex $argv 0]
set common_options [lrange $argv 1 end]

if {![info exists tests($group)]} {
    puts "Error: test group \"$group\" not defined"
    exit -1
}

# print a "Table of Contents" first before running the tests
puts "***"
puts "*** The following tests will be run:"
puts "***"
foreach {test testopts} $tests($group) {
    puts "***   $test $testopts"
}
puts "***\n"

# run the tests
foreach {test testopts} $tests($group) {
    puts "***"
    puts "*** Running Test: $test $testopts"
    puts "***"

    if {$testopts != ""} {
	set options "$common_options --opts \"$testopts\""
    } else {
	set options $common_options
    }
    
    if [catch {
	eval exec [file dirname $argv0]/run-dtn.tcl test/$test -d $options \
		< /dev/null >&@ stdout
    } err ] {
	puts "unexpected error: $err"
    }
}


