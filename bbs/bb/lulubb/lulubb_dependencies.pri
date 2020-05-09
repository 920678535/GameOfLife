AMSTER_LIB_NAME = LULUBB

AMSTER_LIB_DEPENDS += \
    gbbinfra \
    amsterbb \

win32{
    DEFINES += LULU_GENERATOR_DLLEXPORT=__declspec(dllimport)
}else{
    DEFINES += LULU_GENERATOR_DLLEXPORT=
}
