#!/usr/bin/perl
use strict;
use warnings;

my $path = "/home/neo/testChange";
chdir($path) or die "Cant chdir to $path $!";
system("touch hello");

exit(0);
