include(../amster-config.pri)

# make it first
LIBS *= -L$$AMSTERFWK_DIR/lib/amster
include($$AMSTERFWK_SRC_DIR/amsterlibrary.pri)
# 避免编译器内存不足
#CONFIG           -= build_all


include($$AMSTERFWK_SRC_DIR/deploylibraryfiles.pri)

INCLUDEPATH += $$PWD


win32{
    QMAKE_CXXFLAGS += /bigobj
}


BBS_PROTO_DIR = $$PWD/protos
PROTOS_DIR = $$absolute_path($$BBS_PROTO_DIR)
PACKAGET_ROOT_PATH = $$PROTOS_DIR

!isEmpty(IMPORT_PROTOS_CPP_GENERATED){

    GENERATOR_OUTPUT_PATH = $$AMSTERFWK_PROTO_ROOT/generate_files/cpp
    INCLUDEPATH *= $$GENERATOR_OUTPUT_PATH
}

!isEmpty(IMPORT_PROTOS_QT_GENERATED){

    GENERATOR_OUTPUT_PATH = $$AMSTERFWK_PROTO_ROOT/generate_files/qt
    INCLUDEPATH *= $$GENERATOR_OUTPUT_PATH
}

