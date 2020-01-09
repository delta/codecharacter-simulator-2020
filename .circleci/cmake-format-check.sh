#!/bin/bash
set -o nounset
set -o pipefail

# Run cmake-format
cd /root/project/build
find . \( -name '*.cmake' -o -name 'CMakeLists.txt' \) -exec cmake-format -i {} \;   || { echo "cmake-format run failed!"; exit 1; }

# Check git for changes
cd /root/project
cmakeformatcheklist=`git status | grep "modified"`
echo cmakeformatcheklist
if [[ -z $cmakeformatcheklist ]]; then
    echo "cmake-format test passed!"
    exit 0;
else
    echo "You have cmake-format problems :";
    for f in $cmakeformatcheklist; do
        filename=`echo $f | awk '{print $2}'`
        echo $filename
        git diff $filename
    done
    exit 1;
fi
