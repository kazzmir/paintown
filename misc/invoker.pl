#!/usr/bin/env perl

open F, $ARGV[0];
$x = <F>;
close F;

$x =~ s/"-framework"/-framework/g;
$x =~ s/"/\\"/g;

# print "$x\n";

system($x);
