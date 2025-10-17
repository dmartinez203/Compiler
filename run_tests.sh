#!/usr/bin/env bash
# Run optimizer tests for the educational compiler
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$ROOT_DIR"

# Build the compiler if missing
if [ ! -x "./minicompiler" ]; then
  echo "Building compiler..."
  make -j2
fi

TEST_SRC="test_optimizer.c"
OUT_S="test_optimizer.s"

echo "\n=== Running compiler on $TEST_SRC ===\n"
./minicompiler "$TEST_SRC" "$OUT_S"

echo "\n=== Generated MIPS ($OUT_S) ===\n"
cat "$OUT_S" || true

echo "\nDone. Inspect the compiler output above for TAC and optimized TAC sections."
