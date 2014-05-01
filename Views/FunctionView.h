#ifndef FUNCTIONVIEW_H
#define FUNCTIONVIEW_H

#include <QStandardItemModel>
#include <QTreeView>

class FunctionView: public QWidget
{
    Q_OBJECT
    QTreeView* m_FunctionTable;

    QStandardItemModel *Functions;
public:
    FunctionView(QWidget* parent=0);
    QSize sizeHint() const;
    void insertFunctions();
};

#endif // FUNCTIONVIEW_H
