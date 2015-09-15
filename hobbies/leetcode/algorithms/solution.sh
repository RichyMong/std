#!/usr/bin/env bash

if [ $# -lt 1 ]; then
    echo "usage: $0 <number>"
    exit 1
fi

cat > Solution_$1.cpp <<- FILE_CONTENT
#include <iostream>

class Solution {
public:
    
};

int main()
{
}

FILE_CONTENT
