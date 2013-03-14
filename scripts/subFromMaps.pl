#!/usr/bin/perl
use strict;
use warnings;

my $targetFile = shift(@ARGV);
my $mapFile = shift(@ARGV);
my $isPaths ="false";
my $isNets = "false";
my $targetFile_2 = $targetFile;
if($targetFile_2 =~ m/paths$/){
    $isPaths="true";
}
if($targetFile_2 =~ m/nets$/){
    $isNets="true";
}

system("cp ${targetFile} ${targetFile}.copy") == 0 || die ("Cannot execute copy command");

open(inmapFile, $mapFile) || die ("Cannot open $mapFile for reading");
my %cell2Pseudo=();
while(my $row = <inmapFile>)
{
    chomp($row);
    next if($row =~ m/^Pseudo_name/);
    my ($pseudo, $cellName, $libCellName, $type) = split('\s+',$row);
    $cell2Pseudo{$cellName}= $pseudo;
}
close(inmapFile);

open (targetFileCopy, "$targetFile.copy") || die ("C");
open (targetFileHndl, ">$targetFile") || die ("C");

while (my $line = <targetFileCopy>) {
    my @cellNames = split(/\s+/, $line);
    
    if($isPaths eq "true")
    {
        foreach my $cellName (@cellNames) {
	    #my $cellNameBak = $cellName;
	    my ($cellNameCpy,$lastCell)  = ($cellName =~ /(.*)\/(.?)/); 
	   
	    if ((defined($cellNameCpy)) and (exists $cell2Pseudo{$cellNameCpy})) {
	        #    print "BEFORE: \t$line\n";
	    
	        $line =~ s/\Q$cellNameCpy\E/${cell2Pseudo{$cellNameCpy}}/g;
	        #    print "AFTER:\t$line\n";
	    }
	}
    } else {
	if (not (($isNets eq "true") and ($line =~ m/^\s*NetDegree.*/))) { 
	    foreach my $cellName (@cellNames) {
		#my $cellNameBak = $cellName;
		if (exists $cell2Pseudo{$cellName}) {
		    #    print "BEFORE: \t$line\n";
		    
		    $line =~ s/\Q$cellName\E/${cell2Pseudo{$cellName}}/g;
		    #    print "AFTER:\t$line\n";
		}
	    }
	}
    }
    print targetFileHndl $line;
}

close(targetFileHndl);
close(targetFileCopy);
system("rm -rf ${targetFile}.copy") == 0 || die ("Cannot remove target file copy");

