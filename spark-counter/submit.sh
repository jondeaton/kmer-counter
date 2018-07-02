#!/usr/bin/env bash

# Set SPARK_MEM if it isn't already set since we also use it for this process
SPARK_MEM=${SPARK_MEM:-2g}
export SPARK_MEM

# Set JAVA_OPTS to be able to load native libraries and to set heap size
JAVA_OPTS="$OUR_JAVA_OPTS"
JAVA_OPTS="$JAVA_OPTS -Djava.library.path=$SPARK_LIBRARY_PATH"
JAVA_OPTS="$JAVA_OPTS -Xms$SPARK_MEM -Xmx$SPARK_MEM"

spark-submit \
    --class kmercounter.KmerCounterMain \
    --master "local[*]" \
    "target/scala-2.11/spark-counter-assembly-0.1.jar" \
    --in $1 --out $2 --k 4 --spark --verbose