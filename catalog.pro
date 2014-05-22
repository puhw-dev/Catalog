TEMPLATE = subdirs
SUBDIRS = lib/jsoncpp
# core must be last:
CONFIG += ordered debug warn_on
SUBDIRS += core
