use strict;
use warnings;
use Getopt::Long qw(GetOptions);

my %args = (
  'log-file'  => '/dev/null',
  'num-jobs' => $ENV{'BUILD_TEST_NUM_JOBS'} // 1
);
GetOptions(\%args, 'log-file=s', 'num-jobs=i');

my %options = (
  'ADD_VALGRIND_ANNOTATIONS' => ['ON'],                                                 # default OFF
  'BUILD_RTLIB_32'           => ['ON'],                                                 # default OFF
  'CMAKE_BUILD_TYPE'         => ['DEBUG', 'MINSIZEREL', 'RELEASE', 'RELWITHDEBINFO'],
  'ENABLE_DEBUGINFOD'        => ['ON'],                                                 # default OFF
  'ENABLE_LTO'               => ['ON'],                                                 # default OFF
  'ENABLE_STATIC_LIBS'       => ['ON'],                                                 # default OFF
  'LIGHTWEIGHT_SYMTAB'       => ['ON'],                                                 # default OFF
  'SW_ANALYSIS_STEPPER'      => ['OFF'],                                                # default ON
  'USE_OpenMP'               => ['OFF'],                                                # default ON
);

my $build_failed = 0;

for my $o (sort keys %options) {
  for my $v (@{$options{$o}}) {
    my $arg = "-D$o=$v";
    print "Trying $o=$v... ";
    if(&build_dyninst($arg)) {
      if($o ne 'LIGHTWEIGHT_SYMTAB') {
        print "OK\n" if &build_testsuite($arg);
      }
    }
  }
}

# 'christmas tree' build- all options ON
{
  my $xmas = '';
  map {$xmas .= "-D$_=ON "} grep {$_ ne 'CMAKE_BUILD_TYPE' && $_ ne 'LIGHTWEIGHT_SYMTAB'} keys %options;
  print "Trying XMAS... ";
  print "OK\n" if &build_dyninst($xmas) && &build_testsuite($xmas);
}

# 'dark' build- all options OFF
{
  my $dark = '';
  map {$dark .= "-D$_=OFF "} grep {$_ ne 'CMAKE_BUILD_TYPE'} keys %options;
  print "Trying DARK... ";
  print "OK\n" if &build_dyninst($dark) && &build_testsuite($dark);
}

die "Failed\n" if $build_failed;

#-----------------------------------------------------------------------

sub build_dyninst {
  my $ret = system("
      cd /; rm -rf dyninst-build; mkdir dyninst-build; cd dyninst-build
      cmake /dyninst/src -DCMAKE_INSTALL_PREFIX=. -DDYNINST_WARNINGS_AS_ERRORS=ON $_[0] >>$args{'log-file'} 2>&1
      cmake --build . --parallel $args{'num-jobs'} >>$args{'log-file'} 2>&1
      cmake --install . >>$args{'log-file'} 2>&1
    "
  );
  if ($ret != 0) {
    warn "Failed to build Dyninst\n";
    $build_failed = 1;
    return 0;
  }
  return 1;
}

sub build_testsuite {
  my $ret = system("
      cd /; rm -rf testsuite-build; mkdir testsuite-build; cd testsuite-build;
      cmake /testsuite/src -DCMAKE_INSTALL_PREFIX=. -DDyninst_DIR=/dyninst-build/lib/cmake/Dyninst >>$args{'log-file'} 2>&1
      cmake --build . --parallel $args{'num-jobs'} >>$args{'log-file'} 2>&1
      cmake --install . >>$args{'log-file'} 2>&1
    "
  );
  if ($ret != 0) {
    warn "Failed to build testsuite\n";
    $build_failed = 1;
    return 0;
  }
  return 1;
}
