#!/bin/bash

prefix=$(pwd)/tmp
datadir="$prefix/data"
rm -rf $datadir
mkdir -p $datadir

#apps="boost_redis_co boost_redis_cb redis_rs rueidis go_redis"
apps="boost_redis_co boost_redis_cb redis_rs go_redis"

echo "========================================================================="
echo "Commands/s"
echo ""

cmds=60000002

file=$datadir/mb_per_sec_sent.txt
echo "Client Commands/s"  > $file
for app in $apps; do
   /usr/bin/time --output=tmp.txt --format="%e" $prefix/bin/app_$app
   t=$(cat tmp.txt)
   value=$(bc -l <<< "$cmds/($t * 1000)")
   printf "%s %.2f\n" $app $value >> $file
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
