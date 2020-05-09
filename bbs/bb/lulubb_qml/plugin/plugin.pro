URI = net.phoneyou.LULU.BB

API_VER = 1.0

TEMPLATE = lib

TARGET = lulubbplugin

QMLFILES += $$PWD/qmldir \

HEADERS += \
    module.h \

SOURCES += \
    plugin.cpp \
    module.cpp \


# link to protobuf_qt
PARTY_LIBS += \
    g3logwrapper \
    protobuf \

USE_PROTOBUF_QT = 1
include($$(PPM_HOME)/ppm.pri)

# link to easysimqt
AMSTER_LIB_DIRS += $$PWD/../../
IMPORT_PROTOS_QT_GENERATED = 1
include($$PWD/../../../../amster-config.pri)
AMSTER_LIB_DEPENDS += lulubb_qt
include($$AMSTERFWK_SRC_DIR/qmlpluginconfig.pri)
