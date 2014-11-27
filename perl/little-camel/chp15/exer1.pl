#!/usr/bin/perl -w

while (<STDIN>) {
    chomp;
    my $sep = rindex($_, "/");
    my ($head, $tail) = ("", $_);
    unless ($sep == -1) {
        $tail = substr($_, $sep + 1);
        $head = substr($_, 0, $sep);
    }
    print("Head: $head, tail: $tail\n");
}
