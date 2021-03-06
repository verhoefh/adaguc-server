#!/bin/bash
export ADAGUC_PATH=/adaguc/adaguc-server-master/
export ADAGUC_TMP=/tmp

# remove all old services such that only active services are monitored
rm -f /servicehealth/*
if [[ $1 ]]; then

  # Update a specific dataset
  for configfile in /data/adaguc-datasets/$1.xml ;do
    filename=/data/adaguc-datasets/"${configfile##*/}" 
    echo "Starting update for ${filename}" 
    /adaguc/adaguc-server-master/bin/adagucserver --updatedb --config /adaguc/adaguc-server-config.xml,${filename}
    OUT=$?
    filename=${filename##*/}
    echo "$OUT" > /servicehealth/${filename%.*}
  done

else

  # Update all datasets
  for configfile in /data/adaguc-datasets/*xml ;do
    filename=/data/adaguc-datasets/"${configfile##*/}" 
    echo "Starting update for ${filename}" 
    /adaguc/adaguc-server-master/bin/adagucserver --updatedb --config /adaguc/adaguc-server-config.xml,${filename}
    OUT=$?
    filename=${filename##*/}
    echo "$OUT" > /servicehealth/${filename%.*}
  done

fi
