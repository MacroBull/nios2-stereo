# TCL File Generated by Component Editor 15.1
# Tue Mar 29 19:40:38 CST 2016
# DO NOT MODIFY


# 
# hamming "ins_hamming" v1.0
# Macrobull 2016.03.29.19:40:38
# r=hamming(a,b)
# 

# 
# request TCL package from ACDS 15.1
# 
package require -exact qsys 15.1


# 
# module hamming
# 
set_module_property DESCRIPTION r=hamming(a,b)
set_module_property NAME hamming
set_module_property VERSION 1.0
set_module_property INTERNAL false
set_module_property OPAQUE_ADDRESS_MAP true
set_module_property GROUP MyIns
set_module_property AUTHOR Macrobull
set_module_property DISPLAY_NAME ins_hamming
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property EDITABLE true
set_module_property REPORT_TO_TALKBACK false
set_module_property ALLOW_GREYBOX_GENERATION false
set_module_property REPORT_HIERARCHY false


# 
# file sets
# 
add_fileset QUARTUS_SYNTH QUARTUS_SYNTH "" ""
set_fileset_property QUARTUS_SYNTH TOP_LEVEL hamming
set_fileset_property QUARTUS_SYNTH ENABLE_RELATIVE_INCLUDE_PATHS false
set_fileset_property QUARTUS_SYNTH ENABLE_FILE_OVERWRITE_MODE false
add_fileset_file ins_hamming.v VERILOG PATH ins_hamming.v TOP_LEVEL_FILE


# 
# parameters
# 


# 
# display items
# 


# 
# connection point ins_hamming
# 
add_interface ins_hamming nios_custom_instruction end
set_interface_property ins_hamming clockCycle 0
set_interface_property ins_hamming operands 2
set_interface_property ins_hamming ENABLED true
set_interface_property ins_hamming EXPORT_OF ""
set_interface_property ins_hamming PORT_NAME_MAP ""
set_interface_property ins_hamming CMSIS_SVD_VARIABLES ""
set_interface_property ins_hamming SVD_ADDRESS_GROUP ""

add_interface_port ins_hamming oOut result Output 32
add_interface_port ins_hamming iA dataa Input 32
add_interface_port ins_hamming iB datab Input 32

