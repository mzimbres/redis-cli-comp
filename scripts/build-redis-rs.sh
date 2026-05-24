#!/bin/bash

root=$(pwd)
apps=$root/apps
prefix=$root/tmp

manifest=apps/redis-rs/Cargo.toml
cargo fetch --manifest-path $manifest
cargo build --release --timings --jobs 1 --manifest-path $manifest
cargo install --path $apps/redis-rs --root $prefix
