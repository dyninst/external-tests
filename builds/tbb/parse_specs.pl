use strict;
use warnings;

sub parse_version {
	my $v = shift;
	$v =~ s/\s//g;  # strip whitespace
	my ($major,$minor,$other) = split('\.', $v);
	($major,$minor,$other) = map { $_//-1} ($major,$minor,$other);
	return {'version'=>$v, 'major'=>$major,'minor'=>$minor,'other'=>$other};
}

sub specs {
  my ($package, $min_version) = @_;
  my @versions = map { &parse_version($_); } `COLIFY_SIZE=1000x1 spack versions -s $package`;

  if($package eq 'intel-tbb') {
    my @exclude = (
      # 2021.2.0 and 2021.1.1 don't build (see https://github.com/oneapi-src/oneTBB/issues/370)
      '2021.2.0','2021.1.1',
      # intel-tbb@2019 is ambiguous with intel-tbb@2019.X
      '2019'
    );
    @versions = grep { my $v=$_; !grep{$_ eq $v->{'version'}} @exclude} @versions;
  }

  # Exclude everything below `min_version`
  if($min_version) {
    my $mv = &parse_version($min_version);
    sub less {
      ($a,$b) = @_;
      return 0 if $a->{'version'} eq 'master';
      return 1 if $b->{'version'} eq 'master';
      return
        $a->{'major'} < $b->{'major'} ||
        ($a->{'major'} == $b->{'major'} && $a->{'minor'} < $b->{'minor'}) ||
        ($a->{'major'} == $b->{'major'} && $a->{'minor'} == $b->{'minor'} && $a->{'other'} < $b->{'other'});
    }
    @versions = grep { !less($_,$mv) } @versions;
  }
  return map {"$package\@$_->{'version'}"} @versions;
}

for my $s (&specs('intel-tbb','2018.6'), &specs('intel-oneapi-tbb')) {
	print "$s\n";
}
