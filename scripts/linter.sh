#!/bin/sh -e

BASEDIR=$(realpath "$(dirname "$0")")
ROOTDIR=$(realpath "$BASEDIR/..")

find "$ROOTDIR/src"  \( -name '*.c' -o -name '*.h' \) \
    ! -path "$ROOTDIR/src/convolution/gpu/*" ! -path "$ROOTDIR/src/cbmp/*" \
    -print0 | xargs -0 clang-tidy -p build/
find "$ROOTDIR/test" \( -name '*.c' -o -name '*.h' \) -print0 \
    | xargs -0 clang-tidy -p build/
