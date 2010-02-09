use ExtUtils::MakeMaker;
# See lib/ExtUtils/MakeMaker.pm for details of how to influence
# the contents of the Makefile that is written.
WriteMakefile(
         'NAME'	=> 'LockDev',
 'VERSION_FROM'	=> 'LockDev.pm', # finds $VERSION
         'LIBS'	=> [''],
       'DEFINE'	=> '',
          'INC'	=> '-I../src',
     'MYEXTLIB'	=> '../liblockdev.a',
);

sub MY::postamble {
'
$(MYEXTLIB): ../src/lockdev.c ../src/lockdev.h
	cd .. && $(MAKE) static
';
}

