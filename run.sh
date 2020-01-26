#!/bin/bash -euo pipefail
cd "$(dirname "$0")"

./build.sh
./out/langc "$@"
