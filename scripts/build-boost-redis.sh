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
  -DBOOST_REDIS_COROSIO_API=ON\
  -DCMAKE_BUILD_TYPE=Release

cmake --build $boost_redis_builddir --target install -j3
