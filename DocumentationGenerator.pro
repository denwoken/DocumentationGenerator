QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    $$PWD/Widgets/DialogHelp.cpp \
    $$PWD/DrawIoExecutable.cpp \
    $$PWD/DrawioExporter.cpp \
    $$PWD/DrawioFileParser.cpp \
    $$PWD/Widgets/DrawioRenderWidget.cpp \
    $$PWD/Widgets/StatusLamp.cpp \
    $$PWD/main.cpp \
    $$PWD/Widgets/mainwindow.cpp \
    $$PWD/Widgets/DrawioDiagramBuilderWidget.cpp \
    $$PWD/Widgets/CustomStatusBar.cpp \
    $$PWD/Widgets/SimplifiedFieldsParserWidget.cpp

HEADERS += \
    $$PWD/Widgets/DialogHelp.h \
    $$PWD/DrawIoExecutable.h \
    $$PWD/DrawioExporter.h \
    $$PWD/DrawioFileParser.h \
    $$PWD/Widgets/DrawioRenderWidget.h \
    $$PWD/Widgets/StatusLamp.h \
    $$PWD/Widgets/mainwindow.h \
    $$PWD/Widgets/DrawioDiagramBuilderWidget.h \
    $$PWD/Widgets/CustomStatusBar.h \
    $$PWD/Widgets/SimplifiedFieldsParserWidget.h

FORMS += \
    $$PWD/Widgets/DialogHelp.ui \
    $$PWD/Widgets/DrawioRenderWidget.ui \
    $$PWD/Widgets/mainwindow.ui \
    $$PWD/Widgets/DrawioDiagramBuilderWidget.ui \
    $$PWD/Widgets/SimplifiedFieldsParserWidget.ui

INCLUDEPATH += $$PWD/Widgets/

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    $$PWD/resources.qrc


include($$PWD/LogConsole/LogConsole.pri)
INCLUDEPATH += $$PWD/LogConsole/
