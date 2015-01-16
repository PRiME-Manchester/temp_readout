#!/usr/bin/perl

##------------------------------------------------------------------------------
##
## sdp_recv	    SDP receiver program
##
## Copyright (C)    The University of Manchester - 2014
##
## Author           Steve Temple, APT Group, School of Computer Science
## Email            temples@cs.man.ac.uk
##
##------------------------------------------------------------------------------


use strict;
use warnings;

use SpiNN::SCP;


my $spin;		# SpiNNaker handle
my $timeout;	# Receive timeout


# Process the two arguments and open connection to SpiNNaker
# The first argument is the UDP port to listen on and the
# second is the timeout (in seconds) for the receive operation.

sub process_args
{
  die "usage: sdp_recv <port> <timeout>\n" unless
	  $#ARGV == 1 &&
	  $ARGV[0] =~ /^\d+$/ &&
	  $ARGV[1] =~ /^\d+(\.\d+)?$/;

  $spin = SpiNN::SCP->new (port => $ARGV[0]);

  die "Failed to start receiver\n" unless $spin;

  $timeout = $ARGV[1];
}


# Main loop which waits for incoming SDP messages and prints them
# using the SpiNN::SCP debug facility

sub main
{
    my $i;

    process_args ();

    open(MYFILE, '>eth.log');
    $SIG{'INT'} = sub {close(MYFILE)};

    $i = 1;
    while (1)
    {
    	my $rc = $spin->recv_sdp (timeout => $timeout, debug => 4);

        if ($rc) {
            #Replace any %% with %. Seems like not all % were %%
            $rc =~ s/%%/%/g;
            #Replace all % with %%
            $rc =~ s/%/%%/g;
            printf MYFILE $i.' '.$rc;
            printf $i.' '.$rc;
            $i++;
        }
    }

    close(MYFILE);
}


main ();

#------------------------------------------------------------------------------
