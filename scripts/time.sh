#!/bin/bash

prefix=$(pwd)/tmp

# See man 1 time
format="%E %e %S %U %P %M %t %K %D %p %X %Z %F %R %W %c %w %I %O %r %s %k %C %x"

apps="boost_redis redis_rs go_redis rueidis"
echo "Format: $format"
file=tmp.txt
for app in $apps; do
   printf "%s " $app > $file
   /usr/bin/time --append --output $file --format="$format" $prefix/bin/app_$app
done

column --table $file > time.txt
cat time.txt
