#!/bin/bash
set -o nounset
set -o pipefail

# Run cmake-format
cd /root/project
find . \( -type f  -name '*.cmake' ! -path "./build/*" ! -path "./ext/*" \) -exec cmake-format -i {} \;   || { echo "cmake-format run failed!"; exit 1; }
find . \( -type f  -name 'CMakeLists.txt' ! -path "./build/*" ! -path "./ext/*" \) -exec cmake-format -i {} \;   || { echo "cmake-format run failed!"; exit 1; }

#Check git for changes
cd /root/project
cmakeformatcheklist=`git status | grep "modified"`
echo $cmakeformatcheklist
if [[ -z $cmakeformatcheklist ]]; then
    echo "cmake-format test passed!"
    exit 0;
else
    echo "You have cmake-format problems :";
    echo "$cmakeformatcheklist"
    for f in $cmakeformatcheklist; do
        filename=`echo $f | awk '{print $2}'`
        echo "$(git diff $filename)"
    done
    exit 1;
fi
