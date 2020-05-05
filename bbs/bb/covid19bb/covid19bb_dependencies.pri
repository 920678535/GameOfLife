AMSTER_LIB_NAME = COVID-19BB

AMSTER_LIB_DEPENDS += \
    gbbinfra \
    amsterbb \

win32{
    DEFINES += COVID19_GENERATOR_DLLEXPORT=__declspec(dllimport)
}else{
    DEFINES += COVID19_GENERATOR_DLLEXPORT=
}
