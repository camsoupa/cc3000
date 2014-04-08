# Created by Actel Designer Software 10.1.3.1
# Tue Apr 08 15:35:04 2014

# (NEW DESIGN)

# create a new design
new_design -name "cc3000fpga" -family "SmartFusion"
set_device -die {A2F200M3F} -package {484 FBGA} -speed {-1} -voltage {1.5} -IO_DEFT_STD {LVTTL} -OPCONR {COM} -TEMPR {COM} -VCCI_1.5_VOLTR {COM} -VCCI_1.8_VOLTR {COM} -VCCI_2.5_VOLTR {COM} -VCCI_3.3_VOLTR {COM} -VOLTR {COM}


# set default back-annotation base-name
set_defvar "BA_NAME" "cc3000fpga_ba"
set_defvar "IDE_DESIGNERVIEW_NAME" {Impl1}
set_defvar "IDE_DESIGNERVIEW_COUNT" "1"
set_defvar "IDE_DESIGNERVIEW_REV0" {Impl1}
set_defvar "IDE_DESIGNERVIEW_REVNUM0" "1"
set_defvar "IDE_DESIGNERVIEW_ROOTDIR" {C:\Users\camsoupa\Documents\embedded_final_project\cc3000\cc3000fpga\designer}
set_defvar "IDE_DESIGNERVIEW_LASTREV" "1"

# set working directory
set_defvar "DESDIR" "C:/Users/camsoupa/Documents/embedded_final_project/cc3000/cc3000fpga/designer/impl1"

# set back-annotation output directory
set_defvar "BA_DIR" "C:/Users/camsoupa/Documents/embedded_final_project/cc3000/cc3000fpga/designer/impl1"

# enable the export back-annotation netlist
set_defvar "BA_NETLIST_ALSO" "1"

# set EDIF options
set_defvar "EDNINFLAVOR" "GENERIC"

# set HDL options
set_defvar "NETLIST_NAMING_STYLE" "VERILOG"

# setup status report options
set_defvar "EXPORT_STATUS_REPORT" "1"
set_defvar "EXPORT_STATUS_REPORT_FILENAME" "cc3000fpga.rpt"

# legacy audit-mode flags (left here for historical reasons)
set_defvar "AUDIT_NETLIST_FILE" "1"
set_defvar "AUDIT_DCF_FILE" "1"
set_defvar "AUDIT_PIN_FILE" "1"
set_defvar "AUDIT_ADL_FILE" "1"

# import of input files
import_source  \
-format "edif" -edif_flavor "GENERIC" -netlist_naming "VERILOG" {../../synthesis/cc3000fpga.edn} \
-format "pdc"  {..\..\component\work\cc3000fpga\cc3000fpga.pdc}

# save the design database
save_design {cc3000fpga.adb}


compile
report -type "status" {cc3000fpga_compile_report.txt}

save_design
