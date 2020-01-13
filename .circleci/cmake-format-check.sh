#!/bin/bash
set -o nounset
set -o pipefail

# Run cmake-format
cd /root/project
find . \( -path './ext' -o -path './build' -o -path './simulator' \) -prune -o -type f \( -iname '*.cmake' -o -iname 'CMakeLists.txt' \) -exec cmake-format -i {} \; || { echo "cmake-format run failed;" exit 1; } 

#Check git for changes
cd /root/project
cmakeformatchecklist=`git status | grep "modified"`
echo $cmakeformatchecklist
if [[ -z $cmakeformatchecklist ]]; then
    echo "cmake-format test passed!"
    exit 0;
else
    echo "You have cmake-format problems :";
    echo "$cmakeformatchecklist"
    for f in $cmakeformatchecklist; do
        filename=`echo $f | awk '{print $2}'`
        echo "$(git diff $filename)"
    done
    exit 1;
fi
