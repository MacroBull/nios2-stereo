# TCL File Generated by Component Editor 15.1
# Thu Mar 31 21:04:18 CST 2016
# DO NOT MODIFY


# 
# hammingPlus "ins_hammingPlus" v1.0
# Macrobull 2016.03.31.21:04:18
# r=hamming(a,b)+r
# 

# 
# request TCL package from ACDS 15.1
# 
package require -exact qsys 15.1


# 
# module hammingPlus
# 
set_module_property DESCRIPTION r=hamming(a,b)+r
set_module_property NAME hammingPlus
set_module_property VERSION 1.0
set_module_property INTERNAL false
set_module_property OPAQUE_ADDRESS_MAP true
set_module_property GROUP MyIns
set_module_property AUTHOR Macrobull
set_module_property DISPLAY_NAME ins_hammingPlus
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property EDITABLE true
set_module_property REPORT_TO_TALKBACK false
set_module_property ALLOW_GREYBOX_GENERATION false
set_module_property REPORT_HIERARCHY false


# 
# file sets
# 
add_fileset QUARTUS_SYNTH QUARTUS_SYNTH "" ""
set_fileset_property QUARTUS_SYNTH TOP_LEVEL hammingPlus
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
# connection point hammingPlus
# 
add_interface hammingPlus nios_custom_instruction end
set_interface_property hammingPlus clockCycle 2
set_interface_property hammingPlus operands 2
set_interface_property hammingPlus ENABLED true
set_interface_property hammingPlus EXPORT_OF ""
set_interface_property hammingPlus PORT_NAME_MAP ""
set_interface_property hammingPlus CMSIS_SVD_VARIABLES ""
set_interface_property hammingPlus SVD_ADDRESS_GROUP ""

add_interface_port hammingPlus oRes result Output 32
add_interface_port hammingPlus iA dataa Input 32
add_interface_port hammingPlus iB datab Input 32
add_interface_port hammingPlus iOp n Input 1
add_interface_port hammingPlus iClk clk Input 1
add_interface_port hammingPlus iClk_en clk_en Input 1
add_interface_port hammingPlus iReset reset Input 1

