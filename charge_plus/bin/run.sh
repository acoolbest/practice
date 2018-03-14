#!/bin/sh
FULLPATH=$(cd "$(dirname "$0")"; pwd)
export LD_LIBRARY_PATH=${FULLPATH}:$LD_LIBRARY_PATH
${FULLPATH}/$1

