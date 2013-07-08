#!/usr/bin/perl 

$ARGV[0] or die "no data file given\n";
$nodefile = $ARGV[0].'.node';
$elefile  = $ARGV[0].'.ele';
$outfile  = $ARGV[0].'.amdba';
open (FPN, $nodefile) or die "could not open nodefile: $nodefile";
open (FPE, $elefile) or die "could not open element: $elefile";

$line = <FPN>;
$line =~ /(^\d+)/;
($numNode =  $1) or die "bad format for $nodefile";

$line = <FPE>;
$line =~ /(^\d+)/;
($numTri =  $1) or die "bad format for $elefile";

open(OUTP, ">$outfile") or die "could not open output file $outfile";
print OUTP "\t $numNode \t $numTri \n";

while(<FPN>) {print OUTP unless /^\#.*/}
while(<FPE>) {chomp; print OUTP $_."\t1\n"}
