#!/usr/bin/perl -w

while (<>) {
    print("\u\L$_");
    chomp;
    substr($_, 0, 1) =~ tr/a-z/A-Z/;
    substr($_, 1) =~ tr/A-Z/a-z/;
    print("$_\n");
}
