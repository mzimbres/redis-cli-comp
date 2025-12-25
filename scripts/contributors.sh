#!/bin/bash

root="/home/marcelo/Public"
paths="boost/libs/redis go-redis rueidis redis-rs redis-plus-plus fred-rs"

#for path in $paths; do
#   echo $path
#   git -C $root/$path pull 
#done

for path in $paths; do
   echo "$path"
   n=$(git -C $root/$path log --format="%an" | sort -u | wc -l)
   echo "  Contributors: $n"
done
