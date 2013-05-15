#!/bin/bash

export BENCHMARK_GEN_ROOT='/home/tirumanl/Downloads/Benchmarks/benchmark_gen'
export SCRIPT_ROOT='/home/tirumanl/thesis/VLSIPlacement/scripts'
export LIB_PATH="$BENCHMARK_GEN_ROOT/Lib/stdcell_rvt/db_nldm"
export LIB_TECH_PATH="$BENCHMARK_GEN_ROOT/Lib/tech"
export LIB_DIR="$LIB_PATH"
export LEF_FILE="$LIB_PATH/../lef/saed32nm_rvt_1p9m.lef"
export SYNTHESIS_PATH="/opt/CAD/Synopsys/Current/syn/bin"
export ICC_PATH="/opt/CAD/Synopsys/Current/icc/bin"
export PT_PATH="/opt/CAD/Synopsys/Current/pts/bin"
export DESIGN_NAME="$1"
export DEF_FILE="$2"
export SPEF_FILE_OUT="$3"

echo "$SCRIPT_ROOT/genBookShelfTopLevel.pl $DESIGN_NAME"

echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
echo "Beginning routing for benchmark $DESIGN_NAME"
echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
$ICC_PATH/icc_shell -f $SCRIPT_ROOT/viewPlacement.tcl | tee icc_view_log
