#!/bin/csh

#Use by base-cmd.sh. dont use it directly

echo "Inside TCP specific setting " >> $info

# Now generate daemon for the node
if ($id == 1) then  
    set dst = node-$maxnodes
    if ($perhop == 1) then  
	set dst =  node-2-link-1
    endif
    
    echo "executing ftp: client $txdir  $logroot/ftplog.$id $dst  " >> $info
    tclsh $dtn2root/test/simple-ftp.tcl client $txdir  $logroot/ftplog.$id $dst  >>& $info

    exit

endif

if ($id == $maxnodes) then 
    echo "executing ftp: server $rcvdir  $logroot/ftplog.$id  " >> $info
    tclsh $dtn2root/test/simple-ftp.tcl server $rcvdir  $logroot/ftplog.$id >>& $info
    exit
endif     

## For other nodes which are in between
if ($perhop == 1) then
    ## set up TCP forwarding proxies
    set idplus  = `expr $id + 1`
    set idminus = `expr $id - 1`

    set myip = node-$id-link-$idminus
    set dstip = node-$idplus-link-$id
    set rconf = "$logroot/$id.rinetd-conf"
    echo $myip 17600 $dstip 17600 > $rconf 

    echo " executing rinetd at router ($myip, $dstip) " >> $info 
    echo " rule is:  " >> $info
    cat $rconf >> $info 
    $dtnroot/rinetd/rinetd -c $rconf ;

endif
 
