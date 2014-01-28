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
#include "qtDLGPEEditor.h"
#include "qtDLGNanomite.h"

#include "clsMemManager.h"
#include "clsPEManager.h"
#include "clsHelperClass.h"

qtDLGPEEditor::qtDLGPEEditor(clsPEManager *PEManager,QWidget *parent, Qt::WFlags flags, int PID, QString FileName, DWORD64 imageBase)
	: QWidget(parent,flags)
{
	setupUi(this);
	
	this->setStyleSheet(clsHelperClass::LoadStyleSheet());
	this->setLayout(verticalLayout);
	this->setAttribute(Qt::WA_DeleteOnClose,true);

	connect(treePE,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(OnCustomContextMenu(QPoint)));
	connect(new QShortcut(Qt::Key_Escape,this),SIGNAL(activated()),this,SLOT(close()));

	m_processID = PID;
	m_pEManager = PEManager;
	
	if(m_pEManager != NULL)
	{	
		if(FileName.length() > 0)
			m_currentFile = FileName;
		else
			m_currentFile = m_pEManager->getFilenameFromPID(m_processID);

		if(m_currentFile.length() <= 0) 
		{
			QMessageBox::critical(this,"Nanomite","Could not load File!",QMessageBox::Ok,QMessageBox::Ok);

			close();
		}
		this->setWindowTitle(QString("[Nanomite] - PEEditor - FileName: %1").arg(m_currentFile));

		m_pEManager->OpenFile(m_currentFile, m_processID, imageBase);
		
		InitList();
		LoadPEView();
	}
	else
	{
		QMessageBox::critical(this,"Nanomite","Could not load File!",QMessageBox::Ok,QMessageBox::Ok);

		close();
	}
}

qtDLGPEEditor::~qtDLGPEEditor()
{
	if(m_processID == -1)
		m_pEManager->CloseFile(m_currentFile,-1);
}

void qtDLGPEEditor::InitList()
{
	treePE->header()->resizeSection(0,450);
	treePE->header()->resizeSection(1,120);
	treePE->header()->resizeSection(2,120);
	treePE->header()->resizeSection(3,120);
	treePE->header()->resizeSection(4,120);
	treePE->header()->resizeSection(5,120);
}

void qtDLGPEEditor::LoadPEView()
{
	InsertDosHeader();
	InsertFileHeader();
	InsertOptionalHeader();
	InsertImports();
	InsertExports();
	InsertResources();
	InsertSections();	
}

//FIXME: rewrite for recursive variant. it will be more beautiful
void qtDLGPEEditor::InsertResources()
{
	SResourceDirectory rscrDir =	m_pEManager->getResourceDirectory(m_currentFile), 
									rscrDir2, 
									rscrDir3, 
									rscrDir4;

	SResourceDirectoryEntry rscrDirEntry, 
							rscrDirEntry2, 
							rscrDirEntry3, 
							rscrDirEntry4;

	QTreeWidgetItem *topElement, 
					*element, 
					*element2, 
					*element3, 
					*element4, 
					*element5, 
					*element6;

	topElement = new QTreeWidgetItem;
	topElement->setText(0, "Resource Directory");
	treePE->addTopLevelItem(topElement);

	InsertImageRsrcDirInfo(topElement, "Characteristics", rscrDir.m_dir.Characteristics);
	InsertImageRsrcDirInfo(topElement, "Major version", rscrDir.m_dir.MajorVersion);
	InsertImageRsrcDirInfo(topElement, "Minor version", rscrDir.m_dir.MinorVersion);
	InsertImageRsrcDirInfo(topElement, "Number of id entries", rscrDir.m_dir.NumberOfIdEntries);
	InsertImageRsrcDirInfo(topElement, "Number of named entries", rscrDir.m_dir.NumberOfNamedEntries);
	InsertImageRsrcDirInfo(topElement, "Time date stamp", rscrDir.m_dir.TimeDateStamp);

	QString tmpStr;

	for (int i = 0; i < rscrDir.m_directoryEntries.size(); i++) {
		rscrDirEntry = rscrDir.m_directoryEntries.at(i);
		element = new QTreeWidgetItem(topElement);
		
		if (rscrDirEntry.m_resourceName.isEmpty() == false) {
			tmpStr = rscrDirEntry.m_resourceName;
		} else {
			tmpStr = QString("%1").arg(rscrDirEntry.m_directoryEntry.Id);
		}

		element->setText(0, QString("Resource Directory Entry %1, ID: %2")
			.arg(i + 1)
			.arg(tmpStr)); 

		InsertImageRsrcDirInfo(element, "Name", rscrDirEntry.m_directoryEntry.Name);
		InsertImageRsrcDirInfo(element, "OffsetToData", rscrDirEntry.m_directoryEntry.OffsetToData);

		element2 = new QTreeWidgetItem(element);
		element2->setText(0, QString("Resource Directory"));
		
		for (int j = 0; j < rscrDirEntry.m_sDir.m_directoryEntries.size(); j++) {
			rscrDirEntry2 = rscrDirEntry.m_sDir.m_directoryEntries.at(j);
			rscrDir2 = rscrDirEntry.m_sDir;

			if (j == 0) {
				InsertImageRsrcDirInfo(element2, "Characteristics", rscrDir2.m_dir.Characteristics);
				InsertImageRsrcDirInfo(element2, "Major version", rscrDir2.m_dir.MajorVersion);
				InsertImageRsrcDirInfo(element2, "Minor version", rscrDir2.m_dir.MinorVersion);
				InsertImageRsrcDirInfo(element2, "Number of id entries", rscrDir2.m_dir.NumberOfIdEntries);
				InsertImageRsrcDirInfo(element2, "Number of named entries", rscrDir2.m_dir.NumberOfNamedEntries);
				InsertImageRsrcDirInfo(element2, "Time date stamp", rscrDir2.m_dir.TimeDateStamp);
			}
			
			element3 = new QTreeWidgetItem(element2);

			if (rscrDirEntry2.m_resourceName.isEmpty() == false) {
				tmpStr = rscrDirEntry2.m_resourceName;
			} else {
				tmpStr = QString("%1").arg(rscrDirEntry2.m_directoryEntry.Id);
			}

			element3->setText(0, QString("Resource Directory Entry %1, ID: %2")
				.arg(j + 1)
				.arg(tmpStr)); 

			InsertImageRsrcDirInfo(element3, "Name", rscrDirEntry2.m_directoryEntry.Name);
			InsertImageRsrcDirInfo(element3, "OffsetToData", rscrDirEntry2.m_directoryEntry.OffsetToData);

			element4 = new QTreeWidgetItem(element3);
			element4->setText(0, QString("Resource Directory"));
			for (int k = 0; k < rscrDirEntry2.m_sDir.m_directoryEntries.size(); k++) {
				rscrDirEntry3 = rscrDirEntry2.m_sDir.m_directoryEntries.at(k);
				rscrDir3 = rscrDirEntry2.m_sDir;

				if (k == 0) {
					InsertImageRsrcDirInfo(element4, "Characteristics", rscrDir3.m_dir.Characteristics);
					InsertImageRsrcDirInfo(element4, "Major version", rscrDir3.m_dir.MajorVersion);
					InsertImageRsrcDirInfo(element4, "Minor version", rscrDir3.m_dir.MinorVersion);
					InsertImageRsrcDirInfo(element4, "Number of id entries", rscrDir3.m_dir.NumberOfIdEntries);
					InsertImageRsrcDirInfo(element4, "Number of named entries", rscrDir3.m_dir.NumberOfNamedEntries);
					InsertImageRsrcDirInfo(element4, "Time date stamp", rscrDir3.m_dir.TimeDateStamp);
				}
				
				element5 = new QTreeWidgetItem(element4);

				if (rscrDirEntry3.m_resourceName.isEmpty() == false) {
					tmpStr = rscrDirEntry3.m_resourceName;
				} else {
					tmpStr = QString("%1").arg(rscrDirEntry3.m_directoryEntry.Id);
				}

				element5->setText(0, QString("Resource Directory Entry %1, ID: %2")
					.arg(k + 1)
					.arg(tmpStr)); 

				InsertImageRsrcDirInfo(element5, "Name", rscrDirEntry3.m_directoryEntry.Name);
				InsertImageRsrcDirInfo(element5, "OffsetToData", rscrDirEntry3.m_directoryEntry.OffsetToData);
				
				element6 = new QTreeWidgetItem(element5);
				element6->setText(0, QString("Resource Data Entry"));

				InsertImageRsrcDirInfo(element6, "OffsetToData", rscrDirEntry3.m_dataEntry.OffsetToData);
				InsertImageRsrcDirInfo(element6, "Size", rscrDirEntry3.m_dataEntry.Size);
				InsertImageRsrcDirInfo(element6, "CodePage", rscrDirEntry3.m_dataEntry.CodePage);
				InsertImageRsrcDirInfo(element6, "Reserved", rscrDirEntry3.m_dataEntry.Reserved);
			}
		}
	}
}

void qtDLGPEEditor::InsertImageRsrcDirInfo(QTreeWidgetItem *topElement, QString ValueName, quint64 dwValue)
{
	QTreeWidgetItem *dataElement = new QTreeWidgetItem(topElement);
	dataElement->setText(0, ValueName);
	dataElement->setText(1, QString("%1").arg(dwValue, 8, 16, QChar('0')));
}

void qtDLGPEEditor::InsertImports()
{
	QList<APIData> imports = m_pEManager->getImports(m_currentFile);
	if(imports.size() <= 0) return;

	QTreeWidgetItem *topElement,
					*moduleElement;
	QString lastTopElement;
	//DWORD64 moduleIB = NULL;

	topElement = new QTreeWidgetItem();
	topElement->setText(0,"Imports");
	treePE->addTopLevelItem(topElement);

	for(int importCount = 0; importCount < imports.size(); importCount++)
	{
		QStringList currentElement = imports.value(importCount).APIName.split("::");

		if(currentElement[0].compare(lastTopElement) != NULL)
		{	
			moduleElement = new QTreeWidgetItem(topElement);
			moduleElement->setText(0,currentElement[0]);  
			lastTopElement = currentElement[0];
			//moduleIB = clsHelperClass::CalcOffsetForModule((PTCHAR)currentElement[0].toStdWString().c_str(), NULL, m_processID);
		}
		
		QTreeWidgetItem* childElement = new QTreeWidgetItem(moduleElement);
		childElement->setText(0,currentElement[1]);
		childElement->setText(1,QString("%1").arg(imports.value(importCount).APIOffset/* + moduleIB*/, 16, 16, QChar('0')));
	}
}

void qtDLGPEEditor::InsertExports()
{
	QList<APIData> exports = m_pEManager->getExports(m_currentFile);
	if(exports.size() <= 0) return;

	QTreeWidgetItem *topElement,
					*exportElement;

	topElement = new QTreeWidgetItem();
	topElement->setText(0,"Exports");
	treePE->addTopLevelItem(topElement);	

	for(int exportsCount = 0; exportsCount < exports.size(); exportsCount++)
	{		
		exportElement = new QTreeWidgetItem(topElement);
		exportElement->setText(0,exports.at(exportsCount).APIName);
		exportElement->setText(1,QString("%1").arg(exports.value(exportsCount).APIOffset,16,16,QChar('0')));  
	}
}

void qtDLGPEEditor::InsertDosHeader()
{
	IMAGE_DOS_HEADER currentDOS = m_pEManager->getDosHeader(m_currentFile);
	QTreeWidgetItem *topElement;

	topElement = new QTreeWidgetItem();
	topElement->setText(0,"IMAGE_DOS_HEADER");
	treePE->addTopLevelItem(topElement);	

	InsertHeaderData(topElement,"e_cblp",currentDOS.e_cblp);
	InsertHeaderData(topElement,"e_cp",currentDOS.e_cp);
	InsertHeaderData(topElement,"e_cparhdr",currentDOS.e_cparhdr);
	InsertHeaderData(topElement,"e_crlc",currentDOS.e_crlc);
	InsertHeaderData(topElement,"e_cs",currentDOS.e_cs);
	InsertHeaderData(topElement,"e_csum",currentDOS.e_csum);
	InsertHeaderData(topElement,"e_ip",currentDOS.e_ip);
	InsertHeaderData(topElement,"e_lfanew",currentDOS.e_lfanew);
	InsertHeaderData(topElement,"e_lfarlc",currentDOS.e_lfarlc);
	InsertHeaderData(topElement,"e_magic",currentDOS.e_magic);
	InsertHeaderData(topElement,"e_maxalloc",currentDOS.e_maxalloc);
	InsertHeaderData(topElement,"e_minalloc",currentDOS.e_minalloc);
	InsertHeaderData(topElement,"e_oemid",currentDOS.e_oemid);
	InsertHeaderData(topElement,"e_oeminfo",currentDOS.e_oeminfo);
	InsertHeaderData(topElement,"e_ovno",currentDOS.e_ovno);
	InsertHeaderData(topElement,"e_res",(quint64)currentDOS.e_res);
	InsertHeaderData(topElement,"e_res2",(quint64)currentDOS.e_res2);
	InsertHeaderData(topElement,"e_sp",currentDOS.e_sp);
	InsertHeaderData(topElement,"e_ss",currentDOS.e_ss);
}

void qtDLGPEEditor::InsertFileHeader()
{
	QTreeWidgetItem *topElement;
	topElement = new QTreeWidgetItem();
	topElement->setText(0,"IMAGE_FILE_HEADER");
		
	if(m_pEManager->is64BitFile(m_currentFile))
	{
		IMAGE_NT_HEADERS64 currentFileHeader = m_pEManager->getNTHeader64(m_currentFile);
	
		treePE->addTopLevelItem(topElement);
		InsertHeaderData(topElement,"Characteristics",currentFileHeader.FileHeader.Characteristics);
		InsertHeaderData(topElement,"Machine",currentFileHeader.FileHeader.Machine);
		InsertHeaderData(topElement,"NumberOfSections",currentFileHeader.FileHeader.NumberOfSections);
		InsertHeaderData(topElement,"NumberOfSymbols",currentFileHeader.FileHeader.NumberOfSymbols);
		InsertHeaderData(topElement,"PointerToSymbolTable",currentFileHeader.FileHeader.PointerToSymbolTable);
		InsertHeaderData(topElement,"SizeOfOptionalHeader",currentFileHeader.FileHeader.SizeOfOptionalHeader);
		InsertHeaderData(topElement,"TimeDateStamp",currentFileHeader.FileHeader.TimeDateStamp);
	}
	else
	{
		IMAGE_NT_HEADERS32 currentFileHeader = m_pEManager->getNTHeader32(m_currentFile);

		treePE->addTopLevelItem(topElement);
		InsertHeaderData(topElement,"Characteristics",currentFileHeader.FileHeader.Characteristics);
		InsertHeaderData(topElement,"Machine",currentFileHeader.FileHeader.Machine);
		InsertHeaderData(topElement,"NumberOfSections",currentFileHeader.FileHeader.NumberOfSections);
		InsertHeaderData(topElement,"NumberOfSymbols",currentFileHeader.FileHeader.NumberOfSymbols);
		InsertHeaderData(topElement,"PointerToSymbolTable",currentFileHeader.FileHeader.PointerToSymbolTable);
		InsertHeaderData(topElement,"SizeOfOptionalHeader",currentFileHeader.FileHeader.SizeOfOptionalHeader);
		InsertHeaderData(topElement,"TimeDateStamp",currentFileHeader.FileHeader.TimeDateStamp);
	}
}

void qtDLGPEEditor::InsertOptionalHeader()
{
	QTreeWidgetItem *topElement;
	topElement = new QTreeWidgetItem();
	topElement->setText(0,"IMAGE_OPTIONAL_HEADER");

	if(m_pEManager->is64BitFile(m_currentFile))
	{
		IMAGE_NT_HEADERS64 currentFileHeader = m_pEManager->getNTHeader64(m_currentFile);

		treePE->addTopLevelItem(topElement);
		InsertHeaderData(topElement,"AddressOfEntryPoint",currentFileHeader.OptionalHeader.AddressOfEntryPoint);
		InsertHeaderData(topElement,"BaseOfCode",currentFileHeader.OptionalHeader.BaseOfCode);
		InsertHeaderData(topElement,"CheckSum",currentFileHeader.OptionalHeader.CheckSum);
		//InsertHeaderData(topElement,"DataDirectory",currentFileHeader.OptionalHeader.DataDirectory);
		InsertHeaderData(topElement,"DllCharacteristics",currentFileHeader.OptionalHeader.DllCharacteristics);
		InsertHeaderData(topElement,"FileAlignment",currentFileHeader.OptionalHeader.FileAlignment);
		InsertHeaderData(topElement,"ImageBase",currentFileHeader.OptionalHeader.ImageBase);
		InsertHeaderData(topElement,"LoaderFlags",currentFileHeader.OptionalHeader.LoaderFlags);
		InsertHeaderData(topElement,"Magic",currentFileHeader.OptionalHeader.Magic);
		InsertHeaderData(topElement,"MajorImageVersion",currentFileHeader.OptionalHeader.MajorImageVersion);
		InsertHeaderData(topElement,"MajorLinkerVersion",currentFileHeader.OptionalHeader.MajorLinkerVersion);
		InsertHeaderData(topElement,"MajorOperatingSystemVersion",currentFileHeader.OptionalHeader.MajorOperatingSystemVersion);
		InsertHeaderData(topElement,"MajorSubsystemVersion",currentFileHeader.OptionalHeader.MajorSubsystemVersion);
		InsertHeaderData(topElement,"MinorImageVersion",currentFileHeader.OptionalHeader.MinorImageVersion);
		InsertHeaderData(topElement,"MinorLinkerVersion",currentFileHeader.OptionalHeader.MinorLinkerVersion);
		InsertHeaderData(topElement,"MinorOperatingSystemVersion",currentFileHeader.OptionalHeader.MinorOperatingSystemVersion);
		InsertHeaderData(topElement,"MinorSubsystemVersion",currentFileHeader.OptionalHeader.MinorSubsystemVersion);
		InsertHeaderData(topElement,"NumberOfRvaAndSizes",currentFileHeader.OptionalHeader.NumberOfRvaAndSizes);
		InsertHeaderData(topElement,"SectionAlignment",currentFileHeader.OptionalHeader.SectionAlignment);
		InsertHeaderData(topElement,"SizeOfCode",currentFileHeader.OptionalHeader.SizeOfCode);
		InsertHeaderData(topElement,"SizeOfHeaders",currentFileHeader.OptionalHeader.SizeOfHeaders);
		InsertHeaderData(topElement,"SizeOfHeapCommit",currentFileHeader.OptionalHeader.SizeOfHeapCommit);
		InsertHeaderData(topElement,"SizeOfHeapReserve",currentFileHeader.OptionalHeader.SizeOfHeapReserve);
		InsertHeaderData(topElement,"SizeOfImage",currentFileHeader.OptionalHeader.SizeOfImage);
		InsertHeaderData(topElement,"SizeOfInitializedData",currentFileHeader.OptionalHeader.SizeOfInitializedData);
		InsertHeaderData(topElement,"SizeOfStackCommit",currentFileHeader.OptionalHeader.SizeOfStackCommit);
		InsertHeaderData(topElement,"SizeOfStackReserve",currentFileHeader.OptionalHeader.SizeOfStackReserve);
		InsertHeaderData(topElement,"SizeOfUninitializedData",currentFileHeader.OptionalHeader.SizeOfUninitializedData);
		InsertHeaderData(topElement,"Subsystem",currentFileHeader.OptionalHeader.Subsystem);
		InsertHeaderData(topElement,"Win32VersionValue",currentFileHeader.OptionalHeader.Win32VersionValue);
	}
	else
	{
		IMAGE_NT_HEADERS32 currentFileHeader = m_pEManager->getNTHeader32(m_currentFile);

		treePE->addTopLevelItem(topElement);
		InsertHeaderData(topElement,"AddressOfEntryPoint",currentFileHeader.OptionalHeader.AddressOfEntryPoint);
		InsertHeaderData(topElement,"BaseOfCode",currentFileHeader.OptionalHeader.BaseOfCode);
		InsertHeaderData(topElement,"CheckSum",currentFileHeader.OptionalHeader.CheckSum);
		//InsertHeaderData(topElement,"DataDirectory",currentFileHeader.OptionalHeader.DataDirectory);
		InsertHeaderData(topElement,"DllCharacteristics",currentFileHeader.OptionalHeader.DllCharacteristics);
		InsertHeaderData(topElement,"FileAlignment",currentFileHeader.OptionalHeader.FileAlignment);
		InsertHeaderData(topElement,"ImageBase",currentFileHeader.OptionalHeader.ImageBase);
		InsertHeaderData(topElement,"LoaderFlags",currentFileHeader.OptionalHeader.LoaderFlags);
		InsertHeaderData(topElement,"Magic",currentFileHeader.OptionalHeader.Magic);
		InsertHeaderData(topElement,"MajorImageVersion",currentFileHeader.OptionalHeader.MajorImageVersion);
		InsertHeaderData(topElement,"MajorLinkerVersion",currentFileHeader.OptionalHeader.MajorLinkerVersion);
		InsertHeaderData(topElement,"MajorOperatingSystemVersion",currentFileHeader.OptionalHeader.MajorOperatingSystemVersion);
		InsertHeaderData(topElement,"MajorSubsystemVersion",currentFileHeader.OptionalHeader.MajorSubsystemVersion);
		InsertHeaderData(topElement,"MinorImageVersion",currentFileHeader.OptionalHeader.MinorImageVersion);
		InsertHeaderData(topElement,"MinorLinkerVersion",currentFileHeader.OptionalHeader.MinorLinkerVersion);
		InsertHeaderData(topElement,"MinorOperatingSystemVersion",currentFileHeader.OptionalHeader.MinorOperatingSystemVersion);
		InsertHeaderData(topElement,"MinorSubsystemVersion",currentFileHeader.OptionalHeader.MinorSubsystemVersion);
		InsertHeaderData(topElement,"NumberOfRvaAndSizes",currentFileHeader.OptionalHeader.NumberOfRvaAndSizes);
		InsertHeaderData(topElement,"SectionAlignment",currentFileHeader.OptionalHeader.SectionAlignment);
		InsertHeaderData(topElement,"SizeOfCode",currentFileHeader.OptionalHeader.SizeOfCode);
		InsertHeaderData(topElement,"SizeOfHeaders",currentFileHeader.OptionalHeader.SizeOfHeaders);
		InsertHeaderData(topElement,"SizeOfHeapCommit",currentFileHeader.OptionalHeader.SizeOfHeapCommit);
		InsertHeaderData(topElement,"SizeOfHeapReserve",currentFileHeader.OptionalHeader.SizeOfHeapReserve);
		InsertHeaderData(topElement,"SizeOfImage",currentFileHeader.OptionalHeader.SizeOfImage);
		InsertHeaderData(topElement,"SizeOfInitializedData",currentFileHeader.OptionalHeader.SizeOfInitializedData);
		InsertHeaderData(topElement,"SizeOfStackCommit",currentFileHeader.OptionalHeader.SizeOfStackCommit);
		InsertHeaderData(topElement,"SizeOfStackReserve",currentFileHeader.OptionalHeader.SizeOfStackReserve);
		InsertHeaderData(topElement,"SizeOfUninitializedData",currentFileHeader.OptionalHeader.SizeOfUninitializedData);
		InsertHeaderData(topElement,"Subsystem",currentFileHeader.OptionalHeader.Subsystem);
		InsertHeaderData(topElement,"Win32VersionValue",currentFileHeader.OptionalHeader.Win32VersionValue);
	}
}

void qtDLGPEEditor::InsertSections()
{
	QList<IMAGE_SECTION_HEADER> sections = m_pEManager->getSections(m_currentFile);
	if(sections.size() <= 0) return;

	QTreeWidgetItem *topElement;

	topElement = new QTreeWidgetItem();
	topElement->setText(0,"Sections");
	topElement->setText(1,"VA");
	topElement->setText(2,"Virtual Size");
	topElement->setText(3,"Ptr. Raw Data");
	topElement->setText(4,"Size of Raw");
	topElement->setText(5,"Characteristics");
	treePE->addTopLevelItem(topElement);

	for(int i = 0; i < sections.size(); i++)
	{
		QTreeWidgetItem *sectionElement;

		sectionElement = new QTreeWidgetItem(topElement);
		sectionElement->setText(0,QString::fromAscii((char*)sections.at(i).Name, sizeof(sections.at(i).Name)));
		sectionElement->setText(1,QString("%1").arg(sections.at(i).VirtualAddress,8,16,QChar('0')));
		sectionElement->setText(2,QString("%1").arg(sections.at(i).Misc.VirtualSize,8,16,QChar('0')));
		sectionElement->setText(3,QString("%1").arg(sections.at(i).PointerToRawData,8,16,QChar('0')));
		sectionElement->setText(4,QString("%1").arg(sections.at(i).SizeOfRawData,8,16,QChar('0')));
		sectionElement->setText(5,QString("%1").arg(sections.at(i).Characteristics,8,16,QChar('0')));
	}
}

void qtDLGPEEditor::InsertHeaderData(QTreeWidgetItem *topElement,QString ValueName,quint64 dwValue)
{
	QTreeWidgetItem *dataElement = new QTreeWidgetItem(topElement);
	dataElement->setText(0,ValueName);
	dataElement->setText(1,QString("%1").arg(dwValue,16,16,QChar('0')));
}

void qtDLGPEEditor::OnCustomContextMenu(QPoint qPoint)
{
	QMenu menu;

	m_selectedRow = treePE->indexAt(qPoint).row();
	QList<QTreeWidgetItem *> selectedItems = treePE->selectedItems();

	if(m_selectedRow < 0 || selectedItems.count() < 0) return;
	
	m_selectedOffset = selectedItems.value(0)->text(1).toULongLong(0,16);

	menu.addAction(new QAction("Show Offset in disassembler",this));
	connect(&menu,SIGNAL(triggered(QAction*)),this,SLOT(MenuCallback(QAction*)));

	menu.exec(QCursor::pos());
}

void qtDLGPEEditor::MenuCallback(QAction* pAction)
{
	if(QString().compare(pAction->text(),"Show Offset in disassembler") == 0)
	{
		if(m_selectedOffset >= 0)
		{
			qtDLGNanomite::GetInstance()->DisAsGUI->OnDisplayDisassembly(m_selectedOffset);
			m_selectedOffset = NULL;
		}
	}
}