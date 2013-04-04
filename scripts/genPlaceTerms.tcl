proc genTermPlacement {termPlaceFile} {
    set plFile [open $termPlaceFile "w"]

    set ports [get_terminals]

    foreach_in_collection port $ports {
	set portName [get_object_name $port]
	set bbox [get_attribute $port bbox]
	set cellPlOutput "$portName $bbox"

	puts ${plFile} ${cellPlOutput}
    }

    close ${plFile}
 }

