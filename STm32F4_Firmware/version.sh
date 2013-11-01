#!/bin/bash
VERSION_1=`git --git-dir=$1../.git/ describe --tags 2>/dev/null |sed  's/Speedoino_//' | cut -d '-' -f1`;
VERSION_2=`git --git-dir=$1../.git/ describe --tags 2>/dev/null |sed  's/Speedoino_//' | cut -d '-' -f2`;
VERSION_2=`printf "%04i" $VERSION_2`;
VERSION_3=`git --git-dir=$1../.git/ describe --tags 2>/dev/null |sed  's/Speedoino_//' | cut -d '-' -f3`;
echo "#ifndef VERSION_H_
#define VERSION_H_
    #define GIT_REV \"$VERSION_1-$VERSION_2-$VERSION_3\"
#endif" > $1/src/inc/version.h
