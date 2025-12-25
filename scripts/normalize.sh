#!/bin/bash

#/usr/bin/time --format="%E %U %S" ./go-redis/app >& /dev/null

awk '
   NR==1{min_real=$2; min_user=$3; min_sys=$4}
   FNR==NR{min_real=($2+0<min_real)?$2:min_real;next}
   FNR==NR{min_user=($2+0<min_user)?$2:min_user;next}
   FNR==NR{min_sys=($2+0<min_sys)?$2:min_sys;next}
   {printf "%s %.3f %.3f %.3f\n", $1,$2/min_real,$3/min_user,$4/min_sys}
' time.txt time.txt | column --table
