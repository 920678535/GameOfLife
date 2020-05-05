IMPORT_PROTOS_CPP_GENERATED = 1

INSTALL_FILES += \

include(../../bbs-config.pri)

PROTO_FILES += \
    $$BBS_PROTO_DIR/net/phoneyou/covid19/bb/Messages.proto \
    $$BBS_PROTO_DIR/net/phoneyou/covid19/bb/Entities.proto \
    $$BBS_PROTO_DIR/net/phoneyou/covid19/bb/Common.proto \
    $$BBS_PROTO_DIR/net/phoneyou/covid19/bb/Descriptors.proto \
    $$BBS_PROTO_DIR/net/phoneyou/covid19/bb/SingletonDescriptors.proto \

GENERATOR_DLLEXPORT = COVID19_GENERATOR_DLLEXPORT
include($$AMSTERFWK_SRC_DIR/protobufconfig.pri)

DISTFILES += $$PROTO_FILES \

SOURCES += \
    register.cpp
