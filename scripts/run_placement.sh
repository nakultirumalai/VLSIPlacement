#!/bin/bash

export BENCHMARK_GEN_ROOT=$HOME/benchmark_gen
export SCRIPT_ROOT=$HOME/thesis/VLSIPlacement/scripts
export LIB_PATH=$BENCHMARK_GEN_ROOT/Lib/stdcell_rvt/db_nldm
export LIB_TECH_PATH=$BENCHMARK_GEN_ROOT/Lib/tech
export LIB_DIR=$LIB_PATH
export LEF_FILE=$LIB_PATH/../lef/saed32nm_rvt_1p9m.lef
export SYNTHESIS_PATH=/opt/CAD/Synopsys/Current/syn/bin
export ICC_PATH=/opt/CAD/Synopsys/Current/icc/bin
export PT_PATH=/opt/CAD/Synopsys/Current/pts/bin
export DESIGN_NAME="$1"
export PLACER_NAME="$2"

echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
echo "Beginning placement generation for benchmark $DESIGN_NAME"
echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
$SCRIPT_ROOT/genPlacement.pl $DESIGN_NAME
