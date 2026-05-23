#!/bin/bash

root=$(pwd)
apps=$root/apps
prefix=$root/tmp

#--------------------------------------------
# boost-redis

./scripts/build-boost-redis.sh

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

