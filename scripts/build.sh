#!/bin/sh -e

BASEDIR=$(realpath "$(dirname "$0")")
ROOTDIR=$(realpath "$BASEDIR/..")

cmake -S . -B "$ROOTDIR/build" -DUNIT_TESTING=ON -DOPENCL_SUPPORT=OFF
cmake --build "$ROOTDIR/build"
