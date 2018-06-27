#-------------------------------------------------
#
# Project created by QtCreator 2018-03-02T20:43:40
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = loopback_site_blocker
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
QMAKE_LFLAGS_RELEASE += -static -static-libgcc
DEFINES += QT_DEPRECATED_WARNINGS PRO_FILE_PWD=$$sprintf("\"\\\"%1\\\"\"", $$_PRO_FILE_PWD_)
RC_ICONS += icon.ico
RC_FILE += admin.rc
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
    blocker.cpp \
    abstractPasswordForm.cpp \
    passwordConfirmationForm.cpp \
    passwordCreationForm.cpp \
    passwordRecreationForm.cpp \
    confeditor.cpp

HEADERS += \
    blocker.h \
    abstractPasswordForm.h \
    passwordConfirmationForm.h \
    passwordCreationForm.h \
    passwordRecreationForm.h \
    confeditor.h

DISTFILES += \
    admin.rc \
    admin.xml

RESOURCES += \
    blocker_res.qrc

