#
# set up routing...
#

proc setup_interface {cl} {
    global hosts id
    
    route local_tuple bundles://internet/host://$hosts($id)
    
    interface add $cl host://$hosts($id):5000
}

proc setup_linear_topology {type cl {args ""}} {
    global hosts num_nodes id

    # tcp route to next_hop in chain. can just use the peer
    if { $id != [expr $num_nodes - 1] } {
	set peerid [expr $id + 1]
	set peeraddr $hosts($peerid)
	eval link add link-$peerid bundles://internet/host://$peeraddr:5000 \
		$type $cl $args
	route add bundles://internet/host://$peeraddr/* link-$peerid
    }

    # and previous a hop in chain as well
    if { $id != 0 } {
	set peerid [expr $id - 1]
	set peeraddr $hosts($peerid)
	eval link add link-$peerid bundles://internet/host://$peeraddr:5000 \
		$type $cl $args

	route add bundles://internet/host://$peeraddr/* link-$peerid
    }
}

