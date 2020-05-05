PARTY_LIBS += \
    gflags \
    absl \

include(../simmodel-config.pri)

HEADERS += \
    seirsimmodel.h

SOURCES += \
    seirsimmodel.cpp

win32{
    DEFINES += NOMINMAX
}

linux{
    LIBS += -lpthread
}
