#!/bin/bash

if [[ $# != 1 ]]; then
  echo "Usage: $0 <go-dir>"
  exit 1
fi

dir=$1
go get -C $dir

# Do a redundant build first to make sure everything is built in next build
# run.
go build -C $dir -p 1 

# I don't know how to force go to build only the app source file. Touching the
# file does not seem to work so I will use a dirt solution: add a new line to
# the end of the file and then checkout the file.
file=$dir/app.go
echo >> $file
/usr/bin/time --format="%e" go build -C $dir -p 1 
git checkout $file

