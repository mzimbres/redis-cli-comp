#!/bin/bash

if [[ $# != 1 ]]; then
  echo "Usage: $0 <go-project-dir-under-apps>"
  exit 1
fi

app=$1
dir=apps/$app
go get -C $dir
go build -C $dir
cp $dir/app tmp/bin/app_$app

# go install does not allow renaming apps
# GOBIN=$(pwd)/bin go install -C $dir
