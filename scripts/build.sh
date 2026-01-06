#!/bin/bash

root=$(pwd)
apps=$root/apps
prefix=$root/tmp

#--------------------------------------------
# boost-redis

boost_redis_builddir=$prefix/build-app-boost-redis
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
cd $root

#--------------------------------------------
# rueidis

cd $apps/rueidis
go get
go build 
cp app $prefix/bin/app_rueidis
cd $root

#--------------------------------------------
# fred-rs

cargo install --path $apps/fred-rs --root $prefix

