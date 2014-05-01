#ifndef PROGRAMSEGMENTATIONVIEW_H
#define PROGRAMSEGMENTATIONVIEW_H

#include <QListView>
#include <QTreeView>
#include <QStandardItemModel>
#include "../cls/clsPEManager.h"

class ProgramSegmentationView : public QWidget
{
    Q_OBJECT
private:
    QTreeView  *SegmentationTable;
    clsPEManager *m_PeManager;
    std::wstring m_Filename;

    QStandardItemModel* Segm;
public:
    ProgramSegmentationView(clsPEManager* PEm, QWidget* parent);
    void InsertSections(std::wstring FileName);
};

#endif // PROGRAMSEGMENTATIONVIEW_H
