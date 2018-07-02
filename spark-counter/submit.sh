#!/usr/bin/env bash

spark-submit \
    --class kmercounter.KmerCounterMain \
    --master "local[*]" \
    "target/scala-2.11/spark-counter-assembly-0.1.jar" \
    --in $1 --out $2 --k 4 --spark