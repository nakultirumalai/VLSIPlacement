###############################################################################
## Script to generate a Floorplan for any RTL using IC Compiler 
###############################################################################

# Set the design store path
set SCRIPT_ROOT "/home/tirumanl/thesis/VLSIPlacement/scripts"
set DES_SCRIPT_ROOT "/home/tirumanl/Downloads/Benchmarks/benchmark_gen/scripts"
set BOOKSHELF_SCRIPT "genBookShelf.tcl"
set PLACE_TERMINALS_SCRIPT "genPlaceTerms.tcl"

# Define the library type
set LIB_TYPE "32nm"

##################################################
# SOURCE TCL FUNCTIONS
##################################################
source -e -v ${SCRIPT_ROOT}/${PLACE_TERMINALS_SCRIPT}

##################################################
# WHAT LIBRARIES DO I READ
##################################################
set LIB_DIR [getenv LIB_PATH]
set TECH_DIR [getenv LIB_TECH_PATH]
set MW_LIB_PATH [getenv LIB_PATH]/../milkyway
set DEF_FILE [getenv DEF_FILE]
set SPEF_FILE_OUT [getenv SPEF_FILE_OUT]

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

set search_path "$search_path . ${SCRIPT_ROOT} ${DES_SCRIPT_ROOT}/${DESIGN_NAME}"

##################################################
# Source the common file
##################################################
source -e -v ${DESIGN_NAME}.common.tcl

# Read libraries specific to this design
source -e -v ${DESIGN_NAME}.techlib.tcl

##################################################
# SET THE RESULT DIR
##################################################
set RESULT_DIR "/home/tirumanl/Downloads/Benchmarks/benchmark_gen/results/${DESIGN_NAME}"

##################################################
# SET THE SEARCH PATH HERE
##################################################
set search_path "$search_path . $LIB_DIR"

##################################################
# SET THE LINK AND TARGET LIBRARIES
##################################################
set target_library "$LIB_LIST"
set link_library "* $target_library"

##################################################
# CREATE THE MILKYWAY LIBRARY
##################################################
sh rm -rf myLIB
create_mw_lib -tech "$TECH_DIR/milkyway/saed32nm_1p9m_mw.tf" \
    -bus_naming_style {[%d]} \
    -mw_reference_library "${MW_LIB_PATH}/saed32nm_rvt_1p9m" \
    myLIB

open_mw_lib myLIB

set_tlu_plus_files -max_tluplus ${TECH_DIR}/star_rcxt/saed32nm_1p9m_Cmax.tluplus \
     -min_tluplus ${TECH_DIR}/star_rcxt/saed32nm_1p9m_Cmin.tluplus \
    -tech2itf_map ${TECH_DIR}/star_rcxt/saed32nm_tf_itf_tluplus.map

read_verilog ${RESULT_DIR}/${DESIGN_NAME}_compiled.v

current_design ${TOP_DESIGN_NAME}
link

#set_tlu_plus_files -tech2itf_map "$TECH_DIR/milkyway/saed32nm_tf_itf_tluplus.map"
#derive_pg_connection
# Create a floorplan here in IC compiler
# ASPECT RATIO IS THE HEIGHT DIVIDED BY WIDTH
# CORE_UTILIZATION IS THE TOTAL ARE OF ALL CELLS DIVIDED BY THE TOTAL CORE AREA
# ROW CORE RATIO IS THE RATIO OF THE CHANNEL 
#     AREA BETWEEN CELLS RELATIVE TO THE ENTIRE CORE AREA
#     THIS NUMBER SHOULD BE GREATER THAN CORE UTILIZATION
# NO_DOUBLE_BACK: SPECIFIES NO FLIPPING NEEDS TO BE DONE
#     IN A PAIR OF ROWS, ONE IS ALWAYS FLIPPED
#initialize_floorplan -control_type aspect_ratio \
#    -core_aspect_ratio 1.0 \
#    -core_utilization 0.7 \
#    -row_core_ratio 0.7 
source -e -v ${DESIGN_NAME}.cstrs.tcl

read_def  ${RESULT_DIR}/bookshelf/${DEF_FILE}

extract_rc -coupling_cap
write_parasitics -format SPEF -output ${RESULT_DIR}/bookshelf/${SPEF_FILE_OUT}.prerout.spef

set do_routing "true"
if {$do_routing eq "true"} {
#    route_opt -initial_route_only -effort low
    route_global
}

extract_rc -coupling_cap
write_parasitics -format SPEF -output ${RESULT_DIR}/bookshelf/${SPEF_FILE_OUT}.postrout.spef

exit
