#!/bin/bash

export BENCHMARK_GEN_ROOT="${HOME}/Downloads/Benchmarks/benchmark_gen"
export SCRIPT_ROOT="${HOME}/thesis/VLSIPlacement/scripts"
export DES_SCRIPT_ROOT="${HOME}/Downloads/Benchmarks/benchmark_gen/scripts"
export LIB_PATH="$BENCHMARK_GEN_ROOT/Lib/stdcell_rvt/db_nldm"
export LIB_TECH_PATH="$BENCHMARK_GEN_ROOT/Lib/tech"
export LIB_DIR="$LIB_PATH"
export LEF_FILE="$LIB_PATH/../lef/saed32nm_rvt_1p9m.lef"
export SYNTHESIS_PATH="/opt/CAD/Synopsys/Current/syn/bin"
export ICC_PATH="/opt/CAD/Synopsys/Current/icc/bin"
export PT_PATH="/opt/CAD/Synopsys/Current/pts/bin"
export PLACER_PATH="${HOME}/Downloads/ntuplace/NTUPlace3"
export PLACER_NAME="NTUPlace"
export DESIGN_NAME="$1"
export DEF_FILE="$2"
export SPEF_FILE_OUT="$3"

echo "$SCRIPT_ROOT/genRoutingFlow.pl $DESIGN_NAME"

echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
echo "Beginning routing for benchmark $DESIGN_NAME"
echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
$ICC_PATH/icc_shell -f $SCRIPT_ROOT/genRoutingFlow.tcl | tee icc_routing_log
