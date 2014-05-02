#-------------------------------------------------
#
# Project created by QtCreator 2014-04-25T14:19:15
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tr4cedbg
TEMPLATE = app

QMAKE_CXXFLAGS+= -std=c++11
QMAKE_LFLAGS +=  -std=c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    Views/RegisterView.cpp \
    cls/clsstringhelper.cpp \
    cls/clsstylesettings.cpp \
    clsDebugger/clsDebugger.cpp \
    clsDebugger/clsDebuggerBreakpoints.cpp \
    clsDebugger/clsDebuggerCallbacks.cpp \
    clsDebugger/clsDebuggerPublicAPI.cpp \
    cls/clsAPIImport.cpp \
    cls/clsHelperClass.cpp \
    cls/clsMemManager.cpp \
    cls/clsMemPool.cpp \
    cls/clsAppSettings.cpp \
    cls/clsCrashHandler.cpp \
    cls/clsFunctionsViewWorker.cpp \
    cls/clsHexViewWorker.cpp \
    cls/clsMemDump.cpp \
    cls/clsPEFile.cpp \
    cls/clsPEManager.cpp \
    cls/clsStringViewWorker.cpp \
    cls/clsSymbolAndSyntax.cpp \
    db_sqlite/sqlite3.c \
    mainwindow_menu.cpp \
    mainwindow_slots.cpp \
    Models/DisassemblerModel.cpp \
    Models/DisassemblerDbModel.cpp \
    Views/ProgramSegmentationView.cpp \
    Views/JumpView.cpp \
    cls/clsDBInterface.cpp \
    cls/clsDBManager.cpp \
    Helper/ArrowHelper.cpp \
    Views/ImportsView.cpp \
    Views/FunctionView.cpp \
    Views/DynamicDisassemblyView.cpp \
    Views/StaticDisassemblyView.cpp \
    Models/BinaryFileModel.cpp

HEADERS  += NativeHeaders.h \
mainwindow.h \
    Meta.h \
    Views/RegisterView.h \
    BeaEngine/basic_types.h \
    BeaEngine/BeaEngine.h \
    BeaEngine/export.h \
    BeaEngine/macros.h \
    cls/clsstringhelper.h \
    cls/clsstylesettings.h \
    clsDebugger/clsDebugger.h \
    cls/clsAPIImport.h \
    cls/clsHelperClass.h \
    cls/clsMemManager.h \
    cls/clsMemPool.h \
    cls/clsAppSettings.h \
    cls/clsCrashHandler.h \
    cls/clsFunctionsViewWorker.h \
    cls/clsHexViewWorker.h \
    cls/clsMemDump.h \
    cls/clsPEFile.h \
    cls/clsPEManager.h \
    cls/clsStringViewWorker.h \
    cls/clsSymbolAndSyntax.h \
    db_sqlite/sqlite3.h \
    db_sqlite/sqlite3ext.h \
    Models/DisassemblerModel.h \
    Models/DisassemblerDbModel.h \
    Views/ProgramSegmentationView.h \
    Views/JumpView.h \
    Global.h \
    cls/clsDBInterface.h \
    cls/clsDBManager.h \
    Helper/ArrowHelper.h \
    Models/disassemblerdbmodel.h \
    Models/disassemblerdbmodel.h \
    Views/ImportsView.h \
    Views/FunctionView.h \
    Views/DynamicDisassemblyView.h \
    Views/StaticDisassemblyView.h \
    Models/BinaryFileModel.h

FORMS    += mainwindow.ui \
    ProgramSegmentation.ui



RESOURCES += \
    resource.qrc


LIBS += -L"$$PWD/BeaEngine/" -lBeaEngine


INCLUDEPATH += BeaEngine
DEPENDPATH += BeaEngine


LIBS += -L"$$PWD/clsDebugger/" -ldbghelp


INCLUDEPATH += clsDebugger
DEPENDPATH += clsDebugger




