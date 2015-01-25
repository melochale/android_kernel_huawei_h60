####################################################################################################
#
####################################################################################################


####################################################################################################
#Directories for library files 
####################################################################################################
vob_lib_dirs :=

####################################################################################################
#library files
####################################################################################################
vob_lib_files :=

####################################################################################################
#Directories for include files
####################################################################################################
include $(CODEC_MK_FILE_PATH)/dsp_code_gu_modem_core_med.inc

####################################################################################################
#Directories for source files
####################################################################################################
vob_src_dirs := $(CODEC_SRC_FILE_PATH)/codec/hr/src

####################################################################################################
#Source files
####################################################################################################
vob_src_files :=$(vob_src_dirs)/hr_globdefs.c    \
	$(vob_src_dirs)/hr_homing.c      \
	$(vob_src_dirs)/hr_host.c        \
	$(vob_src_dirs)/hr_mathhalf.c    \
	$(vob_src_dirs)/hr_vad.c         \
	$(vob_src_dirs)/hr_interface.c   \
	$(vob_src_dirs)/hr_dtx.c         \
	$(vob_src_dirs)/hr_err_conc.c    \
	$(vob_src_dirs)/hr_sp_dec.c      \
	$(vob_src_dirs)/hr_sp_enc.c      \
	$(vob_src_dirs)/hr_sp_rom.c      \
	$(vob_src_dirs)/hr_sp_frm.c      \
	$(vob_src_dirs)/hr_sp_sfrm.c
	
####################################################################################################
#general make rules
####################################################################################################


####################################################################################################
#
####################################################################################################


####################################################################################################
#
####################################################################################################