#!/bin/bash

prefix=$(pwd)/tmp
datadir="$prefix/data"
rm -rf $datadir
mkdir -p $datadir

# See man 1 time
#
#   e   Elapsed real (wall clock) time used by the process, in seconds.
#   M   Maximum resident set size of the process during its lifetime, in Kilobytes.
#   R   Number of minor, or recoverable, page faults.  These are pages that are not valid (so they fault) but which have not yet been claimed by
#       other virtual pages.  Thus the data in the page is still valid but the system tables must be updated.
#   c   Number of times the process was context-switched involuntarily (because the time slice expired).
#   w   Number of times that the program was context-switched voluntarily, for instance while waiting for an I/O operation to complete.
format="%e %M %R"

#apps="boost_redis_co boost_redis_cb redis_rs rueidis go_redis"
#apps="boost_redis_co boost_redis_cb redis_rs go_redis"
apps="rueidis"

echo "========================================================================="
echo "time"
echo ""

file=$datadir/time.txt
echo "client $format"  > $file
for app in $apps; do
   printf "%s " $app >> $file
   /usr/bin/time --append --output $file --format="$format" $prefix/bin/app_$app
   sleep 1
done

column --table $file

echo "========================================================================="
echo "pidstat -u"
echo ""

file=$datadir/pidstat-u.txt
echo "Type UID PID %usr %system %guest %wait %CPU CPU Command" > $file
for app in $apps; do
   pidstat -u 1 -e $prefix/bin/app_$app | grep '^Average' >> $file
   sleep 1
done

column --table $file

echo "========================================================================="
echo "pidstat -v"
echo ""

file=$datadir/pidstat-v.txt
echo "Type UID PID threads fd-nr Command" > $file
for app in $apps; do
   pidstat -v 1 -e $prefix/bin/app_$app | grep '^Average' >> $file
   sleep 1
done

column --table $file

echo "========================================================================="
echo "pidstat -w"
echo ""

file=$datadir/pidstat-w.txt
echo "Type UID PID cswch/s nvcswch/s Command" > $file
for app in $apps; do
   pidstat -w 1 -e $prefix/bin/app_$app | grep '^Average' >> $file
   sleep 1
done

column --table $file

echo "========================================================================="
echo "pidstat -r"
echo ""

file=$datadir/pidstat-r.txt
echo "Type UID PID minflt/s majflt/s VSZ RSS %MEM Command" > $file
for app in $apps; do
   pidstat -r 1 -e $prefix/bin/app_$app | grep '^Average' >> $file
   sleep 1
done

column --table $file

echo "========================================================================="
echo "perf-stat"
echo ""

perf_res=$datadir/perf-stat.txt
echo "client cache-misses branch-misses" > $perf_res
for app in $apps; do
   file=$datadir/perf-stat-$app.txt
   rm -f $file
   perf stat -B -e branch-misses,cache-misses --output $file -- $prefix/bin/app_$app
   cache_misses=$(grep cache-misses $file | awk '{print $1}')
   branch_misses=$(grep branch-misses $file | awk '{print $1}')
   echo "$app $cache_misses $branch_misses" >> $perf_res
   sleep 1
done

column --table $perf_res

#echo "========================================================================="
#echo "strace"
#echo ""
#
#for app in $apps; do
#   file=$datadir/strace-$app.txt
#   rm -f $file
#   printf "%s " $app >> $file
#   strace --summary-only --output $file --summary-sort-by name $prefix/bin/app_$app
#   sleep 1
#done
