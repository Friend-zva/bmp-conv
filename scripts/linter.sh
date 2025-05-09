#!/bin/sh -e

BASEDIR=$(realpath "$(dirname "$0")")
ROOTDIR=$(realpath "$BASEDIR/..")

find "$ROOTDIR/src" | grep -E ".*(\.c|\.h)" | xargs clang-tidy -p build/
find "$ROOTDIR/test" | grep -E ".*(\.c|\.h)" | xargs clang-tidy -p build/
