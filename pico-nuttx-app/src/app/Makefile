############################################################################
# src/app/Makefile
############################################################################

include $(APPDIR)/Make.defs

# Application info

PROGNAME  = $(CONFIG_ARDUPROF_APP_PROGNAME)
PRIORITY  = $(CONFIG_ARDUPROF_APP_PRIORITY)
STACKSIZE = $(CONFIG_ARDUPROF_APP_STACKSIZE)
MODULE    = $(CONFIG_ARDUPROF_APP)

# Sources

MAINSRC = arduprof_main.cxx
CXXSRCS   = AppContext.cxx $(wildcard thread/*.cxx peripheral/*.cxx)

include $(APPDIR)/Application.mk

