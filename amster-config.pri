!isEmpty(AMSTER_CONFIG_PRI_INCLUDED):error("amster-config.pri already included")
AMSTER_CONFIG_PRI_INCLUDED = 1

AMSTERFWK_DIR = $$(AMSTERFWK)

!exists($$AMSTERFWK_DIR){
    error(cannot found amster framework .)
}

AMSTERFWK_SRC_DIR = $$AMSTERFWK_DIR/sdk
AMSTERFWK_PROTO_ROOT = $$AMSTERFWK_SRC_DIR/protos

!isEmpty(IMPORT_PROTOS_CPP_GENERATED){
    INCLUDEPATH += $$AMSTERFWK_PROTO_ROOT/generate_files/cpp
    CONFIG -= qt
}

!isEmpty(IMPORT_PROTOS_QT_GENERATED){
    INCLUDEPATH += $$AMSTERFWK_PROTO_ROOT/generate_files/qt
    INCLUDEPATH += $$AMSTERFWK_PROTO_ROOT/generate_files/cpp
    CONFIG *= qt
    QT += qml
}
