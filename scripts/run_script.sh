#!/bin/bash

export BENCHMARK_GEN_ROOT='/home/tirumanl/Downloads/Benchmarks/benchmark_gen'
export SCRIPT_ROOT='/home/tirumanl/thesis/VLSIPlacement/scripts'
export LIB_PATH="$BENCHMARK_GEN_ROOT/Lib/stdcell_rvt/db_nldm"
export LEF_FILE="$LIB_PATH/../lef/saed32nm_rvt_1p9m.lef"

export DESIGN_NAME="$1"

echo "script root: $SCRIPT_ROOT"
echo "design name: $DESIGN_NAME"
echo "$SCRIPT_ROOT/genBookShelfTopLevel.pl $DESIGN_NAME"

$SCRIPT_ROOT/genBookShelfTopLevel.pl $DESIGN_NAME
