include(../../../amster-config.pri)
TEMPLATE = aux
include($$AMSTERFWK_DIR/amster.pri)


STATIC_BASE = $$PWD/../../bb
STATIC_OUTPUT_BASE = $$AMSTER_INSTALL_INCLUDE_PATH/libs
STATIC_INSTALL_BASE = $$AMSTER_INSTALL_INCLUDE_PATH/libs


DATA_DIRS = \
    covid19bb \
    covid19bb_qt \

for(data_dir, DATA_DIRS) {
    STATIC_FILES += $$files($$STATIC_BASE/$$data_dir/*_dependencies.pri, true)
}
for(data_dir, DATA_DIRS) {
    STATIC_FILES += $$files($$STATIC_BASE/$$data_dir/*.h, true)
}

include($$AMSTERFWK_DIR/amster_data.pri)

DISTFILES += $$STATIC_FILES
