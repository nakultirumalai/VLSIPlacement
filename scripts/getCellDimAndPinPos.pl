#!/usr/bin/perl

#use strict;
#use warnings;

my $bookshelf = shift(@ARGV);
my $lefFile = shift(@ARGV);

my $mod_filename = $bookshelf;
$mod_filename =~ s/\.nodes$//;

my $PAD_HEIGHT = 56;
my $PAD_WIDTH = 56;

(system("mv ${mod_filename}.nodes ${mod_filename}.nodes.copy") == 0) || die ("Cannot move of the existing nodes file");
open(bkshlfFile, "${mod_filename}.nodes.copy") || die ("Can't open $bookshelf file");

my $mod_filename_1 = $mod_filename.".nodes";
my $mod_filename_2 = $mod_filename.".nodes.flowplace";

open(mod_bkshlfFile_1, ">$mod_filename_1") || die ("Can't open file for writing");
open(mod_bkshlfFile_2, ">$mod_filename_2") || die ("Can't open file for writing");

my $count = 0;
my @libCellArray;
my %cellIsFFTest;

while(my $row = <bkshlfFile>) {
    if ($row =~ /^(#(.*)|\n|NumNodes|NumTerminals|UCLA)/) {
	print mod_bkshlfFile_1 "$row";
	print mod_bkshlfFile_2 "$row";
    } else {
	chomp($row);
    	$row =~ s/^\s*//; 
    	my ($name, $lib_name, $type) = split('\s+', $row);
	if ($lib_name =~ m/\s*PAD\s*/) {
	    push @libCellArray, [ $name, $lib_name ];
	} else {
	    my @splitlib_names = split('/',$lib_name);
	    my $cell_name = $splitlib_names[1];
	    
	    push @libCellArray, [ $name, $cell_name ];
	    if ($type eq "FF") {
		$cellIsFFTest{$name} = 1;
	    }
	}
	$count++;
    }
}

print "Read $count nodes from bookshelf file\n";
close(bkshlfFile);

my %cellDimensionInfo;
my %cellPortInfo;
my @rectArray;
my $cellDefBegin=0;
my $pinDefBegin=0;
my $portDefBegin = 0;
my $cellName = "";
my $pinName = "";

open(inFile, $lefFile) || die ("Cannot open file $lefFile");
$count=0;
while(my $line = <inFile>)
{
    chomp($line);
    if ($line =~ m/\s*MACRO\s*(.*)/) {
	if ($cellDefBegin == 1) {
	    die ("Error in LEF file!");
	}
	$cellName = $1;
#	print "CELL Name : $cellName\n";
	$cellDefBegin=1;
	$count++;
    } 
    if (($cellDefBegin == 1) and ($cellName ne "")) {
#	my $pinPosHash;
	# key = pin name ; value = array of rect arrays;
	if ($line =~ m/\s*SIZE\s*([0-9]+\.[0-9]+)\s*BY\s*([0-9]+\.[0-9]+).*/) {
	    $cellDimensionInfo{$cellName} = [ $1, $2 ];
	    $cellPortInfo{$cellName} = { };
	    $cellPortInfo{$cellName}{"SIZE"} = [ $1 , $2 ];
	}
	if ($line =~ m/\s*PIN\s(.*)/) {
	    $pinName = $1;
	    $pinDefBegin = 1;
#	    print "\tPIN Name: $pinName \n";
	    $cellPortInfo{$cellName}{$pinName} = [ ];
	    next;
	}
	if (($pinDefBegin == 1) and ($pinName ne "")) {
	    if ($line =~ m/\s*PORT\s*/) {
		$portDefBegin = 1;
		next;
#		print "\t\tPORT \n";
	    }
	    if (($portDefBegin == 1)) {
		if ($line =~ m/\s*END\s*/) {
		    $portDefBegin = 0;
		} elsif ($line =~ m/\sRECT\s([0-9]+\.[0-9]+)\s*([0-9]+\.[0-9]+)\s*([0-9]+\.[0-9]+)\s*([0-9]+\.[0-9]+)\s*/) {
		    my $rectArray = [ $1, $2, $3, $4 ];
#		    print "\t\t\tRECT  ";
#		    print @$rectArray;
#		    print "\n";
#		    my $arrRef = $cellPortInfo{$cellName,
		    push (@{$cellPortInfo{$cellName}{$pinName}}, $rectArray);
#		    my $rectArrOfArrs = $cellPortInfo{$cellName}{$pinName};
#		    push(@$rectArrOfArrs, \@rectArray);
		}
	    }
	}
	if ($line =~ m/\s*END\s*(.*)/) {
	    if ($1 eq $pinName) {
		$pinDefBegin = 0;
#		my $map2PinPosHash = $cellPortInfo{$cellname};
#		$map2PinPosHash->{$pinName} = $pinPosHash;
	    }
	}
    }

    if (($cellName ne "") and ($line =~ m/\s*END\s*(.*)/)) {
	if ($cellName eq $1) {
	    $cellDefBegin=0;
	    $cellName = "";
	}
    }
}

close(inFile);

print "Read $count cells from LEF file\n";
if (scalar(@libCellArray) != 0) {
    foreach my $cellLibNameRef (@libCellArray) {
	my @cellLibNameArr = ( @$cellLibNameRef );
	my $isPad = "false";
	$cellName = $cellLibNameArr[0];
	my $libCellName = $cellLibNameArr[1];
#	print "DBG: $cellName : $libCellName\n";
	if ($libCellName =~ m/\s*PAD\s*/) {
	    $isPad = "true";
	}
	my $cellWidth;
	my $cellHeight;
	if ($isPad eq "false") {
	    my $cellDataRef = $cellDimensionInfo{$libCellName};
	    my @cellData = ( @$cellDataRef );
	    $cellWidth = $cellData[0] * 1000;
	    $cellHeight = $cellData[1] * 1000;
	} else {
	    $cellWidth = $PAD_WIDTH;
	    $cellHeight = $PAD_HEIGHT;
	}
	if ($isPad eq "false") {
	    print mod_bkshlfFile_1 "  $cellName  \t$cellWidth\t$cellHeight\n";
	    if (exists $cellIsFFTest{$cellName}) {
		print mod_bkshlfFile_2 "  $cellName\t$cellWidth\t$cellHeight\tFF\n";
	    } else {
		print mod_bkshlfFile_2 "  $cellName\t$cellWidth\t$cellHeight\tnFF\n";
	    }
	} else {
#	    print mod_bkshlfFile_1 "\t$cellName\t$cellWidth\t$cellHeight\tterminal\n";
#	    print mod_bkshlfFile_2 "\t$cellName\t$cellWidth\t$cellHeight\tnFF\tterminal\n";
	    print mod_bkshlfFile_1 "  $cellName\t$cellWidth\t$cellHeight\tterminal \n";
	    print mod_bkshlfFile_2 "  $cellName\t$cellWidth\t$cellHeight\tterminal\tnFF\n";
	}
    }
}

if (0) {
    foreach my $cellName ( keys %cellPortInfo ) {
#    print "CELL : $cellName\n";
#    print "NUM PINS :";
#    print scalar ( keys $cellPortInfo{$cellName} );
#    print "\n";
	foreach my $pinName ( keys $cellPortInfo{$cellName} ) {
	    if ($pinName eq "SIZE") {
#	    print "\tSIZE: ";
#	    print ${$cellPortInfo{$cellName}{"SIZE"}}[0];
#	    print " ";
#	    print ${$cellPortInfo{$cellName}{"SIZE"}}[1];
#	    print "\n";
		next;
	    }
#	print "\tPIN: $pinName\n";
	    foreach my $rectArrOfArrs ( $cellPortInfo{$cellName}{$pinName} ) {
#	    print "\t\tRECT ARRAY SIZE: ";
#	    print scalar ( @$rectArrOfArrs );
#	    print "\n";
		foreach my $rectArr ( @$rectArrOfArrs ) {
#		print "\t\tRECT: ";
#		print @$rectArr;
#		print "\n";
		}
	    }
	}
    }
}

# Parse the nets file here to replace the positions of all the pins
(system("mv ${mod_filename}.nets ${mod_filename}.nets.copy") == 0) || die ("Cannot move of the existing nodes file");
$netOrigFile = $mod_filename.".nets.copy";
$netFile = $mod_filename.".nets";
$pinMapFile = $mod_filename.".pins.map";
open(netOrigFile, $netOrigFile) || die ("Cannot open file $netFile");
open(netFile, ">$netFile") || die ("Cannot open file $netFile for writing");
open(pinMapFile, ">$pinMapFile") || die("Cannot open file $pinMapFile for writing");

my %pinCount;
while(my $line = <netOrigFile>) {
    if ($line =~ /^(#(.*)|\n|NumNets|NumPins|UCLA|NetDegree)/) {
	print netFile $line;
    } else {
	chomp($line);
    	$line =~ s/^\s*//; 
    	my ($cellName, $pinDir, $colon, $libDetails) = split('\s+', $line); 

	my $cellPinCount;
	if (exists $pinCount{$cellName}) {
	    $cellPinCount = $pinCount{$cellName};
	} else {
	    $pinCount{$cellName} = 0;
	    $cellPinCount = 0;
	}
	$cellPinCount = $cellPinCount + 1;
	$pinCount{$cellName} = $cellPinCount;

	if ($libDetails ne " ") {
	    my $pinx, $piny;
#	    print "RECOGNIZED: Cell: $cellName Lib details: $libDetails\n";
	    if ($libDetails eq "PAD") {
		$pinx = 28;
		$piny = 28;
	    } else {
		my ($libName, $libCellName, $libPinName) = split('/', $libDetails);
#		print "\tRECOGNIZED: libname: $libName Lib cell name: $libCellName Pin name: $libPinName \n";
		my $xc = ${$cellPortInfo{$libCellName}{"SIZE"}}[0];
		my $yc = ${$cellPortInfo{$libCellName}{"SIZE"}}[1];
#		print "\tSIZE OF CELL: $xc $yc \n";
		my $midx = $xc / 2;
		my $midy = $yc / 2;
#		print "\tMID POINT OF CELL: $midx $midy \n";
		my $rectArray = $cellPortInfo{$libCellName}{$libPinName};
		($pinx, $piny) = getClosestPos($midx, $midy, $rectArray);
		# Shift the origin to the center of the cell as bookshelf expects this
		$pinx = $pinx - $midx;
		$piny = $piny - $midy;
		# Round the pin position to 3 decimal places and convert to microns
		$pinx = sprintf "%.3f", $pinx;
		$pinx = $pinx * 1000;
		$pinx = sprintf "%.3f", $pinx;
		$piny = sprintf "%.3f", $piny;
		$piny = $piny * 1000;
		$piny = sprintf "%.3f", $piny;
#		print "\tPIN POSITION of pin $libPinName $pinx $piny \n";
		print pinMapFile "${cellName}_${cellPinCount} $libPinName\n";
	    }
	    print netFile "\t$cellName  $pinDir  $colon  $pinx  $piny \n";
	}
    }
}

sub getClosestPos{
    my ($xc, $yc, $arrRef) = @_; 
    my $minDist;
    my ($Xp,$Yp);
    my $isFirstPoint = "true";

    my $xPresent == "false";
    my $yPresent == "false";
    foreach my $rects(@$arrRef){
	my @rect_i = (@$rects);
	if(($xc >= $rect_i[0]) and ($xc <=$rect_i[2])){
	    $xPresent = "true";
	}
	if(($yc >= $rect_i[1]) and ($yc <=$rect_i[3])){
	    $yPresent = "true";
	}
    }
    if ($xPresent == "false" or $yPresent == "false") {
	foreach my $rect_i(@$arrRef){
	    my @eachRect = (@$rect_i);
	    my @fourCorners = ();
	    $fourCorners[0] = [$eachRect[0], $eachRect[1]];
	    $fourCorners[1] = [$eachRect[2], $eachRect[1]];
	    $fourCorners[2] = [$eachRect[2], $eachRect[3]];
	    $fourCorners[3] = [$eachRect[0], $eachRect[3]];
	    
	    for(my $i=0; $i<4; $i++){
		my $thisPoint = $fourCorners[$i];
		my @Point1 = (@$thisPoint);
		my $distSquared = getDist($xc, $yc, $thisPoint);
		if($isFirstPoint eq "true"){
		    $minDist = $distSquared;
		    $Xp = $Point1[0];
		    $Yp = $Point1[1];
		    $isFirstPoint = "false";
		}
		else{
		    if($distSquared < $minDist){
			$minDist = $distSquared;
			$Xp = $Point1[0];
			$Yp = $Point1[1];
		    }
		}
	    }
	}
    } else {
	$Xp = $Xc;
	$Yp = $Yp;
    }
    return ($Xp, $Yp);
    sub getDist{
	my ($Xc, $Yc, $pointArrRef) = @_;
	my @Point = (@$pointArrRef);
	my $distSquared = ((($Point[0]-$Xc) ** 2) + (($Point[1]-$Yc) ** 2));
	return $distSquared;
    }
}
