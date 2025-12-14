#!/bin/bash

/usr/bin/time --format="%E %U %S" ./go-redis/app >& /dev/null

awk '
   NR==1{min=$2}
   FNR==NR{min=($2+0<min)?$2:min;next}
   {printf "%s %.3f %.3f %.3f\n", $1,$2/min,$3/min,$4/min}
' time.txt time.txt
