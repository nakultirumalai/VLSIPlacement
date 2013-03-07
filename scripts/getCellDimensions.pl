#!/usr/bin/perl

#use strict;
#use warnings;

my $bookshelf = shift(@ARGV);
my $lefFile = shift(@ARGV);

my $mod_filename = $bookshelf;
$mod_filename =~ s/\.nodes$//;

system("mv ${mod_filename}.nodes ${mod_filename}.nodes.orig") == 0 || die ("Cannot execute copy command");
system("cp ${mod_filename}.nodes.orig ${mod_filename}.nodes.copy") == 0 || die ("Cannot execute copy command");

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
    	my @splitlib_names = split('/',$lib_name);
    	my $cell_name = $splitlib_names[1];
	
	push @libCellArray, [ $name, $cell_name ];
	if ($type eq "FF") {
	    $cellIsFFTest{$name} = 1;
	}
	$count++;
    }
}

print "Read $count nodes from bookshelf file\n";
close(bkshlfFile);

my %cellDimensionInfo;
my $cellDefBegin=0;
my $cellName = "";

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
	$cellDefBegin=1;
	$count++;
    } 
    if (($cellDefBegin == 1) and ($cellName ne "")) {
	if ($line =~ m/\s*SIZE\s*([0-9]+\.[0-9]+)\s*BY\s*([0-9]+\.[0-9]+).*/) {
	    $cellDimensionInfo{$cellName} = [ $1, $2 ];
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
	$cellName = $cellLibNameArr[0];
	my $cellDataRef = $cellDimensionInfo{$cellLibNameArr[1]};
	my @cellData = ( @$cellDataRef );
	# Convert um to nm
	my $cellWidth = $cellData[0] * 1000;
	my $cellHeight = $cellData[1] * 1000;
	print mod_bkshlfFile_1 "\t$cellName\t$cellWidth\t$cellHeight\n";
	if (exists $cellIsFFTest{$cellName}) {
	    print mod_bkshlfFile_2 "\t$cellName\t$cellWidth\t$cellHeight\tFF\n";
	} else {
	    print mod_bkshlfFile_2 "\t$cellName\t$cellWidth\t$cellHeight\tnFF\n";
	}
    }
}

