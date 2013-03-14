#!/usr/bin/perl

#######################################################
# What this script does?
# Substitutes pseudo names in any file with actual cell names
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

system("mv ${targetFile} ${targetFile}.copy") == 0 || die ("Cannot execute move command");

open(inmapFile, $mapFile) || die ("Cannot open $mapFile for reading");
my %pseudo2Cell=();
while(my $row = <inmapFile>)
{
    chomp($row);
    next if($row =~ m/^Pseudo_name/);
    my ($pseudo, $cellName, $libCellName, $type) = split('\s+',$row);
    $pseudo2Cell{$pseudo}= $cellName;
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
	   
	    if ((defined($cellNameCpy)) and (exists $pseudo2Cell{$cellNameCpy})) {
	        #    print "BEFORE: \t$line\n";
	    
	        $line =~ s/\Q$cellNameCpy\E/${pseudo2Cell{$cellNameCpy}}/g;
	        #    print "AFTER:\t$line\n";
	    }
	}
    }
    else
    {
	foreach my $pseudoName (@cellNames) {
            #my $cellNameBak = $cellName;
            if (exists $pseudo2Cell{$pseudoName}) {
                #    print "BEFORE: \t$line\n";
                my $origCell = $pseudo2Cell{$pseudoName};
		$origCell =~ s/\\//g;
		#print "$origCell\n";
                $line =~ s/$pseudoName/$origCell/g;
                #    print "AFTER:\t$line\n";
            }
        }
    }
    print targetFileHndl $line;
}

close (targetFileHndl);
close (targetFileCopy);

(system("rm -rf $targetFile.copy") == 0) || die ("Cannot delete $targetFile.copy");

