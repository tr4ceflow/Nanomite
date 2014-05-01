
#include "mainwindow.h"

#include <QMessageBox>
#include <QFileDialog>

/**
 * @brief MainWindow::execOpenAction
 * opens a new file
 */
void MainWindow::execOpenAction(){

    // aks for filename of "exe" that should be debugged
    QString fileName = QFileDialog::getOpenFileName(NULL,"Please select a Target",QDir::currentPath(),"Executables (*.exe)");

    if(!fileName.isEmpty()){

        // clear all breakpoints
        m_Debugger->RemoveBPs();
        m_Debugger->ClearTarget();
        m_Debugger->ClearCommandLine();
        // set exe from above to debugger target
        m_Debugger->SetTarget(fileName.toStdWString());
        // debugger should stop at program entry point!
        m_Debugger->dbgSettings.bBreakOnModuleEP = true;


        if(!m_PeManager->OpenFile(m_Debugger->GetTarget()) ||
                !m_PeManager->isValidPEFile(m_Debugger->GetTarget()))
        {
            MessageBoxW(NULL,L"This is a invalid File! Please select another one!",L"Nanomite",MB_OK);
            m_PeManager->CloseFile(m_Debugger->GetTarget(),0);
            m_Debugger->ClearTarget();
            return;
        }

        // open database (new or existing)
        m_DisassemblerDbModel->openNewFile(fileName);


    }



    // start debugging process
    m_Debugger->start();

    emit On_execOpenAction();


}
