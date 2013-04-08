#!/usr/bin/perl

my $placedFile = shift(@ARGV); 
my $terminalPosFile = shift(@ARGV); 

open(termFile, $terminalPosFile) || die ("Cannot open file which has terminal positions");

my %portLocHash;
# Create a map of terminal positions
while(my $line = <termFile>) {
    chomp($line);
    if ($line =~ m/\s*(.*)\s+{([0-9]+\.[0-9]+)\s*([0-9]+\.[0-9]+)}\s*{([0-9]+\.[0-9]+)\s*([0-9]+\.[0-9]+)}\s*/) {
	my $portName = $1;
	# Convert positions from microns to nanometers 
	my $xlow = $2 * 1000;
	my $ylow = $3 * 1000;
	my $xhigh = $4 * 1000;
	my $yhigh = $5 * 1000;

#	my $relCX = ($xhigh - $xlow) / 2;
#	my $relCY = ($yhigh - $ylow) / 2;
	my $relCX = 0;
	my $relCY = 0;
#	print "DBG: Relative X : $relCX Relative Y : $relCY\n";
	my $termX = $xlow + $relCX;
	my $termY = $ylow + $relCY;
	
#	print "DBG: Added \"$portName\" of bbox {$2 $3} {$4 $5} and actual positions $termX $termY\n";
	$portLocHash{$portName} = [ $termX, $termY ];
    }
}

close(termFile);
$placedFileCpy = $placedFile.".copy";

(system("cp $placedFile $placedFileCpy") == 0) || die ("Cannot make a copy of the PL file");
open(plFileCpy, $placedFileCpy) || die ("Cannot open pl file");
open(plFile, ">$placedFile") || die ("Cannot open pl file");
# Replace the positions of terminals from the MAP
while (my $line = <plFileCpy>) {
    chomp($line);
    if ($line =~ m/\s*(.*)\s+([0-9]+\.?[0-9]*)\s*([0-9]+\.?[0-9]*)\s*:\s*(.*)\s*PAD\s*/) {
	my $portName = $1;
	my $portX = $2;
	my $portY = $3;
	my $portOrient = $4;
	if (exists $portLocHash{$portName}) {
	    my @portPosArr = @{$portLocHash{$portName}};
	    my $portPosX = $portPosArr[0];
	    my $portPosY = $portPosArr[1];
	    print plFile "$portName\t$portPosX\t$portPosY\t: $portOrient \n";
	} else {
	    print plFile "$line\n";
	}
    } else {
	print plFile "$line\n";
    }
}

close(plFile);
close(plFileCpy);
