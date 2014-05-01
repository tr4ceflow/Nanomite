
#include "mainwindow.h"

#include <QMessageBox>
#include <QDebug>


void MainWindow::onNewTargetLoaded(){
    m_ProgramSegmentationView->InsertSections(m_Debugger->GetTarget());
    m_ImportsView->InsertImports(m_Debugger->GetTarget());

    m_DisassemblerView->setPESectionData(m_PeManager->getSections(m_Debugger->GetTarget()));
    qDebug() << "onNewTargetLoaded";
   ;
}

/**
 * @brief MainWindow::onSlotDebuggerBreak
 * get fired, when the debugger reaches a breakpoint
 */
void MainWindow::onSlotDebuggerBreak()
{

    if(m_Debugger->GetDebuggingState()){

        // retrieve register states
        QList<quint32> registers;
        registers << m_Debugger->ProcessContext.Eax
                  << m_Debugger->ProcessContext.Ebx
                  << m_Debugger->ProcessContext.Ecx
                  << m_Debugger->ProcessContext.Edx
                  << m_Debugger->ProcessContext.Esi
                  << m_Debugger->ProcessContext.Edi
                  << m_Debugger->ProcessContext.Ebp
                  << m_Debugger->ProcessContext.Esp
                  << m_Debugger->ProcessContext.Eip
                  << m_Debugger->ProcessContext.EFlags;

        m_RegisterView->setNewRegisterValues(registers);

        quint64 dwEIP =m_Debugger->ProcessContext.Eip;

        if(NewFile){
            // peform fuldisassembly
            m_DisassemblerModel->performFullDisassembly(m_Debugger->GetCurrentProcessHandle(),m_Debugger ,m_PeManager);
            NewFile=false;
            m_FunctionView->insertFunctions();
        }

        // Update Disassembler
        if(!m_DisassemblerModel->InsertNewDisassembly(m_Debugger->GetCurrentProcessHandle(),dwEIP))
        {
            m_DisassemblerView->displayDisassembly(dwEIP,&m_DisassemblerModel->SectionDisAs);
        }

    }


}


void MainWindow::onDisplayDisassembly(quint64 dwEIP, QMap<quint64,DISASM> *Data){
    m_DisassemblerView->displayDisassembly(dwEIP,Data);
}


void MainWindow::onMainTabChange(int Tab){
    if(Tab==0){
        m_DisassemblerView->repaint();
    }
}
