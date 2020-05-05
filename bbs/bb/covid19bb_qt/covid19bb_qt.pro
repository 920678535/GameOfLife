IMPORT_PROTOS_QT_GENERATED = 1

include(../../bbs-config.pri)

PROTO_FILES += \
    $$BBS_PROTO_DIR/net/phoneyou/covid19/bb/Messages.proto \
    $$BBS_PROTO_DIR/net/phoneyou/covid19/bb/Entities.proto \
    $$BBS_PROTO_DIR/net/phoneyou/covid19/bb/Common.proto \
    $$BBS_PROTO_DIR/net/phoneyou/covid19/bb/Descriptors.proto \
    $$BBS_PROTO_DIR/net/phoneyou/covid19/bb/SingletonDescriptors.proto \

GENERATOR_DLLEXPORT = COVID19QT_GENERATOR_DLLEXPORT
include($$AMSTERFWK_SRC_DIR/protobufconfig_qt.pri)

DISTFILES += $$PROTO_FILES

SOURCES += \
    $$PWD/../covid19bb/register.cpp
