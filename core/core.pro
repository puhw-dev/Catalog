TEMPLATE = app
TARGET = catalog_server
CONFIG += console warn_on debug
CONFIG -= app_bundle qt

DESTDIR = ../bin
OBJECTS_DIR = obj
INCLUDEPATH += $$PWD/include $$PWD/../lib/jsoncpp/include
DEPENDPATH += $$PWD/include

SOURCES += \
    src/BadURIHandler.cxx \
    src/CatalogServer.cxx \
    src/main.cxx \
    src/MonitorHandler.cxx \
    src/MonitorsListHandler.cxx \
    src/RequestHandlerFactory.cxx \
    src/utils.cxx \
    src/SearchingHandler.cxx

HEADERS += \
    include/json/autolink.h \
    include/json/config.h \
    include/json/features.h \
    include/json/forwards.h \
    include/json/json.h \
    include/json/reader.h \
    include/json/value.h \
    include/json/writer.h \
    include/PUHW/Catalog/BadURIHandler.h \
    include/PUHW/Catalog/CatalogServer.h \
    include/PUHW/Catalog/DataTypes.h \
    include/PUHW/Catalog/MonitorHandler.h \
    include/PUHW/Catalog/MonitorsListHandler.h \
    include/PUHW/Catalog/RequestHandlerFactory.h \
    include/PUHW/Catalog/utils.h \
    include/PUHW/Catalog/SearchingHandler.h

QMAKE_CXXFLAGS += -std=c++11 -Wextra
QMAKE_CXXFLAGS_DEBUG += -O0
QMAKE_CXXFLAGS_DEBUG -= -O2
QMAKE_CXXFLAGS_RELEASE -= -g -O0
QMAKE_CXXFLAGS_RELEASE += -O2

unix|win32: LIBS += -lPocoNet
unix|win32: LIBS += -lPocoUtil
unix|win32: LIBS += -lPocoFoundation
unix|win32: LIBS += -lPocoData
unix|win32: LIBS += -lPocoSQLite
unix|win32: LIBS += -lboost_regex

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../lib/jsoncpp -ljsoncpp
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../lib/jsoncpp -ljsoncpp
else:unix: LIBS += -L$$PWD/../lib/jsoncpp -ljsoncpp


win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../lib/jsoncpp/libjsoncpp.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../lib/jsoncpp/libjsoncpp.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../lib/jsoncpp/jsoncpp.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../lib/jsoncpp/jsoncpp.lib
else:unix: PRE_TARGETDEPS += $$PWD/../lib/jsoncpp/libjsoncpp.a

OTHER_FILES += \
    ../Changelog.txt \
    ../LICENSE.md \
    ../README.md
