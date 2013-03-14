#!/bin/bash

PLACER=$1
PWD=`pwd`

if [ "$PLACER" == "Dragon" ]; then
    cd $2
    /home/tirumanl/Downloads/ISPD05_Placers/Dragon/dragon-linux -f $3
    cd $PWD
fi


    