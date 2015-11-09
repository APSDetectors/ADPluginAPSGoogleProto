TOP=..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

INC += arena.h
INC += arenastring.h
INC += generated_message_util.h
INC += metadata.h
INC += message.h
INC += repeated_field.h
INC += extension_set.h
INC += unknown_field_set.h

ifeq (linux-x86_64, $(findstring linux-x86_64, $(T_A)))
LIB_INSTALLS_Linux += ../GoogleProtoBuf/os/linux-x86_64/lib/libprotobuf.a
LIB_INSTALLS_Linux += ../GoogleProtoBuf/os/linux-x86_64/lib/libprotobuf.so
LIB_INSTALLS_Linux += ../GoogleProtoBuf/os/linux-x86_64/lib/libprotobuf.so.10
LIB_INSTALLS_Linux += ../GoogleProtoBuf/os/linux-x86_64/lib/libprotobuf.so.10.0.0
LIB_INSTALLS_Linux += ../GoogleProtoBuf/os/linux-x86_64/lib/libprotoc.a
LIB_INSTALLS_Linux += ../GoogleProtoBuf/os/linux-x86_64/lib/libprotoc.so
LIB_INSTALLS_Linux += ../GoogleProtoBuf/os/linux-x86_64/lib/libprotoc.so.10
LIB_INSTALLS_Linux += ../GoogleProtoBuf/os/linux-x86_64/lib/libprotoc.so.10.0.0
LIB_INSTALLS_Linux += ../GoogleProtoBuf/os/linux-x86_64/lib/libprotobuf-lite.a
LIB_INSTALLS_Linux += ../GoogleProtoBuf/os/linux-x86_64/lib/libprotobuf-lite.so
LIB_INSTALLS_Linux += ../GoogleProtoBuf/os/linux-x86_64/lib/libprotobuf-lite.so.10
LIB_INSTALLS_Linux += ../GoogleProtoBuf/os/linux-x86_64/lib/libprotobuf-lite.so.10.0.0

endif

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

