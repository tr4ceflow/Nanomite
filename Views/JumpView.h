#ifndef JUMPVIEW_H
#define JUMPVIEW_H

#include <QAbstractScrollArea>
#include <QMap>

#include "../Models/DisassemblerDbModel.h"

#include "../cls/clsstylesettings.h"
#include "../BeaEngine/BeaEngine.h"

class JumpView: public QAbstractScrollArea
{
    Q_OBJECT
private:
    clsStyleSettings m_Style;
    QPainter *painter;

    int m_VisibleLines;
    quint64 m_CurrentEIP,LastEIP;
    quint32 clickedLine;
    bool m_isSomethingToDisplay;
    QMap<quint64,DISASM> *m_DisasmInfo;
    DisassemblerDbModel *m_DisasmModel;

    QMap<quint64,DISASM>::const_iterator m_StartingInstruction;
    QMap<quint64,DISASM>::const_iterator m_EndInstruction;

public:
    JumpView( QWidget *parent=0);
public slots:
    void displayDisassembly(quint64 dwEIP, quint32 dwClickedLine,
                            QMap<quint64,DISASM>* Data,
                            QMap<quint64,DISASM>::const_iterator FirstLine,
                            QMap<quint64,DISASM>::const_iterator LastLine);

protected:
    const int xCoordinate(const int CharInColumn);
    const int yCoordinate(const int Line);
    void repaint();
    virtual void paintEvent(QPaintEvent *event);
    QSize sizeHint() const;
};

#endif // JUMPVIEW_H
