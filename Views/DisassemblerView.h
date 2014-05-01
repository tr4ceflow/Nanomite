#ifndef DisassemblerView_H
#define DisassemblerView_H

#include <QAbstractScrollArea>
#include <QPainter>
#include <QStringList>
#include <QMap>
#include <QString>
#include "../cls/clsstylesettings.h"
#include "../Models/DisassemblerModel.h"
#include "../Models/DisassemblerDbModel.h"

class DisassemblerView : public QAbstractScrollArea
{
    Q_OBJECT
public:
    explicit DisassemblerView(QWidget *parent = 0);

    virtual void paintEvent(QPaintEvent *event);

    void resizeEvent(QResizeEvent* event);
    void repaint();
    void displayDisassembly(quint64 dwEIP,QMap<quint64,DISASM>* Data);

    DisassemblerDbModel *m_DiasmDbModel;

private:
    QStringList m_SegmentNames;
    QMap<int,QString> *m_SizeNames;
    QMap<quint64,DISASM> *m_DisasmInfo;
    QList<PESectionData> m_Sections;


    QPainter *painter;

    quint64 m_CurrentEIP;

    clsStyleSettings m_Style;

    int m_VisibleLines;
    int m_CurrentClickedLine;

    bool m_isSomethingToDisplay;



signals:
    void OnDisassemblyIsReady(
            quint64 eip,
            quint32 Line,
            QMap<quint64,DISASM>* Data,
            QMap<quint64,DISASM>::const_iterator First,
            QMap<quint64,DISASM>::const_iterator Last);
public slots:
    void setPESectionData( QList<PESectionData> Sections );
protected:
    void displayLine(const int Line, const DISASM *Data);
    const int xCoordinate(const int column);
    const int yCoordinate(const int line);
    void displayBaseInstruction(const int Line, const int Col, const DISASM *Data);
    int displayArgument(const int Line, const int Col, const DISASM* Instruction,  const ARGTYPE *Arg);

    virtual void mouseReleaseEvent ( QMouseEvent * e );
};

#endif // DisassemblerView_H
