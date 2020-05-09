include(../../amster-config.pri)
TEMPLATE = aux
include($$AMSTERFWK_DIR/amster.pri)


STATIC_BASE = $$PWD/../protos
STATIC_OUTPUT_BASE = $$AMSTER_INSTALL_SDK_PATH/protos
STATIC_INSTALL_BASE = $$AMSTER_INSTALL_SDK_PATH/protos


DATA_DIRS = \
    net/phoneyou/lulu/bb \

for(data_dir, DATA_DIRS) {
    files = $$files($$STATIC_BASE/$$data_dir/*, true)
    # Info.plist.in are handled below
    for(file, files): \
        STATIC_FILES += $$file
}

include($$AMSTERFWK_DIR/amster_data.pri)

DISTFILES += $$STATIC_FILES
