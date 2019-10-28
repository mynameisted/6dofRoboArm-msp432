#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = C:/ti/simplelink_msp432p4_sdk_3_30_00_13/source;C:/ti/simplelink_msp432p4_sdk_3_30_00_13/kernel/tirtos/packages
override XDCROOT = C:/ti/ccs900/xdctools_3_60_01_27_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = C:/ti/simplelink_msp432p4_sdk_3_30_00_13/source;C:/ti/simplelink_msp432p4_sdk_3_30_00_13/kernel/tirtos/packages;C:/ti/ccs900/xdctools_3_60_01_27_core/packages;..
HOSTOS = Windows
endif
