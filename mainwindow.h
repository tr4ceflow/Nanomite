#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QMap>

#include "clsDebugger/clsDebugger.h"
#include "cls/clsPEManager.h"

#include "Models/DisassemblerModel.h"
#include "Models/DisassemblerDbModel.h"
#include "Models/BinaryFileModel.h"

#include "Views/RegisterView.h"
#include "Views/DynamicDisassemblyView.h"
#include "Views/ProgramSegmentationView.h"
#include "Views/ImportsView.h"
#include "Views/JumpView.h"
#include "Views/FunctionView.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    static MainWindow* instance();
    QString m_CurrentFile;

    //components
    clsDebugger *m_Debugger;
    DisassemblerModel *m_DisassemblerModel;
    DisassemblerDbModel *m_DisassemblerDbModel;
    clsPEManager *m_PeManager;
    BinaryFileModel *m_BinaryFileModel;




protected:
    void createViews();
    void createActions();
    void createComponents();
    void createConnections();


    void closeEvent(QCloseEvent *bar);

private:

    static MainWindow *MyMainWindowPtr;

    bool NewFile;

    Ui::MainWindow *ui;




    QTabWidget *MainTabWidget;

    // views
    FunctionView *m_FunctionView;
    RegisterView *m_RegisterView;
    DynamicDisassemblyView *m_DynamicDisassemblyView;
    ProgramSegmentationView *m_ProgramSegmentationView;
    ImportsView* m_ImportsView;
    JumpView *m_JumpView;

    // actions
    QAction *action_Open;

public slots:
     void execOpenAction();
     void execStartDebuggingAction();


     void onSlotDebuggerBreak();
     void onDisplayDisassembly(quint64 eip,QMap<quint64,DISASM> *Data);
     void onNewTargetLoaded();
     void onMainTabChange(int Tab);

signals:
     void On_execOpenAction();
};

#endif // MAINWINDOW_H