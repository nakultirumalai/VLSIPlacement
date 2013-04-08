#!/usr/bin/perl -w

# parameter : (original DEF, .pl file from Capo, final DEF)
#open(NODES,"$ARGV[0]") || die "Can't open $_[0] for input\n";
open(ORIG,"$ARGV[0]") || die "Can't open $ARGV[0] for input\n";
open(PLACED,"$ARGV[1]") || die "Can't open $ARGV[1] input\n";
open(OUT,"> $ARGV[2]") || die "Can't open  $ARGV[2] output\n";

my %XLOCS;
my %YLOCS;
my %ORIENTS;
my %WIDTHS;
my %HEIGHTS;

#while ($line = <NODES>) {
#    if ($line =~ /^(#|\n|NumNodes|NumTerminals|UCLA)/) { next; }
#    print $line;
#    chomp($line);
#    if ($line =~ m/\s*(\S+)\s*([0-9]+)\s*([0-9]+).*/) {
#	$cellName = $1;
#	$width = int($2);
#	$height = int($3);
#	$WIDTHS{$cellName} = $width;
#	$HEIGHTS{$cellName} = $height;
#	print "Added width and height for \"$cellName\"\n";
#    }
#}

while ($line = <PLACED>)
{
    if($line =~ /#/)
    { next; }
    if($line =~ /UCLA/)
    { next; }
    if($line =~ /[a-zA-Z0-9]+/)
    { 
	chop $line;
	$line =~ s/^\s*//;
	@words = split(/\s+/, $line);
	$nodeName = $words[0];
	$xloc = $words[1];
	$yloc = $words[2];
	$orient = $words[4];
	$XLOCS{$nodeName} = $xloc;
	$YLOCS{$nodeName} = $yloc;
	$ORIENTS{$nodeName} = $orient;
#	print "DBG: $nodeName: $xloc $yloc $orient\n";
    }
    
}

my $beginComponents = 0;
while ($line = <ORIG>)
{
    if ($line =~ m/^\s*COMPONENTS/) {
	$beginComponents = 1;
	print OUT $line;
	next;
    }
    if ($line =~ m/^\s*END COMPONENTS/) {
	$beginComponents = 0;
	print OUT $line;
	next;
    }
    if ($beginComponents == 1) {
	if ($line =~ /\s*-\s*(\S+)\s*(\S+)\s*.*/) {
	    my $cellNameOrig = $1;
	    my $cellName = $cellNameOrig;
	    my $libCellName = $2;
	    $cellName =~ s/\\//g;
	    $xloc = $XLOCS{$cellName};
	    $yloc = $YLOCS{$cellName};
	    $orient = $ORIENTS{$cellName};
#	    $width = $WIDTHS{$cellName};
#	    $height = $HEIGHTS{$cellName};
#	    $xloc = $xloc + ($width / 2);
#	    $yloc = $yloc + ($height / 2);
#	    print "Queried width: $width and height: $height for \"$cellName\"\n";
	    print OUT " - $cellNameOrig $libCellName + PLACED ( $xloc $yloc ) $orient ;\n";
	    next;
	} else {
	    # FOR debugging only:
	    print $line;
	    print "NO MATCH\n"
	}
    }
    print OUT $line;
}    

close(ORIG);
close(PLACED);
close(OUT);    


