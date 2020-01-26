#!/bin/bash -euo pipefail
cd "$(dirname "$0")"

cmake -B out --log-level=NOTICE
cmake --build out
