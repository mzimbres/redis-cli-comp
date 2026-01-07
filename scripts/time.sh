#!/bin/bash

prefix=$(pwd)/tmp
datadir="$prefix/data"
mkdir -p $datadir

# See man 1 time
#
#   e   Elapsed real (wall clock) time used by the process, in seconds.
#   S   Total number of CPU-seconds used by the system on behalf of the process (in kernel mode), in seconds.
#   U   Total number of CPU-seconds that the process used directly (in user mode), in seconds.
#   P   Percentage of the CPU that this job got.  This is just user + system times divided  by  the  total  running  time.   It  also  prints  a
#       percentage sign.
#   M   Maximum resident set size of the process during its lifetime, in Kilobytes.
#   R   Number of minor, or recoverable, page faults.  These are pages that are not valid (so they fault) but which have not yet been claimed by
#       other virtual pages.  Thus the data in the page is still valid but the system tables must be updated.
#   c   Number of times the process was context-switched involuntarily (because the time slice expired).
#   w   Number of times that the program was context-switched voluntarily, for instance while waiting for an I/O operation to complete.
format="%e %S %U %P %M %R %c %w"

#apps="boost_redis_co boost_redis_cb redis_rs go_redis rueidis"
apps="boost_redis_co boost_redis_cb redis_rs go_redis"

file=$datadir/time.txt
echo "client $format"  > $file
for app in $apps; do
   printf "%s " $app >> $file
   /usr/bin/time --append --output $file --format="$format" $prefix/bin/app_$app
   sleep 1
done

for app in $apps; do
   file=$datadir/strace-$app.txt
   rm -f $file
   printf "%s " $app >> $file
   strace --summary-only --output $file --summary-sort-by name $prefix/bin/app_$app
   sleep 1
done

for app in $apps; do
   file=$datadir/pidstat-$app.txt
   rm -f $file
   pidstat -R 1 -e $prefix/bin/app_$app >> $file
   sleep 1
   pidstat -r 1 -e $prefix/bin/app_$app >> $file
   sleep 1
   pidstat -u 1 -e $prefix/bin/app_$app >> $file
   sleep 1
   pidstat -v 1 -e $prefix/bin/app_$app >> $file
   sleep 1
   pidstat -w 1 -e $prefix/bin/app_$app >> $file
   sleep 1
done

for app in $apps; do
   file=$datadir/perf-stat-$app.txt
   rm -f $file
   perf stat --output $file -e -- $prefix/bin/app_$app
   sleep 1
done
