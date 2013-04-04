###############################################################################
## Script to synthesize any RTL using Design Compiler and the 90nm/32nm lib
###############################################################################
set compile_flow_tracing 10
# Set the design store path
set DES_ROOT "/home/tirumanl/Downloads/Benchmarks/benchmark_gen/src"
set SCRIPT_ROOT "/home/tirumanl/thesis/VLSIPlacement/scripts"
set DES_SCRIPT_ROOT "/home/tirumanl/Downloads/Benchmarks/benchmark_gen/scripts"
set BOOKSHELF_SCRIPT "genBookShelf.tcl"

# Define the library type
set LIB_TYPE "32nm"

##################################################
# SOURCE TCL FUNCTIONS
##################################################
source -e -v ${SCRIPT_ROOT}/${BOOKSHELF_SCRIPT}

##################################################
# WHAT LIBRARIES DO I READ
##################################################
set LIB_DIR [getenv LIB_PATH]

# Add to this list later
set LIB_LIST "saed32rvt_ff1p16v25c.db"

set LIB_NAME "work"

# Path for analyzing alibs
set alib_dare_io_path "/home/tirumanl/Downloads/Benchmarks/benchmark_gen/alibs"

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

set search_path "$search_path . ${DES_ROOT}/${DESIGN_NAME} ${SCRIPT_ROOT} ${DES_SCRIPT_ROOT}/${DESIGN_NAME}"

# Sets a variable which lists all the files to 
source -e -v ${DESIGN_NAME}.rtlsrc.tcl

# Read libraries specific to this design
source -e -v ${DESIGN_NAME}.techlib.tcl

##################################################
# SET THE RESULT DIR
##################################################
set RESULT_DIR "/home/tirumanl/Downloads/Benchmarks/benchmark_gen/results/${DESIGN_NAME}"

##################################################
# SET THE SEARCH PATH HERE
##################################################
set search_path "$search_path . $RTL_SOURCE_DIR $LIB_DIR"

##################################################
# SET THE LINK AND TARGET LIBRARIES
##################################################
set target_library "$LIB_LIST"
set link_library "* $target_library"

##################################################
# READ THE RTL 
##################################################
define_design_lib ${LIB_NAME} -path ./work

read_ddc ${RESULT_DIR}/${DESIGN_NAME}_compiled.ddc

##################################################
# REMOVE DONT_* ATTRIBUTES ON ALL LIBRARY CELLS
##################################################
remove_attribute saed32rvt_ff1p16v25c/* "dont_use"
remove_attribute saed32rvt_ff1p16v25c/* "dont_touch"

##################################################
# TIMING CONSTRAINTS
##################################################
# Set the constraints specific to this design
source -e -v ${DESIGN_NAME}.cstrs.tcl

##################################################
# REPORT ALL PARAMS INTO FILES IN THE RESULT DIR
##################################################
report_area > ${RESULT_DIR}/${DESIGN_NAME}_area.rpt
report_timing -max_paths 10000000 -sort_by slack > ${RESULT_DIR}/${DESIGN_NAME}_timing.rpt
report_qor > ${RESULT_DIR}/${DESIGN_NAME}_qor.rpt

##################################################
# WRITE OUT DDC FOR THE COMPILED NETLIST
##################################################
#change_names -rule verilog
write -f verilog ${TOP_DESIGN_NAME} -hier -o ${RESULT_DIR}/${DESIGN_NAME}_compiled.v
write -f ddc ${TOP_DESIGN_NAME} -hier -o ${RESULT_DIR}/${DESIGN_NAME}_compiled.ddc

##################################################
# GENERATE NODES AND NETS IN THE BOOKSHELF FORMAT
##################################################
genBookShelf "${RESULT_DIR}/bookshelf/${DESIGN_NAME}.nodes" \
    "${RESULT_DIR}/bookshelf/${DESIGN_NAME}.nets" \
    "${RESULT_DIR}/bookshelf/${DESIGN_NAME}.pl" \
    true
exit
