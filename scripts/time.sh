#!/bin/bash

prefix=$(pwd)/tmp
datadir="$prefix/data"
rm -rf $datadir
mkdir -p $datadir

#apps="boost_redis_asio_co boost_redis_asio_cb redis_rs rueidis go_redis"
apps="boost_redis_asio_co boost_redis_asio_cb boost_redis_corosio redis_rs go_redis"

#------------------------------------------------------------
# Define the files

timefile=$datadir/time.txt
psu_file=$datadir/pidstat-u.txt
psv_file=$datadir/pidstat-v.txt
psw_file=$datadir/pidstat-w.txt
psr_file=$datadir/pidstat-r.txt
perfs_file=$datadir/perf-stat.txt

#------------------------------------------------------------
# Write the headers

echo "# Wall-clock"  > $timefile
echo "# Type UID PID %usr %system %guest %wait %CPU CPU Command" > $psu_file
echo "# Type UID PID threads fd-nr Command" > $psv_file
echo "# Type UID PID cswch/s nvcswch/s Command" > $psw_file
echo "# Type UID PID minflt/s majflt/s VSZ RSS %MEM Command" > $psr_file
echo "# client cache-misses branch-misses" > $perfs_file

#------------------------------------------------------------

for app in $apps; do
   echo "$app"
   echo "========================================================================="
   echo "Time"
   echo ""

   /usr/bin/time --output=$timefile --append --format="%e" $prefix/bin/app_$app
   sleep 1

   echo "========================================================================="
   echo "pidstat -u"
   echo ""

   pidstat -u 1 -e $prefix/bin/app_$app | grep '^Average' >> $psu_file
   sleep 1

   echo "========================================================================="
   echo "pidstat -v"
   echo ""

   pidstat -v 1 -e $prefix/bin/app_$app | grep '^Average' >> $psv_file
   sleep 1

   echo "========================================================================="
   echo "pidstat -w"
   echo ""

   pidstat -w 1 -e $prefix/bin/app_$app | grep '^Average' >> $psw_file
   sleep 1

   echo "========================================================================="
   echo "pidstat -r"
   echo ""

   pidstat -r 1 -e $prefix/bin/app_$app | grep '^Average' >> $psr_file
   sleep 1

   echo "========================================================================="
   echo "perf-stat"
   echo ""

   file=$datadir/perf-stat-$app.txt
   rm -f $file
   perf stat -B -e branch-misses,cache-misses --output $file -- $prefix/bin/app_$app
   cache_misses=$(grep cache-misses $file | awk '{print $1}')
   branch_misses=$(grep branch-misses $file | awk '{print $1}')
   echo "$app $cache_misses $branch_misses" >> $perfs_file
   sleep 1
done

column --table $timefile
column --table $psu_file
column --table $psv_file
column --table $psw_file
column --table $psr_file
column --table $perfs_file

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
