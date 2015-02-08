TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    Spatch.c \
    SessionLoop.c \
    clientlistner.c \
    authentication.c \
    knowhost.c \
    sendclientcmd.c \
    usrFile.c

HEADERS += \
    Spatch.h \
    SessionLoop.h \
    clientlistner.h \
    authentication.h \
    usrFile.h


LIBS += -lssh -lutil -lpthread
