#!/bin/sh -e

BASEDIR=$(realpath "$(dirname "$0")")
ROOTDIR=$(realpath "$BASEDIR/..")

find "$ROOTDIR/src"  \( -name '*.c' -o -name '*.h' \) -print0 \
    | xargs -0 clang-format --dry-run --verbose
find "$ROOTDIR/test" \( -name '*.c' -o -name '*.h' \) -print0 \
    | xargs -0 clang-format --dry-run --verbose
