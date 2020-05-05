PARTY_LIBS += \
    gflags \
    absl \

include(../simmodelqt-config.pri)

HEADERS += \
    $$PWD/../seir/seirsimmodel.h

SOURCES += \
    $$PWD/../seir/seirsimmodel.cpp

win32{
    DEFINES += NOMINMAX
}

linux{
    LIBS += -lpthread
}
