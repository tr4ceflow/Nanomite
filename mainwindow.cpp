#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Meta.h"
#include "Views/RegisterView.h"
#include "Views/DisassemblerView.h"
#include "Views/ProgramSegmentationView.h"
#include "Views/JumpView.h"
#include "Views/ImportsView.h"
#include "Views/FunctionView.h"


#include <QDockWidget>
#include <QTextEdit>
#include <QMessageBox>
#include <QCloseEvent>
#include <QMutex>
#include <QGridLayout>
#include <QSplitter>


MainWindow* MainWindow::MyMainWindowPtr = NULL;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->showMaximized();

    //readWriteMutex = new QMutex();

    setWindowTitle(QString(mWindowMainTitle));

    NewFile = true;

    MyMainWindowPtr = this;

    createComponents();

    createActions();
    createViews();
    createConnections();

}

MainWindow::~MainWindow()
{
    delete ui;
}

MainWindow* MainWindow::instance()
{
    return MyMainWindowPtr;
}



void MainWindow::createConnections(){
    connect(m_Debugger,SIGNAL(OnDebuggerBreak()),this,SLOT(onSlotDebuggerBreak()),Qt::QueuedConnection);
    connect(m_DisassemblerModel,SIGNAL(DisAsFinished(quint64,QMap<quint64,DISASM>*)),this,SLOT(onDisplayDisassembly(quint64,QMap<quint64,DISASM>*)),Qt::QueuedConnection);

    connect(this,SIGNAL(On_execOpenAction()),this,SLOT(onNewTargetLoaded()));

    connect(MainTabWidget,SIGNAL(currentChanged(int)),this,SLOT(onMainTabChange(int)));

    connect(
        m_DisassemblerView,
        SIGNAL(
            OnDisassemblyIsReady(
                quint64 ,
                quint32,
                QMap<quint64,DISASM>* ,
                QMap<quint64,DISASM>::const_iterator ,
                QMap<quint64,DISASM>::const_iterator
            )
        ),
        m_JumpView,
        SLOT(
            displayDisassembly(
                quint64 ,
                quint32,
                QMap<quint64,DISASM>* ,
                QMap<quint64,DISASM>::const_iterator ,
                QMap<quint64,DISASM>::const_iterator
             )
        ));
  //  connect(m_DisassemblerView,SIGNAL(OnDisassemblyIsReady(quint64 )),m_JumpView,SLOT(displayDisassembly(quint64  )));

}

void MainWindow::createComponents(){
    m_Debugger = new clsDebugger;
    m_DisassemblerDbModel = new DisassemblerDbModel;
    m_DisassemblerModel = new DisassemblerModel;
    m_PeManager = new clsPEManager;

    //actionDebug_Trace_Stop->setDisabled(true);
}

void MainWindow::createViews(){


    QDockWidget *FunctionViewDock = new QDockWidget(tr("Functions Window"), this);
    m_FunctionView = new FunctionView();
    m_FunctionView->setWindowTitle("Functions");
    m_FunctionView->setWindowIcon(QIcon(":/icons/computer.png"));
    m_FunctionView->show();

    FunctionViewDock->setWidget(m_FunctionView);
    addDockWidget(Qt::LeftDockWidgetArea, FunctionViewDock);

    QDockWidget *RegisterViewDock = new QDockWidget(tr("Register"), this);
    // Log View
    m_RegisterView = new RegisterView();
    m_RegisterView->setWindowTitle("Register");
    m_RegisterView->setWindowIcon(QIcon(":/icons/computer.png"));
    m_RegisterView->show();

    RegisterViewDock->setWidget(m_RegisterView);
    addDockWidget(Qt::RightDockWidgetArea, RegisterViewDock);

    m_DisassemblerView = new DisassemblerView(m_DisassemblerDbModel);
    m_ProgramSegmentationView = new ProgramSegmentationView(m_PeManager,this);
    m_JumpView = new JumpView();
    m_ImportsView = new ImportsView(m_PeManager,this);

    QSplitter *splitter = new QSplitter();

    splitter->addWidget(m_JumpView);
    splitter->addWidget(m_DisassemblerView);

    MainTabWidget = new QTabWidget();

    MainTabWidget->addTab(splitter,"Disassembler");
    MainTabWidget->setTabIcon(0,QIcon(":/icons/application-detail.png"));

    MainTabWidget->addTab(m_ProgramSegmentationView,"Program Segmentation");
    MainTabWidget->setTabIcon(1,QIcon(":/icons/application-block.png"));

    MainTabWidget->addTab(m_ImportsView,"Imports");
    MainTabWidget->setTabIcon(2,QIcon(":/icons/application-import.png"));

    setCentralWidget(MainTabWidget);
}


void MainWindow::createActions(){
    action_Open = new QAction(QIcon(":/icons/folder-open-document.png"), tr("&Open..."), this);
    action_Open->setShortcuts(QKeySequence::Open);
    action_Open->setStatusTip(tr("Open an existing file"));
    connect(action_Open, SIGNAL(triggered()), this, SLOT(execOpenAction()));



    ui->menuFile->addAction(action_Open);
    ui->mainToolBar->addAction(action_Open);
}




void MainWindow::closeEvent(QCloseEvent *bar)
{
    if(m_Debugger->GetDebuggingState()){
        QMessageBox::StandardButton reply;
        reply =QMessageBox::information(this,"Debugger is running","Do you really want to close the debugger?",QMessageBox::Yes|QMessageBox::No);
        if(reply == QMessageBox::Yes){
            // user wants to close the application, so close the target too
            m_Debugger->StopDebuggingAll();
            // we accept closing the app
            bar->accept();
        }else{
            bar->ignore();
        }
    }

}
