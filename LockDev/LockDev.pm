package LockDev;

use strict;
use Carp;
use vars qw($VERSION @ISA @EXPORT @EXPORT_OK $AUTOLOAD);

require Exporter;
require DynaLoader;

@ISA = qw(Exporter DynaLoader);
# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.
@EXPORT_OK = qw(
	&dev_testlock
	&dev_lock
	&dev_relock
	&dev_unlock
);
$VERSION = '1.0';

sub AUTOLOAD {
    # This AUTOLOAD is used to 'autoload' constants from the constant()
    # XS function.  If a constant is not found then control is passed
    # to the AUTOLOAD in AutoLoader.

    my $constname;
    ($constname = $AUTOLOAD) =~ s/.*:://;
    my $val = constant($constname, @_ ? $_[0] : 0);
    if ($! != 0) {
	if ($! =~ /Invalid/) {
	    $AutoLoader::AUTOLOAD = $AUTOLOAD;
	    goto &AutoLoader::AUTOLOAD;
	}
	else {
		croak "Your vendor has not defined LockDev macro $constname";
	}
    }
    eval "sub $AUTOLOAD { $val }";
    goto &$AUTOLOAD;
}

bootstrap LockDev $VERSION;

# Preloaded methods go here.

# Autoload methods go after =cut, and are processed by the autosplit program.

1;
__END__
# Below is the stub of documentation for your module. You better edit it!

=head1 NAME

LockDev - Perl extension to manage device lockfiles

=head1 SYNOPSIS

  use LockDev;
  $pid = LockDev::dev_testlock( $device);
  $pid = LockDev::dev_lock( $device);
  $pid = LockDev::dev_relock( $device, $oldpid);
  $pid = LockDev::dev_unlock( $device, $oldpid);

=head1 DESCRIPTION

The LockDev methods act on device locks normally located in /var/lock .
The lock is acquired creating a pair of files hardlinked between them
and named after the device name (as mandated by FSSTND) and the device's
major and minor numbers (as in SVr4 locks). This permits to circumvent
a problem using only the FSSTND lock method when the same device exists
under different names (for convenience or when a device must be accessable
by more than one group of users).

The lock file names are typically in the form LCK..ttyS1 and LCK.004.065 ,
and their content is the pid of the process who owns the lock.

=head1 METHODS

  $pid = LockDev::dev_testlock( $device);

This method simply checks if the device is in some way locked and if
the owner of the lock is still active (otherways it removes the lock).
It recognise a valid lock even if only one of the two lock files exists
(and is owned by an existing process), thus permitting a safe use of
this library toghether with programs using only FSSTND or SVr4 lock style.

  $pid = LockDev::dev_lock( $device);

This method first checks if the device is already locked and then tryes
to acquire the lock building the two lock files. First it creates the
file which name contains the major and minor numbers (in SVr4 style),
then it creates the file with the device name in its name. This order
reduces the clashes with other processes trying to lock the same device
(even with a different name) and using this library. It has no problem
with processes that uses only the FSSTND algorithm.

  $pid = LockDev::dev_relock( $device, $oldpid);

This method changes the owner of an existing lock; if the pid of the
old owner is provided, then it checks if the lock was correctly assigned
(otherways there is the possibility of a process acquiring a lock which
was owned by another unrelated process). If the device was not locked,
locks it.

  $pid = LockDev::dev_unlock( $device, $oldpid);

This method removes the existing locks on the device. If the pid of the
owner of the lock is provided, then it check if the locks are assigned
to that process, avoiding to remove locks assigned to other existing
processes.

=head1 RETURN VALUES

All the methods in LockDev library return ZERO on successfull completion
of the function (LockDev::dev_testlock returns zero if there is no
lock on the device), otherways, if the device is currently locked by
an existing process, they return the pid of the process owner of the
lock.  They return a negative number when some kind of error happens.
Actually they all return only (-1).

=head1 AUTHOR

(c) 1997 by Fabrizio Polacco <fpolacco@icenet.fi>.  The LockDev extension
library and the liblockdev 'C' library are licensed under the FSF's LGPL.

=head1 SEE ALSO

perl(1), lockdev(3).

=cut
