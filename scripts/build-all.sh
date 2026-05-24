#!/bin/bash

# boost-redis
./scripts/build-boost-redis.sh

# redis-rs
./scripts/build-rust.sh apps/redis-rs/Cargo.toml

# go-redis
./scripts/build-go.sh go_redis

# rueidis
./scripts/build-go.sh rueidis

# fred-rs
./scripts/build-rust.sh apps/fred-rs/Cargo.toml

