include(../../amster-config.pri)
TEMPLATE = aux
include($$AMSTERFWK_DIR/amster.pri)


STATIC_BASE = $$PWD
STATIC_OUTPUT_BASE = $$AMSTER_DATA_PATH
STATIC_INSTALL_BASE = $$AMSTER_INSTALL_DATA_PATH


DATA_DIRS = \
    config \

for(data_dir, DATA_DIRS) {
    files = $$files($$PWD/$$data_dir/*, true)
#     Info.plist.in are handled below
    for(file, files):!contains(file, ".*/Info\\.plist\\.in$"):!exists($$file/*): \
        STATIC_FILES += $$file
}

include($$AMSTERFWK_DIR/amster_data.pri)

DISTFILES += $$STATIC_FILES
