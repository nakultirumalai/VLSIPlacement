#!/usr/bin/perl

use strict;
use warnings;

my $xc = 1064;
my $yc = 836;

my @rectArray = ( [qw/ 1495 1084 1904 1134 /],
 [qw/ 1799 1134 1849 1472 /],
 [qw/ 1854 824 1904 1084 /],
 [qw/ 1495 1134 1545 1472 /],
 [qw/ 1854 696 2055 824 /],
 [qw/ 1854 487 1904 696 /],
 [qw/ 1495 437 1904 487 /],
 [qw/ 1799 111 1849 437 /],
 [qw/ 1495 288 1545 437 /],
);
my $xPresent = "false";
my $yPresent = "false";
foreach my $rects(@rectArray){
    my @rect_i = (@$rects);
    if(($xc >= $rect_i[0]) and ($xc <=$rect_i[2])){
	$xPresent = "true";
    }
    if(($yc >= $rect_i[1]) and ($yc <=$rect_i[3])){
	$yPresent = "true";
    }
}
if(($xPresent eq "true") and ($yPresent eq "true")){
    print "Required pin position is coinciding with the xc,yc\n";
    print "The required pin position is ($xc,$yc)\n";
}
else{
    print "Required pin position is not coinciding with xc,yc\n";
    #getClosestPos($xc, $yc, \@rectArray);
    my ($xp, $yp)=getClosestPos($xc, $yc, \@rectArray);
    print "The required pin position is ($xp,$yp)\n";
}

sub getClosestPos{
    my ($xc, $yc, $arrRef) = @_; 
    


    #print "Xc = $xc\n";
    #print "Yc = $yc\n";
    #foreach my $rect_i(@$arrRef){
        #foreach(@$rect_i){
        #    print "$_\t";
        #}
    #getMandCs($arrRef);
    #my $uniqLinesRectsRef = getMandCs($arrRef);
    #my %linesInARect = (%$linesInARectRef);
    
    #sub getMandCs{
	#my $arrRectsRef = shift;
	#my %MandC=();
    my $minDist;
    my ($Xp,$Yp);
    my $isFirstPoint = "true";
    foreach my $rect_i(@$arrRef){
	my @eachRect = (@$rect_i);
	my @fourCorners = ();
	$fourCorners[0] = [$eachRect[0], $eachRect[1]];
	$fourCorners[1] = [$eachRect[2], $eachRect[1]];
	$fourCorners[2] = [$eachRect[2], $eachRect[3]];
	$fourCorners[3] = [$eachRect[0], $eachRect[3]];
	
	for(my $i=0; $i<4; $i++){
	    my $thisPoint = $fourCorners[$i];
	    #my $nextPoint = $fourCorners[($i+1)];
	    #if($i==3){
	#	$nextPoint = $fourCorners[0];
	#    }
	    my @Point1 = (@$thisPoint);
	 #   my @Point2 = (@$nextPoint);
	    #print "(X1, Y1) = ($Point1[0], $Point1[1])\n";
	 #   print "(X2, Y2) = ($Point2[0], $Point2[1])\n";
	    
            #my $m = (($Point2[1] - $Point1[1])/($Point2[0]-$Point1[0]));
	    #my $c = (($Point2[0]*$Point1[1])-($Point1[0]*$Point2[1]));
	    #print "M= $m : C = $c\n";
	    #next if (exists $MandC{"$m	$c"});
	    #$MandC{"$m	$c"}="1";
	    my $distSquared = getDist($xc, $yc, $thisPoint);
	    #print "Distance of (X1, Y1) from center = $distSquared\n";
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
    return ($Xp, $Yp);
    sub getDist{
	my ($Xc, $Yc, $pointArrRef) = @_;
	my @Point = (@$pointArrRef);
	my $distSquared = ((($Point[0]-$Xc) ** 2) + (($Point[1]-$Yc) ** 2));
	return $distSquared;
    }
}

	    		 



exit(0);

	
