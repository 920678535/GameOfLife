QT += quick
CONFIG += c++11 lrelease embed_translations

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp

RESOURCES += \
    resource.qrc

TRANSLATIONS += \
    i18n/show_zh_CN.ts \
    i18n/show_zh_CN.qm \

RC_ICONS = logo.ico
#RC_FILE = logo.rc

AMSTER_SDK_PATH = $$(AMSTERFWK)

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH = $$AMSTER_SDK_PATH/qml

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

AMSTER_LIB_DEPENDS += amsterqt
include($$AMSTER_SDK_PATH/amster.pri)

DEFINES += \
    DATA_PATH=\\\"$$PWD\\\" \

INCLUDEPATH += \
    $$AMSTER_SDK_PATH/sdk/protos/generate_files/qt \
    $$AMSTER_SDK_PATH/sdk/protos/generate_files/cpp

PARTY_LIBS += \
    protobuf \
    g3logwrapper \

linux{
    PARTY_LIBS += \
        opendds \
        nng \
        boost

    BOOST_LIBS += filesystem system
}

USE_PROTOBUF_QT = 1
include($$(PPM_HOME)/ppm.pri)
