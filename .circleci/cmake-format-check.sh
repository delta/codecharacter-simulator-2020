#!/bin/bash
set -o nounset
set -o pipefail

# Run Clangformat
cd /root/project/build
find . \( -name '*.cmake' -o -name 'CMakeLists.txt' \) -exec cmake-format -i {} \;   || { echo "Make Clangformat failed!"; exit 1; }

# Check git for changes
cd /root/project
cmakeformatcheck=`git status | grep "modified"`
if [[ -z $cmakeformatcheck ]]; then
    echo "Cmake-Format test passed!"
    exit 0;
else
    echo "You have cmake-format problems :";
    for f in $cmakeformatcheck; do
        filename=`echo $f | awk '{print $2}'`
        echo $filename
        git diff $filename
    done
    exit 1;
fi
