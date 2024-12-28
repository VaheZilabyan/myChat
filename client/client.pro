QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    chatwindow.cpp \
    ../LoginSystem/loginwindow.cpp \
    ../LoginSystem/dbmanager.cpp \
    ../LoginSystem/sign_in.cpp \
    ../LoginSystem/sign_up.cpp \
    ../LoginSystem/checker.cpp

HEADERS += \
    chatwindow.h \
    ../LoginSystem/loginwindow.h \
    ../LoginSystem/dbmanager.h \
    ../LoginSystem/sign_in.h \
    ../LoginSystem/sign_up.h \
    ../LoginSystem/checker.h


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
