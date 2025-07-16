#!/bin/sh
set -e
LEX=../../zig-out/bin/lexdump
DIR=$(dirname "$0")
for dr in "$DIR"/*.dr; do
  base=$(basename "$dr" .dr)
  out=$($LEX "$dr")
  diff -u "$DIR/$base.json" - <<EOF2
$out
EOF2
done
echo "lexer tests passed"

