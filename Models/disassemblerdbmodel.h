#ifndef DISASSEMBLERDBMODEL_H
#define DISASSEMBLERDBMODEL_H

#include "../Global.h"
#define BEA_ENGINE_STATIC
#define BEA_USE_STDCALL
#include "../BeaEngine/BeaEngine.h"

#include <QWidget>
#include <QSql>
#include <QSqlDatabase>
#include <QString>
#include <QMap>

/**
 * @brief The DisassemblerDbModel class
 *
 * stores all additional informations like
 * - comments
 * - labels
 * - jump-maps
 * - call-maps
 */
class DisassemblerDbModel : public QWidget
{
private:
     QSqlDatabase m_DB;
     QString m_TargetFilename;
     QString m_DatabaseFilename;

     QMap<address_t,DISASM> m_StaticDisassembly;

     QMap<address_t,QString> m_Labels;       // maps <target, name>
     QList<address_t> m_Functions;
     QMap<address_t,address_t> m_JumpTable;   // maps <start, target>
     QMap<address_t,int> m_JumpDeep;         // maps <start, deep>  for drawing

     QMap<address_t,QString> m_Comments;       // maps <target, name>




public:
    DisassemblerDbModel(QWidget * parent=0);

    void openNewFile(QString Filename);


    QList<address_t> functionList();
    QString labelName(address_t loc);
    QString comment(address_t addr);
    void setJumpLocationName(address_t loc, QString name);

    void createTables();
    void createJumpInformation();


    bool hasLabel(address_t addr);
    bool hasComment(address_t addr);

    void addCall(address_t start_adress,address_t target);
    void addJump(address_t start_adress,address_t target);
    void addComment(address_t addr, QString comment);

    void removeJumpTo(address_t target);
    bool existsJumpFrom(address_t start_adress);
    address_t targetAdressFromJump(address_t start_adress);
    int offsetOfJump(address_t start_adress);

    bool isJumpTarget(address_t addr);
    void analyzeInstruction(DISASM Instruction);
    void addInstruction(DISASM Instruction);
};

#endif // DISASSEMBLERDBMODEL_H
