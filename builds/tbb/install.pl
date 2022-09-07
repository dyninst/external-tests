use strict;
use warnings;

my $target_package = shift @ARGV;
my @versions = @ARGV;

open my $fdLog, '>', 'install.log' or die "$!\n";
for my $v (@versions) {
  # Errors here are fatal, so just put it in the log
  print $fdLog "$target_package\@$v\n";

  print "Checking for $target_package\@$v... ";
  `spack/bin/spack find $target_package\@$v >/dev/null 2>&1`;
  if($? == 0) {
    print "found\n";
    next;
  }

  #NB: If 'spack find' fails in any way except 'did not find package',
  #    we'll miss that here.
  print "not found. Installing...\n";
  `spack/bin/spack install -j2 $target_package\@$v`;
  if($? != 0) {
    die "Failed to install $target_package\@$v: '$!\n";
  }
}
