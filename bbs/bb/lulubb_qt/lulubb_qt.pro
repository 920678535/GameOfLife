IMPORT_PROTOS_QT_GENERATED = 1

include(../../bbs-config.pri)

PROTO_FILES += \
    $$BBS_PROTO_DIR/net/phoneyou/lulu/bb/Messages.proto \
    $$BBS_PROTO_DIR/net/phoneyou/lulu/bb/Entities.proto \
    $$BBS_PROTO_DIR/net/phoneyou/lulu/bb/Common.proto \
    $$BBS_PROTO_DIR/net/phoneyou/lulu/bb/Descriptors.proto \
    $$BBS_PROTO_DIR/net/phoneyou/lulu/bb/SingletonDescriptors.proto \

GENERATOR_DLLEXPORT = LULUQT_GENERATOR_DLLEXPORT
include($$AMSTERFWK_SRC_DIR/protobufconfig_qt.pri)

DISTFILES += $$PROTO_FILES

SOURCES += \
    $$PWD/../lulubb/register.cpp
