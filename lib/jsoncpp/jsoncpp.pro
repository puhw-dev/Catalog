TEMPLATE = lib
TARGET = jsoncpp
CONFIG += staticlib warn_off debug
QT -= core gui
INCLUDEPATH += $$PWD/include
DESTDIR = .
OBJECTS_DIR = obj
VERSION = 0.6.0-rc2

unix {
    target.path = /usr/lib
    INSTALLS += target
}

HEADERS += \
    include/json/autolink.h \
    include/json/config.h \
    include/json/features.h \
    include/json/forwards.h \
    include/json/json.h \
    include/json/reader.h \
    include/json/value.h \
    include/json/writer.h \
    src/lib_json/json_batchallocator.h \
    src/lib_json/json_tool.h

SOURCES += \
    src/lib_json/json_internalarray.inl \
    src/lib_json/json_internalmap.inl \
    src/lib_json/json_reader.cpp \
    src/lib_json/json_value.cpp \
    src/lib_json/json_valueiterator.inl \
    src/lib_json/json_writer.cpp
