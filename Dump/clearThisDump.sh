#!/bin/bash

STARTDIR=`pwd`

#Switch to this directory
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$DIR"


rm *.pnm


cd "$STARTDIR"

exit 0
