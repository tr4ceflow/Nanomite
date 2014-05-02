

#include "disassemblerdbmodel.h"
#include <QSqlDatabase>
#include <QDebug>
#include <QFileInfo>
#include <QSqlQuery>
#include <QMap>
#include <QString>
#include "../BeaEngine/BeaEngine.h"

DisassemblerDbModel::DisassemblerDbModel(QWidget * parent) : QWidget(parent)
{

}

void DisassemblerDbModel::openNewFile(QString Filename)
{
    m_StaticDisassembly.clear();
    m_TargetFilename = Filename;

}

QList<address_t> DisassemblerDbModel::functionList()
{
    return m_Functions;
}

QString DisassemblerDbModel::comment(address_t addr)
{
    QMap<address_t,QString>::ConstIterator it = m_Comments.constFind(addr);

    if(it != m_Comments.constEnd())
        return it.value();
    else
        return "";
}

void DisassemblerDbModel::addInstruction(DISASM Instruction){
    m_StaticDisassembly.insert(Instruction.VirtualAddr,Instruction);
    analyzeInstruction(Instruction);
}

void DisassemblerDbModel::analyzeInstruction(DISASM Instruction){
    // resolve Jumps
    if(Instruction.Instruction.BranchType && Instruction.Instruction.BranchType!=RetType && !(Instruction.Argument1.ArgType &REGISTER_TYPE))
    {

        // is jump --> remember target location
        QString targetloc = Instruction.Argument1.ArgMnemonic;
        //qDebug() << "found jump at" << newDisAss.EIP << " with "<< targetloc ;
        targetloc.replace("h","");

        bool ok;
        quint64 l = targetloc.toUInt(&ok,16);
        if(ok){
            // reolve calls ( a special kind of Jumps
            if(Instruction.Instruction.Opcode == 0xE8){
                addCall(Instruction.VirtualAddr,l);
            }else{
                 addJump(Instruction.VirtualAddr,l);
            }

            //dismodel->setJumpLocationName(l,"loc_"+targetloc);
        }




    }

}

void DisassemblerDbModel::createTables(){


}

void DisassemblerDbModel::createJumpInformation()
{
    // create jump informations for drawing
    QMap<address_t,address_t>::ConstIterator JumpPtr = m_JumpTable.begin();
    QMap<address_t,int>::ConstIterator JumpDeepPtr = m_JumpDeep.begin();

    int JumpDrawingOffset = 0;
    QList<address_t> TargetAdresses;

    // iterate all jumps (beginning from the topmost)
    while(JumpPtr != m_JumpTable.end()){

        // jump starts in CurrentAdress
        address_t StartAdress = JumpPtr.key();
        JumpDrawingOffset++;

        // does temp targetadresses has adress below current adress?
        for(QList<address_t>::iterator  it = TargetAdresses.begin();it!=TargetAdresses.end();it++){
            if((*it) < StartAdress){
                // there is a jump that finished in a adress before
                // so no need for such a deep
                JumpDrawingOffset--;
                // we do not have to care about that jump anymore
                it = TargetAdresses.erase(it);
                it = TargetAdresses.begin();
            }
        }

        // remember target adress
        TargetAdresses.append(JumpPtr.value());

        m_JumpDeep[JumpPtr.key()] = JumpDrawingOffset;

        JumpPtr++;

    }

}

bool DisassemblerDbModel::hasComment(address_t addr)
{
    return (m_Comments.find(addr) != m_Comments.end());
}

void DisassemblerDbModel::addCall(address_t start_adress, address_t target_adress)
{
    m_Labels.insert(target_adress,"sub_"+QString("%1").arg(target_adress,6,16,QChar('0')).toUpper());
    m_Functions.append(target_adress);
    // prevent the target adress from beeing a jump target adress (so remove it)
    //removeJumpTo(target_adress);

}

bool DisassemblerDbModel::existsJumpFrom(address_t start_adress){
    QMap<address_t,address_t>::ConstIterator it = m_JumpTable.constFind(start_adress);
    return(it != m_JumpTable.constEnd());
}
bool DisassemblerDbModel::isJumpTarget(address_t addr){
    for(QMap<address_t,address_t>::ConstIterator it = m_JumpTable.begin();it!=m_JumpTable.end();it++){
        if(it.value() == addr)
            return true;
    }
    return false;
}

address_t DisassemblerDbModel::targetAdressFromJump(address_t start_adress){

    QMap<address_t,address_t>::ConstIterator it = m_JumpTable.constFind(start_adress);
    return it.value();

}

int DisassemblerDbModel::offsetOfJump(address_t start_adress)
{
     QMap<address_t,int>::ConstIterator JumpDeepPtr = m_JumpDeep.constFind(start_adress);
     return JumpDeepPtr.value();
}

void DisassemblerDbModel::addJump(address_t start_adress, address_t target_adress)
{
    m_Labels.insert(target_adress,"loc_"+QString("%1").arg(target_adress,6,16,QChar('0')).toUpper());
    m_JumpTable.insert(start_adress,target_adress);
    m_JumpDeep.insert(start_adress,0);
}

void DisassemblerDbModel::addComment(address_t addr, QString comment)
{
    m_Comments.insert(addr,comment);
}

void DisassemblerDbModel::removeJumpTo(address_t target)
{
    /*
     * TODO: remove Error
    for(QMap<adress_t,QString>::iterator it = m_Labels.begin(); it != m_Labels.end();it++){
        if(it.key() == target){
            m_Labels.erase(it);
            it = m_Labels.begin();
        }
    }
    */
}

QString DisassemblerDbModel::labelName(address_t loc){
    QMap<address_t,QString>::ConstIterator it = m_Labels.constFind(loc);

    if(it != m_Labels.constEnd())
        return it.value();
    else
        return "";

}

bool DisassemblerDbModel::hasLabel(address_t addr){
    return (m_Labels.find(addr) != m_Labels.end());
}

/**
 * @brief DisassemblerDbModel::setLocationNameAt
 * @param loc           location as string
 * @param name          name as string
 */
void DisassemblerDbModel::setJumpLocationName(address_t loc, QString name){
       // qDebug() << "insert location label at "<< loc;
        m_Labels.insert(loc,name);


}

