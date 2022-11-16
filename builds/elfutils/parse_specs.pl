use strict;
use warnings;

sub parse_version {
	my $v = shift;
	$v =~ s/\s//g;  # strip whitespace
	my ($major,$minor) = split('\.', $v);
	($major,$minor) = map { $_//-1} ($major,$minor);
	return {'version'=>$v, 'major'=>$major,'minor'=>$minor};
}

my @versions = map { &parse_version($_); }
               `COLIFY_SIZE=1000x1 spack versions -s elfutils`;

my $mv = &parse_version('0.186');
sub less {
  ($a,$b) = @_;
  return
    $a->{'major'} < $b->{'major'} ||
    ($a->{'major'} == $b->{'major'} && $a->{'minor'} < $b->{'minor'});
}

for my $spec (map {"elfutils\@$_->{'version'}"} grep { !less($_,$mv) } @versions) {
	print "$spec\n";
}
