#!/bin/bash

root=$(pwd)
srcdir=$root/apps/boost-redis
builddir=$root/tmp/build-app-boost-redis

apps="app_boost_redis_asio_co app_boost_redis_asio_cb app_boost_redis_corosio"
for app in $apps; do
  printf "%s " $app
  touch $srcdir/$app.cpp
  /usr/bin/time --format="%e" cmake --build $builddir --target $app
done

printf "%s " redis_rs
touch apps/redis-rs/src/main.rs
/usr/bin/time --format="%e" cargo build --jobs 1 --quiet --manifest-path apps/redis-rs/Cargo.toml

printf "%s " go_redis
touch apps/go_redis/app.go
/usr/bin/time --format="%e" go build -C apps/go_redis -p 1

printf "%s " rueidis
touch apps/rueidis/app.go
/usr/bin/time --format="%e" go build -C apps/rueidis -p 1

