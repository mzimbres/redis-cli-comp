#!/bin/bash

root=$(pwd)
srcdir=$root/apps/boost-redis
builddir=$root/tmp/build-app-boost-redis

#------------------------------------------------------------------------------
# src

echo "Source Time"
src=boost_redis_lib_asio
printf "%s " $src
touch $srcdir/$src.cpp
/usr/bin/time --format="%e" cmake --build $builddir --target $src.o

src=boost_redis_lib_corosio
printf "%s " $src
touch $srcdir/$src.cpp
/usr/bin/time --format="%e" cmake --build $builddir --target $src.o
