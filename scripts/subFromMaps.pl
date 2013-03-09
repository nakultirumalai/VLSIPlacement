#!/usr/bin/perl

#######################################################
# What this script does?
# Substitutes actual cell names in any file with pseudo names
# Usage:
# ./subFromMaps.pl <FileInWhichCellNamesHaveToBeSubstituted> fileMap.txt

use strict;
use warnings;

my $targetFile = shift(@ARGV);
my $mapFile = shift(@ARGV);
my $isPaths ="false";
my $targetFile_2 = $targetFile;
if($targetFile_2 =~ m/paths$/){
    $isPaths="true";
}

system("mv ${targetFile} ${targetFile}.copy") == 0 || die ("Cannot execute copy command");

open(inmapFile, $mapFile) || die ("Cannot open $mapFile for reading");
my %cell2Pseudo=();
while(my $row = <inmapFile>)
{
    chomp($row);
    next if($row =~ m/^Pseudo_name/);
    my ($pseudo, $cellName, $libCellName, $type) = split('\t+',$row);
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
    }
    else
    {
	foreach my $cellName (@cellNames) {
            #my $cellNameBak = $cellName;
            if (exists $cell2Pseudo{$cellName}) {
                #    print "BEFORE: \t$line\n";
            
                $line =~ s/\Q$cellName\E/${cell2Pseudo{$cellName}}/g;
                #    print "AFTER:\t$line\n";
            }
        }
    }
    print targetFileHndl $line;
}

close (targetFileHndl);
close (targetFileCopy);

