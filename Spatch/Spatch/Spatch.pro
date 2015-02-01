TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    Spatch.c \
    SessionLoop.c

HEADERS += \
    Spatch.h \
    SessionLoop.h


LIBS += -lssh -lutil -lpthread
