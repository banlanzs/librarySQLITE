QT       += core gui
QT+=sql
QT+=network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    bookin.cpp \
    borrowbook.cpp \
    emailyanshi.cpp \
    main.cpp \
    mainwindow.cpp \
    managebooks.cpp \
    manageusers.cpp \
    mode.cpp \
    readme.cpp \
    register.cpp \
    reservation.cpp \
    returnbook.cpp \
    rewritepwd.cpp \
    usermode.cpp

HEADERS += \
    DatabaseManager.h \
    bookin.h \
    borrowbook.h \
    emailyanshi.h \
    mainwindow.h \
    managebooks.h \
    manageusers.h \
    mode.h \
    readme.h \
    register.h \
    reservation.h \
    returnbook.h \
    rewritepwd.h \
    usermode.h

FORMS += \
    bookin.ui \
    borrowbook.ui \
    emailyanshi.ui \
    mainwindow.ui \
    managebooks.ui \
    manageusers.ui \
    mode.ui \
    readme.ui \
    readme.ui \
    register.ui \
    reservation.ui \
    returnbook.ui \
    rewritepwd.ui \
    usermode.ui

# MySQL library configuration for 64-bit Windows
win32:CONFIG(release, debug|release): LIBS += "C:/Program Files/MySQL/MySQL Server 5.5/lib/libmysql.lib"
win32:CONFIG(debug, debug|release): LIBS += "C:/Program Files/MySQL/MySQL Server 5.5/lib/libmysql.lib"
INCLUDEPATH += "C:/Program/ Files/MySQL/MySQL/ Server/ 5.5/include"
DEPENDPATH += "C:/Program/ Files/MySQL/MySQL/ Server/ 5.5/include"

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    images.qrc
