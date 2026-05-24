#!/bin/bash

if [[ $# != 1 ]]; then
  echo "Usage: $0 <path-to-Cargo-toml>"
  exit 1
fi

root=$(pwd)
apps=$root/apps
prefix=$root/tmp

manifest=$1
cargo fetch --manifest-path $manifest
cargo build --release --timings --jobs 1 --manifest-path $manifest
cargo install --path $apps/redis-rs --root $prefix
