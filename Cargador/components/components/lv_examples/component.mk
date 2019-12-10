#
# Component Makefile
#

#CFLAGS += -DLV_CONF_INCLUDE_SIMPLE

COMPONENT_SRCDIRS := .              \
    lv_cargador/cargador          \

COMPONENT_ADD_INCLUDEDIRS := $(COMPONENT_SRCDIRS) ../
