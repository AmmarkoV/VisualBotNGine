#!/bin/bash

 CURDIR=`pwd`

 mkdir ramfs
 rm ramfs/*
 sudo mount -t tmpfs -o size=32m tmpfs $CURDIR/ramfs
 df

exit 0
