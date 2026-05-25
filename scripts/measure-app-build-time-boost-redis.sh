#!/bin/bash

root=$(pwd)
srcdir=$root/apps/boost-redis
builddir=$root/tmp/build-app-boost-redis

# First make sure everything is built
./scripts/build-boost-redis.sh

apps="app_boost_redis_asio_co app_boost_redis_asio_cb app_boost_redis_corosio"
for app in $apps; do
  printf "%s " $app
  touch $srcdir/$app.cpp
  /usr/bin/time --format="%e" cmake --build $builddir --target $app
done

