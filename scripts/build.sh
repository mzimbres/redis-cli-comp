#!/bin/bash

root=$(pwd)
apps=$root/apps
prefix=$root/tmp

#--------------------------------------------
# boost-redis

boost_redis_builddir=$root/boost_redis
mkdir -p $boost_redis_builddir
cd $boost_redis_builddir
cmake\
  -S $apps/boost-redis\
  -B $boost_redis_builddir\
  -DCMAKE_INSTALL_PREFIX=$prefix\
  -DCMAKE_BUILD_TYPE=Release

cmake --build $boost_redis_builddir --target install -j3

#--------------------------------------------
# redis-rs

cargo install --path $apps/redis-rs --root $prefix

#--------------------------------------------
# go-redis

cd $apps/go-redis
go get
go build 
cp app $prefix/bin/app_go_redis

#--------------------------------------------
# rueidis

cd $apps/rueidis
go get
go build 
cp app $prefix/bin/app_rueidis

#--------------------------------------------
# fred-rs

cargo install --path $apps/fred-rs --root $prefix

# Benchmarks
echo ""
echo "Running benchamrks"
echo "real user sys cpu"
echo ""

apps="boost_redis redis_rs go_redis rueidis"
for app in $apps; do
   echo "$app"
   /usr/bin/time --format="%e %U %S %P" $prefix/bin/app_$app
done

#pidstat -v 1 -e ./app
#pidstat -u 1 -e ./app
#pidstat -w 1 -e ./app
# perf stat
