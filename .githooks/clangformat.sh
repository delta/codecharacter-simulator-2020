#!/bin/bash


echo "## Running clang-format on all staged CXX source files ##"
for file in $(git diff --name-only --staged); do
        if [[ "$file" =~ .*\.(hpp|cpp|c|h)$ ]];     #  this is the snag
        then
                clang-format -i $file && git add $file
                echo "Ran on ${file}"
        fi
done;

