TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

# The following gets rid of the -fno-implicit-templates flag on vxWorks, 
# so we get automatic template instantiation.
# This is what we want for the source files that define template functions.
ifeq (vxWorks,$(findstring vxWorks, $(T_A)))
CODE_CXXFLAGS=
endif

USR_INCLUDES += $(GOOGLEPROTO_INCLUDE)

DBD += NDPluginAPSGoogleProto.dbd

INC += NDPluginAPSGoogleProto.h
INC += dataPipe.pb.h

LIBRARY_IOC += NDPluginAPSGoogleProto

NDPluginAPSGoogleProto_SRCS += NDPluginAPSGoogleProto.cpp
NDPluginAPSGoogleProto_SRCS += dataPipe.pb.cc

include $(ADCORE)/ADApp/commonLibraryMakefile

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

