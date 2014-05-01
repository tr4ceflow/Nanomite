#include "RegisterView.h"
#include "../cls/clsstylesettings.h"
#include "../cls/clsStringHelper.h"

#include <QApplication>
#include <QAbstractScrollArea>
#include <QStringList>
#include <QPainter>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPoint>
#include <QMenu>
#include <QInputDialog>
#include <QClipboard>



void RegisterView::paintEvent(QPaintEvent *)
{
    // start drawing
    QPainter painter(viewport());

    // default brushes
    painter.save();

    // paint background
    painter.setBrush(QBrush(m_Style.m_ColorBackground));
    painter.drawRect(this->rect());

    // draw highlighted line
    if(m_CurrentHighlightedLine!=-1){
        painter.setBrush(QBrush(m_Style.m_ColorHighlight));
        painter.setPen(QPen(m_Style.m_ColorHighlight));
        painter.drawRect(m_Style.m_FontWidth*(m_maxNameWidth+2),m_CurrentHighlightedLine*m_Style.m_FontHeight+0.2*m_Style.m_FontHeight,m_Style.m_FontWidth*8,m_Style.m_FontHeight);
    }

    painter.restore();

    // draw Registers
    painter.setPen(QPen(m_Style.m_ColorRegister));
    for(int i=0;i<m_RegisterIndicies.size();i++)
        painter.drawText(m_Style.m_FontWidth*1, m_Style.m_FontHeight*(i+1), m_RegisterIndicies.at(i));


    // draw Values
    painter.setPen(QPen(m_Style.m_ColorValue));

    const int iCommentPadding = (m_maxNameWidth+m_maxValueWidth+3);

    // draw possible ascii char
    for(int i=0;i<m_NumberOfFields;i++){
        painter.drawText(m_Style.m_FontWidth*(m_maxNameWidth+2), m_Style.m_FontHeight*(i+1), clsStringHelper::intToHexString(m_CurrentProcessorRegisterStates.at(i)));
        if(clsStringHelper::isPrintable(m_CurrentProcessorRegisterStates.at(i))){
            painter.setPen(QPen(m_Style.m_ColorComment));
            painter.drawText(m_Style.m_FontWidth*iCommentPadding, m_Style.m_FontHeight*(i+1), "'"+QString((char)(m_CurrentProcessorRegisterStates.at(i)))+"'");
            painter.setPen(QPen(m_Style.m_ColorValue));
        }
    }




}
void RegisterView::repaint() {
    viewport()->repaint();
}

void RegisterView::setNewRegisterValues(QList<quint32> newValues){
    m_CurrentProcessorRegisterStates.clear();
    for(int i=0;i<newValues.size();i++)
        m_CurrentProcessorRegisterStates.append(newValues.at(i));
    repaint();
}

RegisterView::RegisterView(QWidget * parent) : QAbstractScrollArea(parent)
{
    m_Style = clsStyleSettings();

    m_RegisterIndicies << "EAX" << "EBX" << "ECX" << "EDX" << "ESI" << "EDI" << "EBP" << "ESP" << "EIP" << "EFL";
    for(int i=0;i<m_RegisterIndicies.count();i++)
        m_CurrentProcessorRegisterStates <<0;

    m_NumberOfFields = m_RegisterIndicies.size();
    m_maxValueWidth = 8;
    m_maxNameWidth = 3;

    m_CurrentHighlightedLine = -1;
    QAbstractScrollArea::setFont(m_Style.m_DefaultFont);
}

void RegisterView::mouseDoubleClickEvent(QMouseEvent *event)
{
    const int row = event->y()/(double)m_Style.m_FontHeight;
    const int col = event->x()/(double)m_Style.m_FontWidth;

    if(row <m_NumberOfFields && row >=0 && col>0 && col <18){
        m_CurrentHighlightedLine = row;
        repaint();
    }
}

void RegisterView::contextMenuEvent(QContextMenuEvent *event)
{
    const int row = event->y()/(double)m_Style.m_FontHeight;
    const int col = event->x()/(double)m_Style.m_FontWidth;

    if(row <m_NumberOfFields && row >=0 && col>0 && col <18){
        m_CurrentHighlightedLine = row;
        repaint();


        QMenu *const menu = new QMenu(this);
        menu->addAction(tr("Modify value..."), this, SLOT(mnuModifyRegisterValue()));
        menu->addAction(tr("Zero value"), this, SLOT(mnuZeroRegisterValue()),QKeySequence(tr("0")));
        menu->addAction(tr("Toggle value"), this, SLOT(mnuToggleRegisterValue()),QKeySequence(tr("space")));
        menu->addAction(tr("Increment value"), this, SLOT(mnuIncrementRegisterValue()),QKeySequence(tr("+")));
        menu->addAction(tr("Decrement value"), this, SLOT(mnuDecrementRegisterValue()),QKeySequence(tr("-")));
        menu->addAction(tr("Copy"), this, SLOT(mnuToClipboard()));
        menu->addAction(tr("Copy all"), this, SLOT(mnuAllToClipboard()));

        menu->exec(event->globalPos());
        delete menu;
    }


}


void RegisterView::mnuTest()
{
    QMessageBox::information(this,"Register Clicked",m_RegisterIndicies[m_CurrentHighlightedLine]);
}

void RegisterView::mnuToggleRegisterValue()
{

    m_CurrentProcessorRegisterStates[m_CurrentHighlightedLine]++;
    m_CurrentProcessorRegisterStates[m_CurrentHighlightedLine] *= -1;
    repaint();

}
void RegisterView::mnuModifyRegisterValue()
{
    bool ok;

    QString text = QInputDialog::getText(this, tr("Modify register value"),tr("new value:"), QLineEdit::Normal,clsStringHelper::intToHexString(m_CurrentProcessorRegisterStates[m_CurrentHighlightedLine]), &ok);

    bool conversionOk = false;
    int value = text.toInt(&conversionOk, 16);
    if (conversionOk && text.length() > 0)
    {
        m_CurrentProcessorRegisterStates[m_CurrentHighlightedLine]=value;
        repaint();
    }

}

void RegisterView::mnuZeroRegisterValue()
{
    m_CurrentProcessorRegisterStates[m_CurrentHighlightedLine]=0;
    repaint();
}

void RegisterView::mnuIncrementRegisterValue()
{
    m_CurrentProcessorRegisterStates[m_CurrentHighlightedLine]++;
    repaint();
}
void RegisterView::mnuDecrementRegisterValue()
{
    m_CurrentProcessorRegisterStates[m_CurrentHighlightedLine]--;
    repaint();
}
void RegisterView::mnuToClipboard()
{
    QClipboard *p_Clipboard = QApplication::clipboard();
    p_Clipboard->setText(clsStringHelper::intToHexString(m_CurrentProcessorRegisterStates[m_CurrentHighlightedLine]));

}
void RegisterView::mnuAllToClipboard()
{
    QString clip = "";
    for(int i=0;i<m_NumberOfFields;i++){
        clip.append( m_RegisterIndicies.at(i));
        clip.append(" ");
        clip.append( clsStringHelper::intToHexString(m_CurrentProcessorRegisterStates.at(i)));


        if(clsStringHelper::isPrintable(m_CurrentProcessorRegisterStates.at(i))){

            clip.append( " '"+QString((char)(m_CurrentProcessorRegisterStates.at(i)))+"'");

        }
        clip.append("\n");
    }


    QClipboard *p_Clipboard = QApplication::clipboard();
    p_Clipboard->setText(clip);

}



QSize RegisterView::sizeHint() const
{
    return QSize(m_Style.m_FontWidth*20,m_Style.m_FontHeight*11);
}
