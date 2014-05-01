#ifndef RegisterView_H
#define RegisterView_H

#include <QAbstractScrollArea>
#include <QList>
#include <QStringList>

#include "../cls/clsstylesettings.h"

class RegisterView : public QAbstractScrollArea
{
    Q_OBJECT

private:
    int m_CurrentHighlightedLine;
    QList<qint32> m_CurrentProcessorRegisterStates;
    QStringList m_RegisterIndicies;
    clsStyleSettings m_Style;

    int m_NumberOfFields, m_maxValueWidth, m_maxNameWidth;


protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void contextMenuEvent(QContextMenuEvent *event);


public:
    explicit RegisterView(QWidget *parent = 0);
    QColor m_ColorRegister, m_ColorValue, m_ColorComment, m_ColorBackground;

    QSize sizeHint() const;


    void setNewRegisterValues(QList<quint32> newValues);
signals:

public slots:

    void repaint();

    void mnuTest();
    void mnuZeroRegisterValue();
    void mnuIncrementRegisterValue();
    void mnuDecrementRegisterValue();
    void mnuModifyRegisterValue();
    void mnuToggleRegisterValue();
    void mnuToClipboard();
    void mnuAllToClipboard();
};

#endif // RegisterView_H
