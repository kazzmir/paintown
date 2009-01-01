#!/usr/bin/env perl

open F, $ARGV[0];
$x = <F>;
close F;

$x =~ s/"/\\"/g;

system($x);
