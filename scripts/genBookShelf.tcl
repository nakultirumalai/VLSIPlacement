proc genBookShelf {nodesFileStr netsFileStr plFileStr indicateFF} {
    set nodesFile [open $nodesFileStr "w"]
    set netsFile [open $netsFileStr "w"]
    set plFile [open $plFileStr "w"]

    # Write the Banner in both the nodes file and the PL file
    puts ${nodesFile} "UCLA nodes 1.0" 
    puts ${nodesFile} "# Created      : Fri Mar 15 15:32:16 2013"
    puts ${nodesFile} "# User         : tirumanl@arugula"
    puts ${nodesFile} "# Platform     : Linux 3.2.0-35-generic #55-Ubuntu SMP Wed Dec 5 17:42:16 UTC 2012 x86_64\n"

    puts ${plFile} "UCLA pl 1.0" 
    puts ${plFile} "# Created      : Fri Mar 15 15:32:16 2013"
    puts ${plFile} "# User         : tirumanl@arugula"
    puts ${plFile} "# Platform     : Linux 3.2.0-35-generic #55-Ubuntu SMP Wed Dec 5 17:42:16 UTC 2012 x86_64\n"

    set nodes [get_cells * -hier]
    set seqCells [get_cells * -hier -filter "is_sequential==true"]
    set hierCells [get_cells * -hier -filter "is_hierarchical==true"]

    array unset seqCellHash
    array unset hierCellHash

    foreach_in_collection cell $seqCells {
	set cellName [get_object_name $cell]
	set seqCellHash("$cellName") 1
    }
    foreach_in_collection cell $hierCells {
	set cellName [get_object_name $cell]
	set hierCellHash("$cellName") 1
    }

    set ports [get_ports *]
    set numCells [sizeof_collection $nodes]
    set numPorts [sizeof_collection $ports]
    set numCells [expr $numCells + $numPorts]
    set numTerminals [sizeof_collection $ports]
    
    puts ${nodesFile} "NumNodes     : $numCells "
    puts ${nodesFile} "NumTerminals : $numTerminals  "

    foreach_in_collection cell $nodes {
	set cellName [get_object_name $cell]
	set libCellName [get_object_name [get_lib_cells -of $cell]]
	set nodeOutputLine "\t$cellName $libCellName  "
	set cellPlOutput "$cellName\t0\t0\t: N"
	if {$indicateFF == "true"} {
	    if {[info exists hierCellHash("$cellName")]} {
		continue
	    }
	    if {[info exists seqCellHash("$cellName")]} {
		set nodeOutputLine "${nodeOutputLine} FF"
	    } else {
		set nodeOutputLine "${nodeOutputLine} nFF"
	    }
	    puts ${nodesFile} ${nodeOutputLine}
	}
	puts ${plFile} ${cellPlOutput}
    }

   foreach_in_collection port $ports {
	set portName [get_object_name $port]
	set libCellName "PAD"
	set nodeOutputLine "\t$portName $libCellName "
	set cellPlOutput "$portName\t0\t0\t: N PAD"
	puts ${nodesFile} ${nodeOutputLine}
	puts ${plFile} ${cellPlOutput}
    }
    
    # Write the nets next
    puts ${netsFile} "UCLA nets 1.0" 
    puts ${netsFile} "# Created      : Fri Mar 15 15:32:16 2013"
    puts ${netsFile} "# User         : tirumanl@arugula"
    puts ${netsFile} "# Platform     : Linux 3.2.0-35-generic #55-Ubuntu SMP Wed Dec 5 17:42:16 UTC 2012 x86_64\n"

    set designNets [get_nets * -hier]
    set numNets [sizeof_collection $designNets]
    set numTotalPins 0;
    
    foreach_in_collection net $designNets {
        set netPins [get_pins -of $net]
        set netPorts [get_ports -of $net]
	set numPins [sizeof_collection $netPins]
	set numPorts [sizeof_collection $netPorts]
	set numTotalPins [expr $numTotalPins + $numPins]
	set numTotalPins [expr $numTotalPins + $numPorts]
    }
	
    puts ${netsFile} "NumNets : $numNets"
    puts ${netsFile} "NumPins : $numTotalPins\n"
    
    foreach_in_collection net $designNets {
        set netPins [get_pins -of $net]
	set netPorts [get_ports -of $net]
	set netName [get_object_name $net]
	set numPins [sizeof_collection $netPins]
	set numPorts [sizeof_collection $netPorts]

	set numPins [expr $numPins + $numPorts]

	puts ${netsFile} "NetDegree : $numPins   $netName"
	foreach_in_collection pin $netPins {
	    set libPinName [get_object_name [get_lib_pin -of $pin]]
	    set pinDir [get_attribute [get_pins $pin] "pin_direction"]
	    set cell [get_cell -of $pin]
	    set cellName [get_object_name $cell]
	    if {$pinDir == "in"} {
		set pinDir "I"
	    } elseif {$pinDir == "out"} {
		set pinDir "O"
	    } else {
		set pinDir "B"
	    }
	    puts ${netsFile} "\t$cellName $pinDir : $libPinName"
	}
	foreach_in_collection port $netPorts {
	    set portDir [get_attribute $port direction]
	    set portName [get_object_name $port]
	    if {$portDir == "in"} {
		set portDir "O"
	    } elseif {$portDir == "out"} { 
		set portDir "I"
	    } else {
		set portDir "B"
	    }
	    puts ${netsFile} "\t$portName $portDir : PAD"
	}
    }
    close ${nodesFile}
    close ${netsFile}
 }
