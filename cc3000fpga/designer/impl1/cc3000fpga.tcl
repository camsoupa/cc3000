# Created by Actel Designer Software 10.1.3.1
# Thu Apr 17 11:48:35 2014

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


layout -timing_driven
report -type "status" {cc3000fpga_place_and_route_report.txt}
report -type "globalnet" {cc3000fpga_globalnet_report.txt}
report -type "globalusage" {cc3000fpga_globalusage_report.txt}
report -type "iobank" {cc3000fpga_iobank_report.txt}

save_design
