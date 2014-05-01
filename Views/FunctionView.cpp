#include "FunctionView.h"
#include "../Global.h"
#include "../mainwindow.h"

#include <QGridLayout>
#include <QTreeView>
#include <QDebug>

FunctionView::FunctionView(QWidget *parent)
{
    QGridLayout* layout = new QGridLayout(this);

    m_FunctionTable = new QTreeView ;

    layout->addWidget(m_FunctionTable,0,0);
}

void FunctionView::insertFunctions(){
    QList<address_t> functions = MainWindow::instance()->m_DisassemblerDbModel->functionList();
    qDebug() << "func size " << functions.size();
    if(functions.size() <= 0) return;

    quint64 ModulBase = 0x00400000;

    Functions = new QStandardItemModel (functions.size(),1);
    Functions->setHorizontalHeaderItem( 0, new QStandardItem( "Name" ) );

    m_FunctionTable->setModel(Functions);

    for(int i = 0; i < functions.size(); i++)
    {
        QStandardItem *itm_name = new QStandardItem(MainWindow::instance()->m_DisassemblerDbModel->labelName(functions.at(i)));
        Functions->setItem(i,0,itm_name);



    }
}

QSize FunctionView::sizeHint() const
{
    return QSize(400,100);
}
