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
    usrFile.c \
    CmdAnalyser.c \
    cmdlist.c \
    svrFile.c

HEADERS += \
    Spatch.h \
    SessionLoop.h \
    clientlistner.h \
    authentication.h \
    usrFile.h \
    CmdAnalyser.h \
    svrFile.h


LIBS += -lssh -lutil -lpthread
