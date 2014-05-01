#include "JumpView.h"
#include "../mainwindow.h"

#include <QWidget>
#include <QAbstractScrollArea>
#include <QMap>
#include <QPainter>
#include <QDebug>

#include "../Models/DisassemblerDbModel.h"
#include "../Helper/ArrowHelper.h"
#include "../BeaEngine/BeaEngine.h"


/**
 * @brief JumpView::JumpView
 * @param parent
 *
 * represent the left panel of the disassembler view
 */
JumpView::JumpView( QWidget *parent) :  QAbstractScrollArea(parent)
{
    LastEIP = -1;
    QAbstractScrollArea::setFont(m_Style.m_DefaultFont);
}

/**
 * @brief JumpView::xCoordinate
 * @param p_CharInColumn
 * @return x coordinate
 *
 * calculates the coordinate from the character position
 */
const int JumpView::xCoordinate(const int p_CharInColumn){
    return m_Style.m_FontWidth*p_CharInColumn;
}

/**
 * @brief JumpView::yCoordinate
 * @param p_Line line number offset of visible lines
 * @return y coordinate
 *
 * calculates the coordinate from the character position
 */
const int JumpView::yCoordinate(const int p_Line){
    return m_Style.m_FontHeight*(p_Line+1);

}

/**
 * @brief JumpView::repaint
 *
 * wraps repaint()
 */
void JumpView::repaint() {
    viewport()->repaint();
}


/**
 * @brief JumpView::paintEvent
 * @param event
 *
 * will draw the panel
 */
void JumpView::paintEvent(QPaintEvent *)
{
    // set background to white!
    painter = new QPainter(viewport());
    painter->setBrush(QBrush(QColor("#ffffff")));
    painter->drawRect(this->rect());

    // start from given instruction
    QMap<quint64,DISASM>::const_iterator it = m_StartingInstruction;
    // count the number of lines from viewport start (vertical)
    int LineOffset = 0;


    QMap<quint64,DISASM>::const_iterator CurrentClickedLine = NULL;
    if(clickedLine!=-1){
       CurrentClickedLine = m_StartingInstruction+clickedLine;
    }

    // until we reach the last instruction line from disassembler view
    // we have to work
    while(it!= m_EndInstruction){
        // draw EIP label
        if(m_CurrentEIP == it.key()){
            // rectangle and text
            painter->setBrush(QBrush(QColor("#4040ff")));
            painter->setPen(QPen(QColor("#4040ff")));
            int yy = LineOffset*m_Style.m_FontHeight+0.2*m_Style.m_FontHeight;

            painter->drawRect(1,yy,3*m_Style.m_FontWidth,m_Style.m_FontHeight);
            painter->setPen(QPen(QColor("#ffffff")));
            painter->drawText(2,yCoordinate(LineOffset),"EIP");
            yy = (LineOffset+1)*m_Style.m_FontHeight-m_Style.m_FontHeight/3.0;
            painter->setPen(QPen(QColor("#4040ff"),2));
            painter->setBrush(QBrush(QColor("#4040ff")));
            ArrowHelper::drawStraightArrow(painter,3*m_Style.m_FontWidth+3,yy,this->viewport()->width()-2,yy);
        }

        // draw jump arrows
        painter->setPen(QPen(QColor("#4040ff")));
        // is there a jump ?
        if(m_DisasmModel->existsJumpFrom(it.key()) ){

            address_t TargetAdr = m_DisasmModel->targetAdressFromJump(it.key());
            int drawOffset = m_DisasmModel->offsetOfJump(it.key())+1;

            if(LineOffset==clickedLine || ( CurrentClickedLine!=NULL && TargetAdr == CurrentClickedLine.value().VirtualAddr )){
                if((it.value().Instruction.Opcode == 0xEB) || it.value().Instruction.Opcode == 0xE9){
                    // jmp
                    painter->setPen(QPen(QColor("#000000"),1, Qt::SolidLine));
                }else{
                    painter->setPen(QPen(QColor("#000000"),1, Qt::DashLine));
                }

            }else{
                painter->setPen(QPen(QColor("#c0c0c0"),1, Qt::DashLine));
            }
            // debug draws jump offset
            //painter->drawText(2,yCoordinate(LineOffset),QString("%1").arg(drawOffset));

            /* there are 3 cases:
             *  - jumps from within viewport to within viewport              A
             *  - jumps from within viewport to outside viewport (top)       B
             *  - jumps from within viewport to outside viewport (bottom)    C
             */

            if(TargetAdr > m_StartingInstruction.key() && TargetAdr < m_EndInstruction.key()){
                // CASE: A
                const int StartYCord = (LineOffset+1)*m_Style.m_FontHeight-m_Style.m_FontHeight/3.0;
                // find y coordinate of jump target
                int addOffset = 0;
                QMap<quint64,DISASM>::const_iterator it2 = it;
                while(it2.key()!=TargetAdr){
                    it2++;
                    addOffset++;
                }
                const int EndYCord = (LineOffset+1+addOffset)*m_Style.m_FontHeight-m_Style.m_FontHeight/3.0;
                ArrowHelper::drawJumpArrowInViewport(painter,m_Style.m_FontWidth*2,this->viewport()->width(),StartYCord,EndYCord,drawOffset);
            }else{
                if(TargetAdr < m_StartingInstruction.key() ){
                    // CASE: B
                    // jumps target is above viewport start
                    const int StartYCord = (LineOffset+1)*m_Style.m_FontHeight-m_Style.m_FontHeight/3.0;
                    ArrowHelper::drawJumpArrowLeaveTopViewport(painter,m_Style.m_FontWidth*2,this->viewport()->width(),StartYCord,drawOffset);
                }else{
                    // CASE: C
                    // jumps target is below viewport end
                    const int StartYCord = (LineOffset+1)*m_Style.m_FontHeight-m_Style.m_FontHeight/3.0;
                    ArrowHelper::drawJumpArrowLeaveBottomViewport(
                                painter,
                                m_Style.m_FontWidth*2,
                                this->viewport()->width(),
                                StartYCord,drawOffset,
                                this->viewport()->height()
                                );
                }
            }
        }
        // new line please
        LineOffset++;
        // next instruction please
        it++;
    }
    LastEIP = m_CurrentEIP;
    delete painter;

}


void JumpView::displayDisassembly(quint64 dwEIP,
                                  quint32 dwClickedLine,
                                  QMap<quint64,DISASM>* Data,
                                  QMap<quint64,DISASM>::const_iterator FirstLine,
                                  QMap<quint64,DISASM>::const_iterator LastLine)
{
    m_StartingInstruction = FirstLine;
    m_EndInstruction = LastLine;
    m_CurrentEIP = dwEIP;
    m_VisibleLines = this->viewport()->height() / m_Style.m_FontHeight;
    m_isSomethingToDisplay = true;
    m_DisasmInfo = Data;
    m_DisasmModel = MainWindow::instance()->m_DisassemblerDbModel;
    clickedLine = dwClickedLine;
    repaint();

}

QSize JumpView::sizeHint() const
{
    return QSize(m_Style.m_FontWidth*4,this->viewport()->height());
}
