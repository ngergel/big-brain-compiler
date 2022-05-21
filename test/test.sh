#!/bin/bash

#  ------------------------------------------------------------
#   test.sh
#  
#   Testing script that, given a test name, runs the associated
#   test and checks that it's output matches the expected.
#  ------------------------------------------------------------


# Get the root directory and set all the appropriate directories from there.
ROOT_DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && cd ../ && pwd )"

BRAINC="$ROOT_DIR/bin/brainc"
INPUT="$ROOT_DIR/test/input/$1.bf"
OUTPUT="$ROOT_DIR/test/output/$1.out"
TEMP="$ROOT_DIR/test/temp"
TEMP_OUT="$ROOT_DIR/test/temp.txt"

# Make sure the input and output are both regular files.
test ! -f $INPUT -o ! -f $OUTPUT -o ! -f $BRAINC && exit 1

# Run the brainc compiler with the given input, and compare the output.
$BRAINC $INPUT -o $TEMP &> /dev/null

$TEMP > $TEMP_OUT
if ! diff <(sed -e '$a\' $TEMP_OUT) <(sed -e '$a\' $OUTPUT) > /dev/null
then
    rm -f $TEMP $TEMP_OUT
    exit 1
fi

# Remove the temp files regardless.
rm -f $TEMP $TEMP_OUT