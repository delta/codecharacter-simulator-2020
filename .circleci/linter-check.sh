#!/bin/bash
set -o nounset
set -o pipefail

echo "##### Running Linter Checks #####"
cd /root/project/.circleci

# Running clang-format
echo "### Running clang-format ####"
bash "./clang-format-check.sh"

# Running cmake-format
echo "### Running cmake-format ####"
bash "./cmake-format-check.sh"
