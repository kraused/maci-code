#!/bin/bash

# First we record the build ty
echo "" > $1
echo " +------------------------------------------------+" >> $1
echo " | MACI (Multiscale Atomistic Continuum Interface |" >> $1
echo " +------------------------------------------------+" >> $1
echo "" >> $1

# The current time
now=$( date +"%a %b %y %H %M %S" )

echo " BUILD TIME: $now" >> $1
echo "" >> $1

echo " SVN INFO: " >> $1
svn info >> $1

echo " SVN DIFF: " >> $1
svn diff >> $1

# TODO This is only a temporary solution. On the long
#      run we need to find a different solution to present
#      the configuration in a nicer way.
echo " CONFIG: " >> $1
cat $2 >> $1

