###############################################################################
## Script to generate a Floorplan for any RTL using IC Compiler 
###############################################################################
# Set the design store path
set SCRIPT_ROOT [getenv SCRIPT_ROOT]
set DES_SCRIPT_ROOT [getenv DES_SCRIPT_ROOT]
set TIMING_RPT_DIR [getenv TIMING_RPT_DIR]
set BOOKSHELF_SCRIPT "${SCRIPT_ROOT}/genBookShelf.tcl"
set PLACE_TERMINALS_SCRIPT "${SCRIPT_ROOT}/genPlaceTerms.tcl"

# source uses search path
set sh_source_uses_search_path true
# Define the library type
set LIB_TYPE "32nm"

##################################################
# WHAT LIBRARIES DO I READ
##################################################
set LIB_DIR [getenv LIB_PATH]
set TECH_DIR [getenv LIB_TECH_PATH]
set MW_LIB_PATH [getenv LIB_PATH]/../milkyway
set PLACED_FILE_OUT [getenv PLACED_FILE_OUT]

# Add to this list later
set LIB_LIST "saed32rvt_ff1p16v25c.db"

set LIB_NAME "work"

##################################################
# SET WHAT DESIGN FROM THE ONES WE HAVE IS BEING 
# COMPILED. 
# These are your options: 
# (Add to the list when more are discovered)
# "cpu16"
# "m1_core"
# "avr_core"
# "usb_sie"
##################################################
set DESIGN_NAME [getenv DESIGN_NAME]

set search_path "$search_path . ${SCRIPT_ROOT} ${DES_SCRIPT_ROOT}/${DESIGN_NAME}"

##################################################
# Source the common file
##################################################
source -e -v [getenv BENCHMARK_GEN_ROOT]/scripts/${DESIGN_NAME}/${DESIGN_NAME}.common.tcl

# Read libraries specific to this design
source -e -v [getenv BENCHMARK_GEN_ROOT]/scripts/${DESIGN_NAME}/${DESIGN_NAME}.techlib.tcl

##################################################
# SET THE RESULT DIR
##################################################
set RESULT_DIR [getenv BENCHMARK_GEN_ROOT]/results/${DESIGN_NAME}

##################################################
# SET THE SEARCH PATH HERE
##################################################
set search_path "$search_path . $LIB_DIR"

##################################################
# SET THE LINK AND TARGET LIBRARIES
##################################################
set target_library "$LIB_LIST"
set link_library "* $target_library"

read_verilog ${RESULT_DIR}/${DESIGN_NAME}_compiled.v
current_design ${TOP_DESIGN_NAME}

read_parasitics -keep_capacitive_coupling ${PLACED_FILE_OUT}.postrout.spef.max

##################################################
# TIMING CONSTRAINTS
##################################################
# Set the constraints specific to this design
source -e -v [getenv BENCHMARK_GEN_ROOT]/scripts/${DESIGN_NAME}/${DESIGN_NAME}.cstrs.tcl

#report_timing -max_paths 10000 -from [all_registers -clock_pins] -to [all_registers -data_pins] > ${REPORTS_DIR}/timing_report.rpt
report_timing -max_paths 2000000  > ${TIMING_RPT_DIR}_post_route_timing.rpt
report_global_timing > ${TIMING_RPT_DIR}_global_timing.rpt

remove_design -all

read_verilog ${RESULT_DIR}/${DESIGN_NAME}_compiled.v
current_design ${TOP_DESIGN_NAME}

read_parasitics -keep_capacitive_coupling ${PLACED_FILE_OUT}.prerout.spef.max

##################################################
# TIMING CONSTRAINTS
##################################################
# Set the constraints specific to this design
source -e -v [getenv BENCHMARK_GEN_ROOT]/scripts/${DESIGN_NAME}/${DESIGN_NAME}.cstrs.tcl

#report_timing -max_paths 10000 -from [all_registers -clock_pins] -to [all_registers -data_pins] > ${REPORTS_DIR}/timing_report.rpt
report_timing -max_paths 2000000 > ${TIMING_RPT_DIR}_pre_route_timing.rpt
report_global_timing > ${TIMING_RPT_DIR}_global_timing.rpt

remove_design -all

exit
