#!/bin/bash

format="%e %U %S %P"
root=$(pwd)
apps_dir=$root/apps

#--------------------------------------------
mkdir -p $root/tmp
cd $root/tmp
cmake -S $apps_dir/boost-redis -B . -DCMAKE_BUILD_TYPE=Release
make -j2

#--------------------------------------------
cd $apps_dir/go-redis
go get
go build 

#--------------------------------------------
cd $apps_dir/rueidis
go get
go build 

#--------------------------------------------
cd $apps_dir/redis-rs
cargo build --release

#--------------------------------------------
cd $apps_dir/fred-rs
cargo build --release

# Benchmarks
echo ""
echo "Running benchamrks"
echo "real user sys cpu"
echo ""

echo "========================================================="
echo "boost-redis"
cd $root/tmp
/usr/bin/time --format="$format" ./app

echo "========================================================="
echo "go-redis"
cd $apps_dir/go-redis
/usr/bin/time --format="$format" ./app

echo "========================================================="
echo "rueidis"
cd $apps_dir/rueidis
/usr/bin/time --format="$format" ./app

echo "========================================================="
echo "redis-rs"
cd $apps_dir/redis-rs/target/release
/usr/bin/time --format="$format" ./app

