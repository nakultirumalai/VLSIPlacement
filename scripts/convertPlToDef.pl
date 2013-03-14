#!/usr/bin/perl -w

sub generateDEF
{
    # parameter : (original DEF, .pl file from Capo, final DEF)
    open(ORIG,"< $_[0]") || die "Can't open $_[0] for input\n";
    open(PLACED,"< $_[1]") || die "Can't open $_[1] for input\n";
    open(OUT,"> $_[2]") || die "Can't open $_[2] for output\n";

    my %XLOCS;
    my %YLOCS;
    my %ORIENTS;

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
#	       print "DBG: $nodeName: $xloc $yloc $orient\n";
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
		$orient = $ORIENTS{$nodeName};
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
}

die "Use: convertPlToDef orig.def placed.pl complete.def\n" if $#ARGV<0;

generateDEF($ARGV[0], $ARGV[1], $ARGV[2]);
