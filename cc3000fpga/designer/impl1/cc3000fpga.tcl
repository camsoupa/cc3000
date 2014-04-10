# Created by Actel Designer Software 10.1.3.1
# Thu Apr 10 17:20:58 2014

# (OPEN DESIGN)

open_design "cc3000fpga.adb"

# set default back-annotation base-name
set_defvar "BA_NAME" "cc3000fpga_ba"
set_defvar "IDE_DESIGNERVIEW_NAME" {Impl1}
set_defvar "IDE_DESIGNERVIEW_COUNT" "1"
set_defvar "IDE_DESIGNERVIEW_REV0" {Impl1}
set_defvar "IDE_DESIGNERVIEW_REVNUM0" "1"
set_defvar "IDE_DESIGNERVIEW_ROOTDIR" {C:\Users\camsoupa\Documents\embedded_final_project\cc3000\cc3000fpga\designer}
set_defvar "IDE_DESIGNERVIEW_LASTREV" "1"


# import of input files
import_source  \
-format "edif" -edif_flavor "GENERIC" -netlist_naming "VERILOG" {../../synthesis/cc3000fpga.edn} \
-format "pdc"  {..\..\component\work\cc3000fpga\cc3000fpga.pdc} -merge_physical "yes" -merge_timing "yes"
compile
report -type "status" {cc3000fpga_compile_report.txt}

save_design
