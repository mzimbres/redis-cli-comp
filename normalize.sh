#!/bin/bash

awk '
   NR==1 { min=$2 }
   FNR==NR{min=($2+0<min)?$2:min;next}
   {print $1,$2/min}
' time.txt time.txt
