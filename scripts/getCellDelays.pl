#!/usr/bin/perl
use strict;
use warnings;
use Liberty::Parser;
use liberty;
my $nodes_file =shift(@ARGV);
my $out_file = shift(@ARGV);
my $delay_type = "N/A";

if(scalar @ARGV ==2)
{
    $delay_type = shift(@ARGV);
}

my $input_slew = 1.024;
my $output_load = 8.0;

if($delay_type eq "-c")
{
    $input_slew = shift(@ARGV);
    $output_load = shift(@ARGV);
}

open(nodesFile, $nodes_file) || die ("Cannot open file $nodes_file");
my @lib_cell_names = ();
my $lib_name ="";
my $lib_path = "";
my %uniqCells = ();
my $cell_name;

while(my $row_nodes = <nodesFile>) 
{
    next if($row_nodes =~ m/^(#|\n|NumNodes|NumTerminals|UCLA)/);
    next if($row_nodes =~ m/\s*.*\s*PAD\s*$/);
    chomp($row_nodes);
    $row_nodes =~ s/^\s*//;
    my ($name, $full_lib_name, $type) = split('\s+', $row_nodes);
    my @splitlib_names = split('/',$full_lib_name);
    $lib_name = $splitlib_names[0].".lib";
    $lib_path = $ENV{"LIB_DIR"}."/".$lib_name;

    $cell_name = $splitlib_names[1];
    next if(exists $uniqCells{$cell_name});
    $uniqCells{$cell_name}=$type;
}

my $count = scalar(keys %uniqCells);
print "$count Unique cells found in the $nodes_file from $lib_name library\n";
close(nodesFile);
get_riseandfall_delay($lib_path,\%uniqCells,$input_slew,$output_load,$out_file);

sub get_riseandfall_delay{
    my $parser = new Liberty::Parser;
    my $lib_name = shift;
    my $uniqCellsRef = shift;
    my $tran = shift;
    my $load = shift;
    my $out_file = shift;
    my $arc_group1;
    my $arc_group2;
    
    open(outFile, ">$out_file") || die("Cannot open file: $out_file\n");
    print outFile "# Cell\tOutput_pin\tInput_pin\tRise\tFall\n";
    my $g = $parser->read_file("$lib_name");
    my $cells;
    foreach $cells(keys %{$uniqCellsRef}) 
    {
    	my $cell_group = $parser->locate_cell($g, $cells);
    	my @pin_groups = $parser->get_groups_by_type($cell_group, "pin");
	my $cell_type = $uniqCellsRef->{$cells};
	foreach(@pin_groups) 
	{
	    my $pin_direction= $parser->get_simple_attr_value($_, "direction");
    	    next if($pin_direction eq "input");
	    my $pin_name = $parser->get_group_name($_);
	    if($cell_type eq "FF") 
	    {
	        my @timing_groups = $parser->get_groups_by_type($_,"timing");
		if(@timing_groups)
		{
	            my %relatedPins=();
	            foreach(@timing_groups)
	            {
	                my $related_pin = $parser->get_simple_attr_value($_, "related_pin");
	                next if(exists $relatedPins{$related_pin});
	    	        $relatedPins{$related_pin} = "1";
	    	        #print "$cell\t$pin_name\t$related_pin\n";
	    	        $arc_group1 = $parser->locate_group_by_type($_, "cell_rise");
            	        my $delay_cell_rise = get_arc_delay($arc_group1, $tran, $load);
            	        $arc_group2 = $parser->locate_group_by_type($_, "cell_fall");
            	        my $delay_cell_fall = get_arc_delay($arc_group2, $tran, $load);
             	        print outFile "$cells\t$pin_name\t$related_pin\t$delay_cell_rise\t$delay_cell_fall\n";
		     }
	    	}
		else{
		    print outFile "$cells\tN/A\tN/A\t0\t0\n";
		}
	    } 
	    elsif($cell_type eq "nFF")
	    {
		my @timing_groups = $parser->get_groups_by_type($_,"timing");
                if(@timing_groups)
		{
		    my %relatedPins=();
                
		    foreach(@timing_groups)
                    {
		    	my $timing_type = $parser->get_simple_attr_value($_, "timing_type");
		    	if (defined $timing_type)
		    	{
			    next unless($timing_type eq "combinational");
			    my $related_pin = $parser->get_simple_attr_value($_, "related_pin");
                            next if(exists $relatedPins{$related_pin});
                    	    $relatedPins{$related_pin} = "1";
			    $arc_group1 = $parser->locate_group_by_type($_, "cell_rise");
	    		    my $delay_cell_rise = get_arc_delay($arc_group1, $tran, $load);
	    		    $arc_group2 = $parser->locate_group_by_type($_, "cell_fall");
    	    		    my $delay_cell_fall = get_arc_delay($arc_group2, $tran, $load);
            		    print outFile "$cells\t$pin_name\t$related_pin\t$delay_cell_rise\t$delay_cell_fall\n";
		        }
		        else
		        {
			    my $related_pin = $parser->get_simple_attr_value($_, "related_pin");
                            next if(exists $relatedPins{$related_pin});
                            $relatedPins{$related_pin} = "1";
                            $arc_group1 = $parser->locate_group_by_type($_, "cell_rise");
                            my $delay_cell_rise = get_arc_delay($arc_group1, $tran, $load);
                            $arc_group2 = $parser->locate_group_by_type($_, "cell_fall");
                            my $delay_cell_fall = get_arc_delay($arc_group2, $tran, $load);
                            print outFile "$cells\t$pin_name\t$related_pin\t$delay_cell_rise\t$delay_cell_fall\n";
		    	}
		    }
		}
		else
		{
                    print outFile "$cells\tN/A\tN/A\t0\t0\n";
		}
	    }
	}
	
    }     
    sub get_arc_delay {
    	my $arc = shift;
      	my $tran = shift;
  	my $load = shift;

  	my @index_1 = $parser->get_lookup_table_index_1($arc);
  	my @index_2 = $parser->get_lookup_table_index_2($arc);

  	my @lookup_table = $parser->get_lookup_table($arc);

  	my ($i,$j) = get_index(\@index_1, $tran);
  	my ($m,$n) = get_index(\@index_2, $load);
  
  	#input slew exact but output load not
  
  	if(($j == -1) and ($n != -1))
  	{
    	    my @quad_table = ($lookup_table[$i][$m],$lookup_table[$i][$n]);
            my $p1 = $quad_table[0] + ($quad_table[1] - $quad_table[0])/($index_2[$n] - $index_2[$m])*($load - $index_2[$m]);
    	    my $p = $p1;
    	    $p = sprintf("%.4f", $p);
       	}

  	#input slew not exact but output load exact

  	elsif(($j!=-1) and ($n==-1))
  	{
    	    my @quad_table = ($lookup_table[$i][$m],$lookup_table[$j][$m]);
    	    my $p2 = $quad_table[0] + ($quad_table[1] - $quad_table[0])/($index_1[$j] - $index_1[$i])*($tran - $index_1[$i]);
    	    my $p = $p2;
    	   $p = sprintf("%.4f", $p);
  	}
 
  	#both input slew and output load are approximate
 
  	elsif(($j !=-1) and ($n !=-1))
  	{
    	    my @quad_table = ($lookup_table[$i][$m],$lookup_table[$i][$n],$lookup_table[$j][$m],$lookup_table[$j][$n]);
            my $p1 = $quad_table[0] + ($quad_table[1] - $quad_table[0])/($index_2[$n] - $index_2[$m])*($load - $index_2[$m]);
    	    my $p2 = $quad_table[2] + ($quad_table[3] - $quad_table[2])/($index_2[$n] - $index_2[$m])*($load - $index_2[$m]);
    	    my $p = $p1+($p2-$p1)/($index_1[$j] - $index_1[$i])*($tran - $index_1[$i]);
    	    $p = sprintf("%.4f", $p);
  	}
  
  	#both input slew and output load are exact

  	elsif(($j==-1) and ($n==-1))
  	{
    	    my @quad_table = ($lookup_table[$i][$m])	;
    	    my $p = $quad_table[0];
    	    $p = sprintf("%.4f", $p);
  	}
    }

    sub get_index {
  	my $a = shift;
  	my $value = shift;
  	my $i;
  	my $j;
  	for($i=0;$i<=$#$a;$i++)
  	{
    	    return ($i,-1) if($value eq $$a[$i]);
  	}
  	for($i=0;$i<$#$a;$i=$i+1)
  	{
    	    $j = $i + 1;
    	    return ($i,$j) if($value > $$a[$i] and $value < $$a[$j]);
  	}
  	die "Error: The input value $value is out of the max lookup table index value $$a[$#$a]\n";
    }
}
exit(0);
