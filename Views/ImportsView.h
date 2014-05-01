#ifndef IMPORTSVIEW_H
#define IMPORTSVIEW_H

#include <QStandardItemModel>
#include <QTreeView>
#include "../cls/clsPEManager.h"

class ImportsView: public QWidget
{
    Q_OBJECT
private:

    clsPEManager *m_PeManager;
    QTreeView* m_ImportsTable;

    QStandardItemModel *Imports;


public:
    ImportsView(clsPEManager* PEm, QWidget* parent);

    void InsertImports(std::wstring p_FileName);
};

#endif // IMPORTSVIEW_H
