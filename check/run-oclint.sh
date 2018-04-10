#!/bin/bash

# gets script directory
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# defines settings
PATH_OCLINT=$1
OPTIONS=$(cat $DIR/options.conf)
COMPILER_OPTIONS='-std=c++11 -Wall -g -I.. -I../include -I../external/AppCommon/include -I../external/AppCommon/res -I../external/Models/include -I../external/wxWidgets/include -I../res -c'

# generates a list of files
FILES=$(find $DIR/../src/ $DIR/../external/AppCommon/src/ -type f -name \*.cc -print)

# runs oclint
$PATH_OCLINT $OPTIONS $FILES -- $COMPILER_OPTIONS
