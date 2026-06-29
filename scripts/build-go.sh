#!/bin/bash

if [[ $# != 1 ]]; then
  echo "Usage: $0 <go-project-dir-under-apps>"
  exit 1
fi

app=$1
dir=apps/$app
go get -C $dir
go clean -cache
/usr/bin/time --format="%e" go build -C $dir -buildvcs=false -a -p 1 -o $PWD/tmp/bin/app_$app
