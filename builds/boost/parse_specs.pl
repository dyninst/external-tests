use strict;
use warnings;

sub parse_version {
	my $v = shift;
	$v =~ s/\s//g;  # strip whitespace
	my ($major,$minor,$other) = split('\.', $v);
	($major,$minor,$other) = map { $_//-1} ($major,$minor,$other);
	return {'version'=>$v, 'major'=>$major,'minor'=>$minor,'other'=>$other};
}

my @exclude = ('develop');
my @versions = grep { my $v=$_; !grep{$_ eq $v->{'version'}} @exclude}
               map { &parse_version($_); }
               `COLIFY_SIZE=1000x1 spack versions -s boost`;

my $mv = &parse_version('1.70.0');
sub less {
  ($a,$b) = @_;
  return 0 if $a->{'version'} eq 'master';
  return 1 if $b->{'version'} eq 'master';
  return
    $a->{'major'} < $b->{'major'} ||
    ($a->{'major'} == $b->{'major'} && $a->{'minor'} < $b->{'minor'}) ||
    ($a->{'major'} == $b->{'major'} && $a->{'minor'} == $b->{'minor'} && $a->{'other'} < $b->{'other'});
}

my $components = 'atomic+chrono+date_time+filesystem+thread+timer';
for my $spec (map {"boost\@$_->{'version'}"} grep { !less($_,$mv) } @versions) {
	print "$spec+$components\n";
}
