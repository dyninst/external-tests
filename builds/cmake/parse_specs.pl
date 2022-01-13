use strict;
use warnings;

sub parse_version {
	my $v = shift;
	$v =~ s/\s//g;  # strip whitespace
	my ($major,$minor,$other) = split('\.', $v);
	($major,$minor,$other) = map { $_//-1} ($major,$minor,$other);
	return {'version'=>$v, 'major'=>$major,'minor'=>$minor,'other'=>$other};
}

# There is a bug in 3.19.0 that causes .S files to be treated like C files
my @exclude = ('master', '3.19.0');
my @versions = grep { my $v=$_; !grep{$_ eq $v->{'version'}} @exclude}
							 map { &parse_version($_); } `COLIFY_SIZE=1000x1 spack versions -s cmake`;

# Exclude everything below `min_version`
my $mv = &parse_version('3.13.0');
sub less {
  ($a,$b) = @_;
  return
    $a->{'major'} < $b->{'major'} ||
    ($a->{'major'} == $b->{'major'} && $a->{'minor'} < $b->{'minor'}) ||
    ($a->{'major'} == $b->{'major'} && $a->{'minor'} == $b->{'minor'} && $a->{'other'} < $b->{'other'});
}

for my $spec (map {"cmake\@$_->{'version'}"} grep { !less($_,$mv) } @versions) {
	print "$spec\n";
}
