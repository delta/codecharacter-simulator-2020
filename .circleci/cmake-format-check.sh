#!/bin/bash
set -o nounset
set -o pipefail

# Run CMake-Format
cd /root/project/build
find . \( -name '*.cmake' -o -name 'CMakeLists.txt' \) -exec cmake-format -i {} \;   || { echo "CMake-format run failed!"; exit 1; }

# Check git for changes
cd /root/project
listcmakeformatcheck=`git status | grep "modified"`
if [[ -z $listcmakeformatcheck ]]; then
    echo "Cmake-Format test passed!"
    exit 0;
else
    echo "You have cmake-format problems :";
    for f in $listcmakeformatcheck; do
        filename=`echo $f | awk '{print $2}'`
        echo $filename
        git diff $filename
    done
    exit 1;
fi
