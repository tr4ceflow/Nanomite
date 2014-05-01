#include "ImportsView.h"

#include <QGridLayout>
#include <QTreeView>
#include <QStandardItemModel>
#include "../cls/clsPEManager.h"

ImportsView::ImportsView(clsPEManager* PEm, QWidget* parent)
{
    QGridLayout *layout = new QGridLayout(this);

    m_ImportsTable = new QTreeView ;
    m_PeManager = PEm;

    layout->addWidget(m_ImportsTable,0,0);
}

void ImportsView::InsertImports(std::wstring p_FileName)
{
    QList<APIData> imports = m_PeManager->getImports(p_FileName);
    if(imports.size() <= 0) return;

    quint64 ModulBase = 0x00400000;

    Imports = new QStandardItemModel (imports.size(),3);
    Imports->setHorizontalHeaderItem( 0, new QStandardItem( "Address" ) );
    Imports->setHorizontalHeaderItem( 1, new QStandardItem( "Name" ) );
    Imports->setHorizontalHeaderItem( 2, new QStandardItem( "Library" ) );

    m_ImportsTable->setModel(Imports);

    for(int importCount = 0; importCount < imports.size(); importCount++)
    {
        QStringList currentElement = imports.value(importCount).APIName.split("::");

        /*if(currentElement[0].compare(lastTopElement) != NULL)
        {
            moduleElement = new QTreeWidgetItem(topElement);
            moduleElement->setText(0,currentElement[0]);
            lastTopElement = currentElement[0];
            //moduleBase = clsHelperClass::CalcOffsetForModule((PTCHAR)currentElement[0].toLower().toStdWString().c_str(),NULL,_PID);
        }
*/

        QStandardItem *itm_address = new QStandardItem(QString("%1").arg(imports.value(importCount).APIOffset +ModulBase,8,16,QChar('0')));
        QStandardItem *itm_name = new QStandardItem(currentElement[1]);
        QStandardItem *itm_lib = new QStandardItem(currentElement[0]);

        Imports->setItem(importCount,0,itm_address);
        Imports->setItem(importCount,1,itm_name);
        Imports->setItem(importCount,2,itm_lib);


    }
}
