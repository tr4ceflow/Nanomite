/*
 * 	This file is part of Nanomite.
 *
 *    Nanomite is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    Nanomite is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Nanomite.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "DisassemblerModel.h"
#include "../cls/clsAPIImport.h"
#include "../cls/clsMemManager.h"
#include "../cls/clsHelperClass.h"
#include "../cls/clsSymbolAndSyntax.h"
#include "../cls/clsPEFile.h"

#include "../mainwindow.h"

#include <string>
#include <TlHelp32.h>
#include <QDebug>
#include <QMessageBox>

#define BEA_ENGINE_STATIC
#define BEA_USE_STDCALL
#include "../BeaEngine/BeaEngine.h"
#include "DisassemblerDbModel.h"

using namespace std;

DisassemblerModel::DisassemblerModel()
{
    dismodel = MainWindow::instance()->m_DisassemblerDbModel;

    _dwStartOffset = 0;_dwEndOffset = 0;
}

DisassemblerModel::~DisassemblerModel()
{

}

/**
 * @brief DisassemblerModel::fullDisassemble disassemble given memory (linear)
 * @param DataPtr
 * @param DataSize
 * @param InstructionOffset
 * @param BaseAddress
 * @param VAOffset
 */
void DisassemblerModel::fullDisassemble(byte_t *DataPtr, quint64 DataSize, quint64 InstructionOffset, address_t BaseAddress, address_t VAOffset)
{

    /*
     * DataPtr  points to the memory of bytes
     * DataSize hold the size of the pointed data
     * InstructionOffset helps to not start in the first byte of the DataPtr
     * BaseAddress is usually 0x401000
     * VAOffset is usually    0x000000
     *
     *
     * this function should perform a static analyse of the given memory range
     */

    int len;
    DISASM BeaEngineStruct;
    // Reset Disasm Structure
    memset(&BeaEngineStruct, 0, sizeof(DISASM));

#ifdef _AMD64_
    BeaEngineStruct.Archi = 64;
#else
    BeaEngineStruct.Archi = 0;
#endif

    BeaEngineStruct.EIP = (UIntPtr)((quint64)DataPtr + (address_t)InstructionOffset);
    BeaEngineStruct.VirtualAddr = BaseAddress + VAOffset;

    /*qDebug() << "full disasm starts at " << QString("%1").arg(BeaEngineStruct.VirtualAddr,8,16,QChar('0'));
    qDebug() << "full disasm ends   at " << QString("%1").arg(BeaEngineStruct.VirtualAddr+DataSize,8,16,QChar('0'));
    qDebug() << "data size             " << DataSize;
    qDebug() << "minstr offset         " << InstructionOffset;
    qDebug() << BeaEngineStruct.VirtualAddr;
    qDebug() << BaseAddress + VAOffset+DataSize;*/

    while(BeaEngineStruct.VirtualAddr <  BaseAddress + VAOffset+DataSize){

        BeaEngineStruct.SecurityBlock = (UIntPtr)((quint64)DataSize - InstructionOffset+BaseAddress);
        len = Disasm(&BeaEngineStruct);
        if (len == OUT_OF_BLOCK) {
            qDebug() << "not allowed to read more memory ";

            break;
        }
        else if (len == UNKNOWN_OPCODE) {
            qDebug() << "unknown opcode";

            break;
        }
        else {
            InstructionOffset += len;
            BeaEngineStruct.EIP += len;
            BeaEngineStruct.VirtualAddr += len;

            MainWindow::instance()->m_DisassemblerDbModel->addInstruction(BeaEngineStruct);

        }
    }
}

bool DisassemblerModel::IsNewInsertNeeded()
{
    int NumberOfInstructionBeforeEIP = 10;
    int NumberOfInstructionAfterEIP = 30;



    qDebug() << "IsNewInsertNeeded()";
    if(_dwEIP <= _dwStartOffset || _dwEIP >= _dwEndOffset || SectionDisAs.count() <= 0)
    {
        _dwStartOffset = _dwEIP - 16 * (NumberOfInstructionBeforeEIP + 3);
        _dwEndOffset = _dwEIP + 16 * (NumberOfInstructionAfterEIP + 3);

        return IsNewInsertPossible();
    }
    return false;
}

bool DisassemblerModel::IsNewInsertPossible()
{

    MODULEENTRY32 pModEntry;
    pModEntry.dwSize = sizeof(MODULEENTRY32);
    MEMORY_BASIC_INFORMATION mbi;


    quint64 dwAddress = NULL;
    while(VirtualQueryEx(_hProc,(LPVOID)dwAddress,&mbi,sizeof(mbi)))
    {
        quint64 dwBaseBegin = (quint64)mbi.BaseAddress,
                dwBaseEnd	= ((quint64)mbi.BaseAddress + mbi.RegionSize);

        if(_dwEIP >= dwBaseBegin && _dwEIP <= dwBaseEnd)
        {
            switch (mbi.State)
            {
            case MEM_COMMIT:
            {
                switch (mbi.Type)
                {
                case MEM_IMAGE:
                case MEM_MAPPED:
                    return true;
                    break;
                }
                break;
            }
            }
        }
        dwAddress += mbi.RegionSize;
    }
    qDebug() << "return false";
    return false;
}

bool DisassemblerModel::InsertNewDisassembly(HANDLE hProc,quint64 dwEIP,bool bClear)
{
    if(_hProc == INVALID_HANDLE_VALUE || dwEIP == NULL)
        return false;

    _dwEIP = dwEIP;
    _hProc = hProc;

    if(bClear) SectionDisAs.clear();

    if(IsNewInsertNeeded())
    {
        SectionDisAs.clear();
        this->start();
        return true;
    }
    else
        return false;
}

/**
 * @brief DisassemblerModel::performFullDisassembly
 * @param _hProc         handle to current process
 *
 * should scan the whole memory of the process to save
 * jump locations and xrefs
 */
void DisassemblerModel::performFullDisassembly(HANDLE _hProc, clsDebugger *_coreDebugger,  clsPEManager *PEM)
{
    QList<PESectionData> Sections = PEM->getSections(_coreDebugger->GetTarget());
    quint64 ModulBase = 0x00400000;
    quint64 dwBaseBegin = ModulBase + Sections.first().VirtualAddress;
    quint64 dwBaseEnd   = ModulBase + Sections.first().VirtualAddress + Sections.first().VirtualSize;


    // calculate size of region
    quint64 dwSize = dwBaseEnd - dwBaseBegin;
    // create coresponding buffer
    LPVOID pBuffer = malloc(dwSize);



    // read the specific memory
    if(ReadProcessMemory(_hProc,(LPVOID)dwBaseBegin,pBuffer,dwSize,NULL))
    {
        // BeaEngin returns a struct "DISASM"
        DISASM newDisAss;
        // flag
        bool bContinueDisAs = true;
        // length of instruction
        int iLen = 0;

        memset(&newDisAss, 0, sizeof(DISASM));

        // set information for BeaEngine
        newDisAss.EIP = (quint64)pBuffer;
        newDisAss.VirtualAddr = dwBaseBegin ;
#ifdef _AMD64_
        newDisAss.Archi = 64;
#else
        newDisAss.Archi = 0;
#endif

        PTCHAR sTemp = (PTCHAR)clsMemManager::CAlloc(MAX_PATH * sizeof(TCHAR));
        // disassembling loop
        while(bContinueDisAs)
        {
            // BeaEngine just stays within the SecurityBlock and ignores everything
            // thats not in the block
            newDisAss.SecurityBlock = (int)dwBaseEnd;

            // disassemble bytes
            iLen = Disasm(&newDisAss);

            if (iLen == OUT_OF_BLOCK)
                bContinueDisAs = false;

            else if(iLen != UNKNOWN_OPCODE)
            {
                memset(sTemp,0,MAX_PATH *  sizeof(TCHAR));
                int iTempLen = ((iLen == UNKNOWN_OPCODE) ? 0 : ((newDisAss.Instruction.Opcode == 0x00 && iLen == 2) ? 1 : iLen));
                if(iTempLen > 0){
                    dismodel->analyzeInstruction(newDisAss);


                }

            }
            // go to next instruction
            newDisAss.EIP += ((iLen == UNKNOWN_OPCODE) ? 1 : ((newDisAss.Instruction.Opcode == 0x00 && iLen == 2) ? iLen -= 1 : iLen));
            newDisAss.VirtualAddr += ((iLen == UNKNOWN_OPCODE) ? 1 : ((newDisAss.Instruction.Opcode == 0x00 && iLen == 2) ? iLen -= 1 : iLen));

            // out of range?
            if (newDisAss.VirtualAddr >= dwBaseEnd)
                bContinueDisAs = false;
        }
        clsMemManager::CFree(sTemp);
    }
    else
    {
        free(pBuffer);
        // Can not disassemble this buffer
        return;
    }

    free(pBuffer);
    dismodel->createJumpInformation();

}

void DisassemblerModel::run()
{
    qDebug() << "disassembler started";

    if(_dwStartOffset == 0 || _dwEndOffset == 0)
        return;

    _dwStartOffset = (_dwStartOffset < 0) ? 0 : _dwStartOffset;


    quint64 dwSize = _dwEndOffset - _dwStartOffset;
    LPVOID pBuffer = malloc(dwSize);

    if(ReadProcessMemory(_hProc,(LPVOID)_dwStartOffset,pBuffer,dwSize,NULL))
    {
        DISASM newDisAss;
        bool bContinueDisAs = true;
        int iLen = 0;

        memset(&newDisAss, 0, sizeof(DISASM));

        newDisAss.EIP = (quint64)pBuffer;
        newDisAss.VirtualAddr = _dwStartOffset;
#ifdef _AMD64_
        newDisAss.Archi = 64;
#else
        newDisAss.Archi = 0;
#endif

        PTCHAR sTemp = (PTCHAR)clsMemManager::CAlloc(MAX_PATH * sizeof(TCHAR));
        while(bContinueDisAs)
        {
            newDisAss.SecurityBlock = (int)_dwEndOffset - newDisAss.VirtualAddr;

            iLen = Disasm(&newDisAss);
            if (iLen == OUT_OF_BLOCK)
                bContinueDisAs = false;
            else if(iLen != UNKNOWN_OPCODE)
            {
                memset(sTemp,0,MAX_PATH *  sizeof(TCHAR));
                int iTempLen = ((iLen == UNKNOWN_OPCODE) ? 0 : ((newDisAss.Instruction.Opcode == 0x00 && iLen == 2) ? 1 : iLen));
                if(iTempLen > 0)
                    SectionDisAs.insert(newDisAss.VirtualAddr,newDisAss);
            }

            newDisAss.EIP += ((iLen == UNKNOWN_OPCODE) ? 1 : ((newDisAss.Instruction.Opcode == 0x00 && iLen == 2) ? iLen -= 1 : iLen));
            newDisAss.VirtualAddr += ((iLen == UNKNOWN_OPCODE) ? 1 : ((newDisAss.Instruction.Opcode == 0x00 && iLen == 2) ? iLen -= 1 : iLen));

            if (newDisAss.VirtualAddr >= _dwEndOffset)
                bContinueDisAs = false;
        }
        clsMemManager::CFree(sTemp);
    }
    else
    {
        free(pBuffer);
        MessageBox(NULL,L"Access Denied! Can not disassemble this buffer :(",L"Nanomite",MB_OK);
        return;
    }

    free(pBuffer);

    if(SectionDisAs.count() > 0)
    {
        QMap<quint64,DISASM>::iterator iEnd = SectionDisAs.end();iEnd--;
        _dwEndOffset = iEnd.key();
    }
    else
        _dwEndOffset = 0;

    emit DisAsFinished(_dwEIP,&SectionDisAs);
}
