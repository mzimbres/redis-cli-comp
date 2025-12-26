#!/bin/bash

root=$(pwd)
cd $root/go-redis
go get
go build 

cd $root/rueidis
go get
go build 

cd $root/redis-rs
cargo build --release

cd $root/fred-rs
cargo build --release

# Benchmarks
echo "Running benchamrks"

PS1="$(pwd)"

echo "========================================================="
echo "go-redis"
cd $root/go-redis
time ./go_redis_app

echo "========================================================="
echo "rueidis"
cd $root/rueidis
time ./rueidis_app

echo "========================================================="
echo "redis-rs"
cd $root/redis-rs/target/release
time ./app
