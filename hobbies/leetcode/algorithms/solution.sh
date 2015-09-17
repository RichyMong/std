#!/usr/bin/env bash

if [ $# -lt 1 ]; then
    echo "usage: $0 <number>"
    exit 1
fi

file_name=solution_$1.cpp
cat > $file_name  <<- FILE_CONTENT
#include <iostream>

using namespace std;

class Solution {
public:
    
};

int main()
{
}
FILE_CONTENT

vim $file_name
