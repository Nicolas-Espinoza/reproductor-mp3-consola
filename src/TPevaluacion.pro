TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.c \
    sound.c \
    procedimientos.c

HEADERS += \
    sound.h \
    sound.h \
    procedimientos.h
LIBS += "C:\portaudio_qt_nuevo\lib\libportaudio.dll"
INCLUDEPATH += "C:\portaudio_qt_nuevo\include"
