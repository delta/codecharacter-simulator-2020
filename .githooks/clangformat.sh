#!/bin/bash

echo "## Running clang-format on all staged CXX source files ##"
for file in $(git diff --name-only --staged); do
        if [[ "$file" =~ .*\.(hpp|cpp|c|h)$ ]]; then #  this is the snag
                clang-format -i $file && git add $file
                echo "Ran on ${file}"
        fi
done
