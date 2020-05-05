IMPORT_PROTOS_QT_GENERATED = 1
include(../amster-config.pri)

AMSTER_LIB_DIRS += $$PWD/libs
# make it first
LIBS *= -L$$AMSTERFWK_DIR/lib/amster
include($$AMSTERFWK_SRC_DIR/amsteragent.pri)

USE_PROTOBUF_QT = 1
PARTY_LIBS += \
    pluma \
    gflags \
    g3logwrapper \
    protobuf \
    absl \
    cctz \

include($$(PPM_HOME)/ppm.pri)

CONFIG += warn_off

INCLUDEPATH += \
    $$PWD/libs


# 避免编译器内存不足
#CONFIG           -= build_all

win32{
    DEFINES += NOMINMAX
}

posix{
    LIBS += -lpthread
}
