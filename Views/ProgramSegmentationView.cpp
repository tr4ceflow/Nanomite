#include "ProgramSegmentationView.h"
#include "../cls/clsPEManager.h"
#include <QWidget>
#include <QGridLayout>
#include <QStandardItemModel>
#include <QTreeWidgetItem>
#include <QListView>

ProgramSegmentationView::ProgramSegmentationView(clsPEManager* p_PeManager, QWidget*)
{
    QGridLayout *layout = new QGridLayout(this);

    SegmentationTable = new QTreeView ;
    m_PeManager = p_PeManager;


    layout->addWidget(SegmentationTable,0,0);

}

void ProgramSegmentationView::InsertSections(std::wstring p_FileName)
{
    m_Filename = p_FileName;
    QList<PESectionData> sections = m_PeManager->getSections(m_Filename);
    if(sections.size() <= 0) return;

    Segm = new QStandardItemModel (sections.size(),3);
    Segm->setHorizontalHeaderItem( 0, new QStandardItem( "Name" ) );
    Segm->setHorizontalHeaderItem( 1, new QStandardItem( "Start" ) );
    Segm->setHorizontalHeaderItem( 2, new QStandardItem( "End" ) );

    SegmentationTable->setModel(Segm);

    quint64 ModulBase = 0x00400000;


    for( int r=0; r<sections.size(); r++ ){
        QStandardItem *itm_name = new QStandardItem( sections.at(r).SectionName );
        quint64 dwSegBegin = ModulBase + sections.at(r).VirtualAddress;
        quint64 dwSegEnd   = ModulBase + sections.at(r).VirtualAddress + sections.at(r).VirtualSize;

        QStandardItem *itm_start = new QStandardItem(QString("%1").arg(dwSegBegin,8,16,QChar('0') ));
        QStandardItem *itm_end = new QStandardItem(QString("%1").arg(dwSegEnd,8,16,QChar('0') ));

        Segm->setItem(r,0,itm_name);
        Segm->setItem(r,1,itm_start);
        Segm->setItem(r,2,itm_end);

    }


}
