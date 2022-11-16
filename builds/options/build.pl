use strict;
use warnings;

die "BUILD_TEST_NUM_JOBS is not set\n" unless exists $ENV{'BUILD_TEST_NUM_JOBS'};

my %options = (
  'ADD_VALGRIND_ANNOTATIONS' => ['ON'], # default OFF
  'BUILD_RTLIB_32' => ['ON'], # default OFF
  'CMAKE_BUILD_TYPE' => ['DEBUG', 'MINSIZEREL', 'RELEASE', 'RELWITHDEBINFO'],
  'ENABLE_DEBUGINFOD' => ['ON'], # default OFF
  'ENABLE_LTO' => ['ON'], # default OFF
  'ENABLE_STATIC_LIBS' => ['ON'], # default OFF
  'LIGHTWEIGHT_SYMTAB' => ['ON'], # default OFF
  'SW_ANALYSIS_STEPPER' => ['OFF'], # default ON
  'USE_OpenMP' => ['OFF'] # default ON
);

&build(%options);

# 'christmas tree' build
my %xmas = map {$_=>['ON']} grep {$_ ne 'CMAKE_BUILD_TYPE'} keys %options;
&build(%xmas);

# 'dark' build
my %dark = map {$_=>['OFF']} grep {$_ ne 'CMAKE_BUILD_TYPE'} keys %options;
&build(%dark);

sub build {
	my %opts = @_;
	for my $o (sort keys %opts) {
		for my $v (@{$opts{$o}}) {
			my $arg = "-D$o=$v";
			print "Trying $o=$v... ";
			&build_dyninst($arg);
			&build_testsuite($arg);
			print "OK\n";
		}
	}
}

sub build_dyninst {
	my $ret = system(
		"
			cd /; rm -rf dyninst-build; mkdir dyninst-build; cd dyninst-build
			cmake /dyninst/src -DCMAKE_INSTALL_PREFIX=. -DDYNINST_WARNINGS_AS_ERRORS=ON $_[0] >/dev/null 2>&1
			cmake --build . --parallel $ENV{'BUILD_TEST_NUM_JOBS'} >/dev/null 2>&1
			cmake --install . >/dev/null 2>&1
		"
	);
	die "Failed to build Dyninst\n" if $ret != 0;
}

sub build_testsuite {
	my $ret = system(
		"
			cd /; rm -rf testsuite-build; mkdir testsuite-build; cd testsuite-build;
			cmake /testsuite/src -DCMAKE_INSTALL_PREFIX=. -DDyninst_DIR=/dyninst-build/lib/cmake/Dyninst >/dev/null 2>&1
			cmake --build . --parallel $ENV{'BUILD_TEST_NUM_JOBS'} >/dev/null 2>&1
			cmake --install . >/dev/null 2>&1
		"
	);
	die "Failed to build testsuite\n" if $ret != 0;
}