!/bin/sh -e

BASEDIR=$(realpath "$(dirname "$0")")
ROOTDIR=$(realpath "$BASEDIR/..")

cmake -S . -B "$ROOTDIR/build" -DUNIT_TESTING=ON
cmake --build "$ROOTDIR/build"
