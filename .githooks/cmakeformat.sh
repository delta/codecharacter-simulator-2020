#!/bin/bash

echo "## Runnig cmake-format on cmake lists files ##"
for file in $(git diff --name-only --staged); do
        if [[ "$file" =~ .*(.cmake|CMakeLists.txt)$ ]];     #  this is the snag
        then
            cmake-format -i $file && git add $file
            echo "Ran on ${file}"
        fi
done;
