#!/bin/bash

export BENCHMARK_ROOT=$HOME/Downloads/Benchmarks
export SCRIPT_ROOT=$HOME/thesis/VLSIPlacement/scripts
export EXEC_FILE=$HOME/thesis/VLSIPlacement/code/the_exec
export OUTPUT_PATH=$(pwd)
export DESIGNS="usb_sie"
#export LIB_DIR=$LIB_PATH
#export LEF_FILE=$LIB_PATH/../lef/saed32nm_rvt_1p9m.lef
export SYNTHESIS_PATH=/opt/CAD/Synopsys/Current/syn/bin
export ICC_PATH=/opt/CAD/Synopsys/Current/icc/bin
export PT_PATH=/opt/CAD/Synopsys/Current/pts/bin
export INPUT_TAB="$1"

echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
echo "Beginning analysis for inputs $INPUT_TAB"
echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
$SCRIPT_ROOT/testArena/topAnalyze.pl $INPUT_TAB
