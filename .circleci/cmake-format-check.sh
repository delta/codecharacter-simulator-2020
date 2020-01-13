#!/bin/bash
set -o nounset
set -o pipefail

# Run cmake-format
cd /root/project

# find all cmake list files in working tree, excluding some directories and assigns them in array list
filelist="$(find . -type d \( -wholename './build' -o -wholename './ext' -o -wholename './simulator' -o name "*cmake*" \) -prune -o  \( -type f \( -iname '*.cmake' -o -iname 'CMakeLists.txt' \) -print \))"

for file in $filelist; do
    cmake-format -i $file
done


#Check git for changes
cd /root/project
cmakeformatchecklist=`git status | grep "modified"`
echo $cmakeformatchecklist
if [[ -z $cmakeformatchecklist ]]; then
    echo "cmake-format test passed!"
    exit 0;
else
    echo "You have cmake-format problems in files :";
    echo "$cmakeformatchecklist"
    for f in $cmakeformatchecklist; do
        filename=`echo $f | awk '{print $2}'`
        echo "$(git diff $filename)"
    done
    exit 1;
fi
