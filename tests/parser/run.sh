#!/bin/sh
set -e
PARSE=../../zig-out/bin/parse
DIR=$(dirname "$0")
for dr in "$DIR"/*.dr; do
  base=$(basename "$dr" .dr)
  max=$(cat "$DIR/$base.max")
  count=$($PARSE "$dr")
  if [ "$count" -gt "$max" ]; then
    echo "$base: got $count diagnostics, expected <= $max"
    exit 1
  fi
done
echo "parser tests passed"
