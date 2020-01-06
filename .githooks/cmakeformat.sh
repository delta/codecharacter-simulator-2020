for file in $(git diff --name-only --staged); do
        if [[ "$file" =~ .*(.cmake|CMakeLists.txt)$ ]];     #  this is the snag
        then
            echo formatting $file
            cmake-format -i $file && git add $file
        fi
done;