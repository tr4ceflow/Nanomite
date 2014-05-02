/**
  * This represents the disassembler view and will show the instructions, ...
  */

#include "DynamicDisassemblyView.h"
#include <QAbstractScrollArea>
#include <QColor>
#include <QPainter>
#include <QMap>
#include <QMessageBox>

#include "../mainwindow.h"

#include "../Models/DisassemblerModel.h"
#include "../Models/DisassemblerDbModel.h"
#include "../cls/clsstringhelper.h"

#include "../BeaEngine/BeaEngine.h"

#define ESReg 1
#define DSReg 2
#define FSReg 3
#define GSReg 4
#define CSReg 5
#define SSReg 6

/**
 * @brief DisassemblerCodeView::DisassemblerCodeView
 * @param disasmdb     Model that represents extra information (user comments, label names, ...)
 * @param parent
 */
DynamicDisassemblyView::DynamicDisassemblyView( QWidget *parent) :  QAbstractScrollArea(parent)
{
    // calculate maximum of visible lines using the default font
    m_VisibleLines = this->height() / m_Style.m_FontHeight;
    // set default font
    QAbstractScrollArea::setFont(m_Style.m_DefaultFont);

    // flag that indicates of the view has to paint something
    m_isSomethingToDisplay = false;

    // reference to extra information model
    m_DiasmDbModel = MainWindow::instance()->m_DisassemblerDbModel;
    //m_DiasmDbModel = disasmdb;

    m_SegmentNames << "" <<"es" << "ds" << "fs" << "gs" << "cs" << "ss";

    m_SizeNames = new QMap<int,QString>;
    m_SizeNames->insert(8,"byte");
    m_SizeNames->insert(16,"word");
    m_SizeNames->insert(32,"dword");
    m_SizeNames->insert(64,"qword");
    m_SizeNames->insert(80,"tword");
    m_SizeNames->insert(128,"dqword");
    m_SizeNames->insert(256,"yword");
    m_SizeNames->insert(512,"zword");

    m_CurrentClickedLine = -1;




}

void DynamicDisassemblyView::resizeEvent(QResizeEvent *event)
{
    // TODO
}

/**
 * @brief DynamicDisassemblyView::xCoordinate
 * @param CharInColumn
 * @return x coordinate
 *
 * calculates the coordinate from the character position
 */
const int DynamicDisassemblyView::xCoordinate(const int CharInColumn){
    return m_Style.m_FontWidth*CharInColumn;
}

/**
 * @brief DynamicDisassemblyView::yCoordinate
 * @param Line line number offset of visible lines
 * @return y coordinate
 *
 * calculates the coordinate from the character position
 */
const int DynamicDisassemblyView::yCoordinate(const int Line){
    return m_Style.m_FontHeight*(Line+1);
}

/**
 * @brief DynamicDisassemblyView::displayArgument
 * @param Line            line in view
 * @param Col             starting character position
 * @param Instruction     BeaEngine Instruction
 * @param Argument        current Argument
 * @return
 *
 * will display the argument from the given instruction
 */
int DynamicDisassemblyView::displayArgument(const int Line, const int Col, const DISASM* Instruction, const ARGTYPE *Argument){
    // length to compute the starting position of the next argument
    int ArgumentTextLength = 0;

    const int y = yCoordinate(Line);
    int x = xCoordinate(Col);

    // get type of argument
    int argtype=Argument->ArgType;

    // get mnemonic (adress)
    QString ArgMne=QString(Argument->ArgMnemonic);

    if(argtype!=NO_ARGUMENT && *Argument->ArgMnemonic)
    {
        if(argtype&MEMORY_TYPE)
        {
            // argument is adress (no register!)

            // extract segment (like x64dbg)
            QString SegmentName = m_SegmentNames.at(Argument->SegmentReg);

            // insert labels
            quint64 GivenAdress=Argument->Memory.Displacement;
            if(Argument->ArgType&RELATIVE_)
                GivenAdress=Instruction->Instruction.AddrValue;

            // do we have a label for given address?
            if(m_DiasmDbModel->hasLabel(GivenAdress)){
                // if so replace to adress
                QString LabelText=QString("%1").arg(GivenAdress, 0, 16, QChar('0')).toUpper()+"h";
                if(ArgMne.indexOf(LabelText)!=-1)
                {
                    QString NewLabelText = m_DiasmDbModel->labelName(GivenAdress);
                    ArgMne.replace(LabelText, QString(NewLabelText)+">");
                }

            }


            QString Tmp;
            QString SizeStr = m_SizeNames->find(Argument->ArgSize).value();
            Tmp = QString("%1 ptr %2:[%3]").arg(SizeStr,SegmentName, ArgMne);
            ArgumentTextLength += Tmp.length();
            int itmp = Tmp.length();
            painter->drawText(x, y,Tmp);
            x += xCoordinate(itmp);
        }
        else
        {
            // argument ist constant

            quint64 GivenAdress=Instruction->Instruction.Immediat;
            if(!GivenAdress)
                GivenAdress=Instruction->Instruction.AddrValue;

            bool AdressHasLabel = m_DiasmDbModel->hasLabel(GivenAdress);

            QString GivenAdressAsText=QString("%1").arg(GivenAdress, 8, 16, QChar('0')).toUpper() +"h";
            if(AdressHasLabel)
            {
                QString NewLabelText = m_DiasmDbModel->labelName(GivenAdress);
                if(ArgMne.indexOf(GivenAdressAsText)!=-1)
                {
                    ArgMne.replace(GivenAdressAsText, NewLabelText);
                }
            }

            //jumps
            char shortjmp[100]="\0";
            bool has_shortjmp=false;
            int BranchType=Instruction->Instruction.BranchType;
            if(BranchType && BranchType!=RetType && !(argtype&REGISTER_TYPE))
            {
                unsigned char* opc=(unsigned char*)&Instruction->Instruction.Opcode;
                if(*opc==0xEB || *opc==0xE9 || Instruction->Instruction.Opcode<0x80)
                {
                    strcpy(shortjmp+1, "short ");
                    has_shortjmp=true;
                }
            }

            if(clsStringHelper::isHex(ArgMne)){
                QString Tmp2 = QString("%1").arg( shortjmp+has_shortjmp);

                ArgumentTextLength += Tmp2.length();
                int itmp = Tmp2.length();
                painter->drawText(x, y,Tmp2);
                x += xCoordinate(itmp);

                painter->save();
                painter->setPen(QPen(m_Style.m_ColorValue));

                Tmp2 = QString("%1").arg(ArgMne);
                ArgumentTextLength += Tmp2.length();
                itmp = Tmp2.length();
                painter->drawText(x, y,Tmp2);
                painter->restore();
                x += xCoordinate(itmp);
            }else{
                QString Tmp2 = QString("%1%2").arg( shortjmp+has_shortjmp,ArgMne);
                ArgumentTextLength += Tmp2.length();
                int itmp = Tmp2.length();
                painter->drawText(x, y,Tmp2);
                x += xCoordinate(itmp);
            }



        }

    }
    return ArgumentTextLength;

}

void DynamicDisassemblyView::mouseReleaseEvent(QMouseEvent *e)
{
    const int y = e->pos().y();
    const int clickline = y/m_Style.m_FontHeight;

    if(clickline != m_CurrentClickedLine){
        m_CurrentClickedLine = clickline;
        repaint();
    }
}

/**
 * @brief DynamicDisassemblyView::displayBaseInstruction
 * @param Line      current line in view
 * @param Col       current char pos in line
 * @param Data      disasm struct for current line
 */
void DynamicDisassemblyView::displayBaseInstruction(const int Line, const int Col, const DISASM *Data){
    // line number to y coordinate
    const int y = m_Style.m_FontHeight*(Line+1);
    // character pos to x coordinate
    int x =xCoordinate(Col);

    // ida blue for instructions
    painter->setPen(QPen(QColor("#000080")));
    QString InstrMne = QString("%1").arg(Data->Instruction.Mnemonic);
    painter->drawText(x, y,InstrMne);
}

/**
 * @brief DynamicDisassemblyView::displayLine
 * @param Line      current line in view
 * @param Data      disasm struct for current line
 */
void DynamicDisassemblyView::displayLine(const int Line, const DISASM *Data){
    int y = m_Style.m_FontHeight*(Line+1);
    int x;

    quint64 ModulBase = 0x00400000;
    QString SectionName = "";
    SectionName = m_Sections.at(0).SectionName;

    for(QList<PESectionData>::iterator it=m_Sections.begin();it!=m_Sections.end();it++){
        if( Data->VirtualAddr - ModulBase > (*it).VirtualAddress){
            break;
        }
        SectionName = it->SectionName;
    }
    // draw ".text"
    x =xCoordinate(1);
    if(m_CurrentClickedLine == Line){
        painter->setPen(QPen(QColor("#0000ff")));
    }else{
        painter->setPen(QPen(QColor("#333")));
    }

    painter->drawText(x, y,SectionName+":");

    // draw address
    x = xCoordinate(7);
    QString Loc = QString("%1").arg(Data->VirtualAddr,8,16,QChar('0')).toUpper();
    painter->drawText(x, y, Loc);


    // draw label ?

    QString Label = m_DiasmDbModel->labelName(Data->VirtualAddr);
    if(Label.length()!=0){
        // ; ---------------------------------------------------------------------------
        painter->setPen(QPen(QColor("#808080"),1,Qt::DashLine));
        const int yy = y-m_Style.m_FontHeight+3;
        painter->drawLine(0,yy,this->width(),yy);
        painter->setPen(QPen(QColor("#0000ff")));
        painter->drawText(xCoordinate(17), y, Label+":");
    }

    // draw instruction mnemonic
    displayBaseInstruction(Line,32,Data);



    // draw arguments
    bool Arg1Visible=false, Arg2Visible=false, Arg3Visible=false;

    int offset = displayArgument(Line,40,Data,&Data->Argument1);
    if(offset!=0){
        //
        Arg1Visible = true;
        offset += 2;
    }
    int offset2 = displayArgument(Line,40+offset,Data,&Data->Argument2);
    if(offset2!=0){
        Arg2Visible = true;
        offset2 += 2;
    }

    int offset3 = displayArgument(Line,40+offset+offset2,Data,&Data->Argument3);

    // do we need a "," ?
    if(Arg2Visible && (offset!=0)){
        painter->drawText( xCoordinate(40+offset-2),y,QString(","));
    }

    if(m_DiasmDbModel->hasComment(Data->VirtualAddr))
    {
        painter->setPen(QPen(QColor("#808080")));
        painter->drawText(xCoordinate(85), y, m_DiasmDbModel->comment(Data->VirtualAddr));
    }


}

void DynamicDisassemblyView::displayDisassembly(quint64 dwEIP,QMap<quint64,DISASM>* Data)
{

    m_CurrentEIP = dwEIP;
    m_VisibleLines = this->height() / m_Style.m_FontHeight;
    m_isSomethingToDisplay = true;
    m_DisasmInfo = Data;
    repaint();


}

void DynamicDisassemblyView::setPESectionData(QList<PESectionData> Sections)
{
    m_Sections = Sections;

}
void DynamicDisassemblyView::repaint() {
    viewport()->repaint();

}

void DynamicDisassemblyView::paintEvent(QPaintEvent *)
{

    painter = new QPainter(viewport());
    painter->setPen(QPen(m_Style.m_ColorValue));

    // default brushes
    painter->save();

    // paint background
    painter->setBrush(QBrush(m_Style.m_ColorBackground));
    //painter->drawRect(this->rect());
    painter->restore();

    // is data to paint?
    if(m_isSomethingToDisplay){

        painter->setPen(QPen(m_Style.m_ColorValue));
        QMap<quint64,DISASM>::const_iterator First;
        QMap<quint64,DISASM>::const_iterator Last;
        // start drawing
        QMap<quint64,DISASM>::const_iterator it = m_DisasmInfo->constFind(m_CurrentEIP);
        for(int i=0;i<3;i++){
            if(it != m_DisasmInfo->constBegin())
                it--;
        }
        First = it;
        // iterate each line

        for(int i=0;i<m_VisibleLines;i++){

            // highlight current line?
            if(m_CurrentEIP == it.key()){
                const int wd = this->width() / m_Style.m_FontWidth;
                painter->setBrush(QBrush(m_Style.m_ColorHighlight));
                painter->setPen(QPen(m_Style.m_ColorHighlight));
                painter->drawRect(1,i*m_Style.m_FontHeight+0.2*m_Style.m_FontHeight,this->width(),m_Style.m_FontHeight);
            }

            // display instructions
            displayLine(i,&(*it));


            if(it != m_DisasmInfo->constEnd())
                ++it;
            else
                break;

        }
        Last = it;

        emit OnDisassemblyIsReady(m_CurrentEIP,m_CurrentClickedLine,m_DisasmInfo,First,Last);

    }
    //m_isSomethingToDisplay = false;

    delete painter;


}
