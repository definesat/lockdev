# Before `make install' is performed this script should be runnable with
# `make test'. After `make install' it should work as `perl test.pl'

######################### We start with some black magic to print on failure.

# Change 1..1 below to 1..last_test_to_print .
# (It may become useful if the test is moved to ./t subdirectory.)

BEGIN { $| = 1; print "1..8\n"; }
END {print "not ok 1\n" unless $loaded;}
use LockDev;
$loaded = 1;
print "ok 1\n";

######################### End of black magic.

# Insert your test code below (better if it prints "ok 13"
# (correspondingly "not ok 13") depending on the success of chunk 13
# of the test code):

# 2 no lock
print &LockDev::dev_testlock( "/dev/ttyS3")	== 0  ? "ok 2\n" : "not ok 2\n";
print &LockDev::dev_lock(     "/dev/ttyS3")	== 0  ? "ok 3\n" : "not ok 3\n";
print &LockDev::dev_testlock( "/dev/ttyS3")	== $$ ? "ok 4\n" : "not ok 4\n";
print &LockDev::dev_relock(   "/dev/ttyS3", 0)	== 0  ? "ok 5\n" : "not ok 5\n";
print &LockDev::dev_testlock( "/dev/ttyS3")	== $$ ? "ok 6\n" : "not ok 6\n";
print &LockDev::dev_unlock(   "/dev/ttyS3", $$)	== 0  ? "ok 7\n" : "not ok 7\n";
print &LockDev::dev_testlock( "/dev/ttyS3")	== 0  ? "ok 8\n" : "not ok 8\n";

