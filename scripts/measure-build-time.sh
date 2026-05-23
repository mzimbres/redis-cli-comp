#!/bin/bash

root=$(pwd)
srcdir=$root/apps/boost-redis
prefix=
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

echo ""

#------------------------------------------------------------------------------
# app

apps="app_boost_redis_asio_co app_boost_redis_asio_cb app_boost_redis_corosio"
echo "App Time"
for app in $apps; do
  printf "%s " $app
  touch $srcdir/$app.cpp
  /usr/bin/time --format="%e" cmake --build $builddir --target $app
done

