#!/bin/bash

export SCRIPT_ROOT='/home/tirumanl/thesis/VLSIPlacement/scripts'

$SCRIPT_ROOT/run_gen_bm.sh $1
$SCRIPT_ROOT/run_gen_placement.sh $1

