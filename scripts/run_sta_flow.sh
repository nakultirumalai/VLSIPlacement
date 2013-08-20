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
export PLACED_FILE_OUT="$2"
export TIMING_RPT_DIR="$3"

echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
echo "Beginning STA for benchmark $DESIGN_NAME"
echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
$PT_PATH/pt_shell -f $SCRIPT_ROOT/ptAnalysisFlow.tcl | tee pt_analysis_log


