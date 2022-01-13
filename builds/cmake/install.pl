use strict;
use warnings;

die "Usage: $0 package min_version" unless @ARGV == 2;

my $target_package = $ARGV[0]; 
my $min_version = &parse_version($ARGV[1]);

my @versions = `COLIFY_SIZE=1000x1 spack/bin/spack versions -s $target_package`;

open my $fdLog, '>', 'install.log' or die "$!\n";
for my $v (reverse @versions) {
  # strip whitespace
  $v =~ s/\s//g;

  next if $v eq 'master';
  my $version = &parse_version($v);
  next if $version < $min_version;

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
  `spack/bin/spack install -j16 $target_package\@$v`;
  if($? != 0) {
    die "Failed to install $target_package\@$v: '$!\n";
  }
}


sub parse_version() {
  my $str = shift;
  
  # check for valid format
  my @nums = split('\.', $str);
  unless(@nums == 3) {
    warn "Bad version string '$str'\n";
    return -1;
  }

  # Convert the dotted notation into a lex string
  $str =~ s/\.//g;
  return $str;
}
