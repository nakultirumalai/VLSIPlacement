#!/bin/bash

PLACER=$1
PRESENT_WORKING_DIR=`pwd`

if [ "$PLACER" = "Dragon" ]; then
    cd $2
    /home/tirumanl/Downloads/ISPD05_Placers/Dragon/dragon-linux -f $3
    cd $PRESENT_WORKING_DIR
fi

if [ "$PLACER" = "NTUPlace" ]; then
    cd $2
    /home/tirumanl/Downloads/ntuplace/NTUPlace3/ntuplace3 -aux $3
    cd $PRESENT_WORKING_DIR
fi

if [ "$PLACER" = "mPL6" ]; then
    cd $2
    /home/tirumanl/Downloads/ISPD05_Placers/mPL6-release/mPL6 -d $3
    cd $PRESENT_WORKING_DIR
fi

if [ "$PLACER" = "ourplacer" ]; then
    cd $2
    /home/tirumanl/thesis/VLSIPlacement/code/the_exec . $3
    cd $PRESENT_WORKING_DIR
fi

