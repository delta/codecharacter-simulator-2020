#!/bin/bash

for file in $(git diff --name-only --staged); do
        if [[ "$file" =~ .*\.(hpp|cpp|c|h)$ ]];     #  this is the snag
        then
                clang-format -i $file && git add $file
        fi
done;

