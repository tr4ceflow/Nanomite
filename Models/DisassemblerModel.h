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
#ifndef CLSDISAS_H
#define CLSDISAS_H
#define NOMINMAX
#include <Windows.h>

#include <QtCore>
#include <QtGui>
#include <QByteArray>

#include "../Models/DisassemblerDbModel.h"
#include "../cls/clsPEFile.h"
#include "../cls/clsPEManager.h"
#include "../clsDebugger/clsDebugger.h"

#define BEA_ENGINE_STATIC
#define BEA_USE_STDCALL
#include "../BeaEngine/BeaEngine.h"

typedef unsigned char byte_t;

class DisassemblerModel : public QThread
{
    Q_OBJECT

public:
    DisassemblerDbModel *dismodel;



    QMap<quint64,DISASM> SectionDisAs;
    DisassemblerModel();
    ~DisassemblerModel();


    void fullDisassemble(byte_t* DataPtr, quint64 DataSize, quint64 InstructionOffset, address_t BaseAddress,
                         address_t origInstRVA);


    bool InsertNewDisassembly(HANDLE hProc,quint64 dwEIP,bool bClear = false);
    void performFullDisassembly(HANDLE hProc, clsDebugger *_coreDebugger, clsPEManager *PEM);
signals:
    void DisAsFinished(quint64 dwEIP,QMap<quint64,DISASM>* data);

private:
    HANDLE	_hProc;
    quint64 _dwEIP,
            _dwStartOffset,
            _dwEndOffset;

    bool IsNewInsertNeeded();
    bool IsNewInsertPossible();



protected:
    void run();
};

#endif
