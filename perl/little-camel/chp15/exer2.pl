#!/usr/bin/perl -w

chomp(my @numbers = <STDIN>);

@numbers = sort { $a <=> $b } @numbers;

printf("%20g\n", $_) foreach (@numbers);
