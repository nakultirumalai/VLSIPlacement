#!/bin/bash

export BENCHMARK_ROOT=$HOME/Downloads/Benchmarks
export SCRIPT_ROOT=$HOME/thesis/VLSIPlacement/scripts
export EXEC_FILE=$HOME/thesis/VLSIPlacement/code/the_exec
export OUTPUT_PATH=$(pwd)
export DESIGNS="pairing"
export SYNTHESIS_PATH=/opt/CAD/Synopsys/Current/syn/bin
export ICC_PATH=/opt/CAD/Synopsys/Current/icc/bin
export PT_PATH=/opt/CAD/Synopsys/Current/pts/bin
export INPUT_TAB="$1"
export EXEC_TYPE="$2"

echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
echo "Beginning analysis for inputs $INPUT_TAB"
echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
$SCRIPT_ROOT/topAnalyze.pl $INPUT_TAB
