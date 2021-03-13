#!/bin/bash

echo "## Running cmake-format on cmake lists files ##"
for file in $(git diff --name-only --staged); do
    if [[ "$file" =~ .*(.cmake|CMakeLists.txt)$ ]]; then #  this is the snag
        cmake-format -i $file && git add $file
        echo "Ran on ${file}"
    fi
done
