#include "qtDLGMemoryView.h"
#include "qtDLGNanomite.h"

#include <Psapi.h>
#include <TlHelp32.h>

qtDLGMemoryView::qtDLGMemoryView(QWidget *parent, Qt::WFlags flags,qint32 iPID)
	: QWidget(parent, flags)
{
	setupUi(this);
	this->setAttribute(Qt::WA_DeleteOnClose,true);
	_iPID = iPID;
	this->setFixedSize(this->width(),this->height());

	// Init List
	tblMemoryView->horizontalHeader()->resizeSection(0,75);
	tblMemoryView->horizontalHeader()->resizeSection(1,135);
	tblMemoryView->horizontalHeader()->resizeSection(2,135);
	tblMemoryView->horizontalHeader()->resizeSection(3,150);
	tblMemoryView->horizontalHeader()->resizeSection(4,135);

	// Display
	qtDLGNanomite *myMainWindow = qtDLGNanomite::GetInstance();

	int iForEntry = 0;
	int iForEnd = myMainWindow->coreDebugger->PIDs.size();

	for(int i = 0; i < myMainWindow->coreDebugger->PIDs.size(); i++)
	{
		if(myMainWindow->coreDebugger->PIDs[i].dwPID == _iPID)
			iForEntry = i; iForEnd = i +1;
	}


	PTCHAR sTemp = (PTCHAR)malloc(MAX_PATH * sizeof(TCHAR));
	PTCHAR sTemp2 = (PTCHAR)malloc(MAX_PATH * sizeof(TCHAR));

	MODULEENTRY32 pModEntry;
	pModEntry.dwSize = sizeof(MODULEENTRY32);
	MEMORY_BASIC_INFORMATION mbi;

	for(int i = iForEntry; i < iForEnd;i++)
	{
		DWORD64 dwAddress = NULL;
		while(VirtualQueryEx(myMainWindow->coreDebugger->PIDs[i].hProc,(LPVOID)dwAddress,&mbi,sizeof(mbi)))
		{
			tblMemoryView->insertRow(tblMemoryView->rowCount());

			// PID
			tblMemoryView->setItem(tblMemoryView->rowCount() -1,0,
				new QTableWidgetItem(QString().sprintf("%08X",myMainWindow->coreDebugger->PIDs[i].dwPID)));
			

			// Base Address
#ifdef _AMD64_
			wsprintf(sTemp,L"0x%016I64X",mbi.BaseAddress);
#else
			wsprintf(sTemp,L"0x%016X",mbi.BaseAddress);
#endif
			tblMemoryView->setItem(tblMemoryView->rowCount() -1,1,
				new QTableWidgetItem(QString().fromStdWString(sTemp)));

			// Size
			wsprintf(sTemp,L"0x%08X",mbi.RegionSize);
			tblMemoryView->setItem(tblMemoryView->rowCount() -1,2,
				new QTableWidgetItem(QString().fromStdWString(sTemp)));

			// Path
			int iModPos = NULL,
				iModLen = NULL;

			memset(sTemp,0,MAX_PATH * sizeof(TCHAR));
			memset(sTemp2,0,MAX_PATH * sizeof(TCHAR));
			GetMappedFileName(myMainWindow->coreDebugger->PIDs[i].hProc,(LPVOID)dwAddress,sTemp2,MAX_PATH * sizeof(TCHAR));

			iModLen = wcslen(sTemp2);
			if(iModLen > 0)
			{
				for(int i = iModLen; i > 0 ; i--)
				{
					if(sTemp2[i] == '\\')
					{
						iModPos = i;
						break;
					}
				}
						
				memcpy(sTemp,(LPVOID)&sTemp2[iModPos + 1],(iModLen - iModPos) * sizeof(TCHAR));

				tblMemoryView->setItem(tblMemoryView->rowCount() -1,3,
					new QTableWidgetItem(QString().fromStdWString(sTemp)));			
			}

			// Mem Type
			switch (mbi.State)
			{
			case MEM_FREE:			wsprintf(sTemp,L"%s",L"Free");		break;
			case MEM_RESERVE:       wsprintf(sTemp,L"%s",L"Reserve");	break;
			case MEM_COMMIT:
				switch (mbi.Type)
				{
				case MEM_FREE:		wsprintf(sTemp,L"%s",L"Free");     break;
				case MEM_RESERVE:   wsprintf(sTemp,L"%s",L"Reserve");  break;
				case MEM_IMAGE:		wsprintf(sTemp,L"%s",L"Image");    break;
				case MEM_MAPPED:    wsprintf(sTemp,L"%s",L"Mapped");   break;
				case MEM_PRIVATE:   wsprintf(sTemp,L"%s",L"Private");  break;
				}
				break;
			}
			tblMemoryView->setItem(tblMemoryView->rowCount() -1,4,
				new QTableWidgetItem(QString().fromStdWString(sTemp)));

			// Access
			wsprintf(sTemp,L"%s",L"Unknown");
			if(mbi.State == MEM_FREE) mbi.Protect = PAGE_NOACCESS;
			if(mbi.State == MEM_RESERVE) mbi.Protect = mbi.AllocationProtect;
			switch (mbi.Protect & ~(PAGE_GUARD | PAGE_NOCACHE | PAGE_WRITECOMBINE))
			{
			case PAGE_READONLY:          wsprintf(sTemp,L"%s",L"-R--"); break;
			case PAGE_READWRITE:         wsprintf(sTemp,L"%s",L"-RW-"); break;
			case PAGE_WRITECOPY:         wsprintf(sTemp,L"%s",L"-RWC"); break;
			case PAGE_EXECUTE:           wsprintf(sTemp,L"%s",L"E---"); break;
			case PAGE_EXECUTE_READ:      wsprintf(sTemp,L"%s",L"ER--"); break;
			case PAGE_EXECUTE_READWRITE: wsprintf(sTemp,L"%s",L"ERW-"); break;
			case PAGE_EXECUTE_WRITECOPY: wsprintf(sTemp,L"%s",L"ERWC"); break; 
			case PAGE_NOACCESS:          wsprintf(sTemp,L"%s",L"----"); break;
			}
			tblMemoryView->setItem(tblMemoryView->rowCount() -1,5,
				new QTableWidgetItem(QString().fromStdWString(sTemp)));

			dwAddress += mbi.RegionSize;
		}
	}
	free(sTemp2);
	free(sTemp);
}

qtDLGMemoryView::~qtDLGMemoryView()
{

}