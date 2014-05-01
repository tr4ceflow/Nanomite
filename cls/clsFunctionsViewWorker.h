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
#ifndef CLSFUNCTIONSWORKER_H
#define CLSFUNCTIONSWORKER_H
#define NOMINMAX
#include <Windows.h>

#include <QThread>
#include <QMap>

struct FunctionData
{
	quint64 FunctionOffset;
	quint64 FunctionSize;
	int PID;
	QString functionSymbol;
};

struct FunctionProcessingData
{
	PTCHAR currentModule;
	HANDLE hProc;
	int PID;
};

class clsFunctionsViewWorker : public QThread
{
	Q_OBJECT

public:
	QList<FunctionData>	functionList;
	
	clsFunctionsViewWorker(QList<FunctionProcessingData> dataForProcessing);
	~clsFunctionsViewWorker();

private:
	QList<FunctionProcessingData> m_processingData;

	void GetValidMemoryParts(PTCHAR lpCurrentName,HANDLE hProc);
	void ParseMemoryRangeForFunctions(HANDLE hProc,quint64 BaseAddress,quint64 Size);
	void InsertSymbolsIntoLists(HANDLE hProc,WORD PID);

	quint64 GetPossibleFunctionEnding(quint64 BaseAddress,quint64 Size,DWORD SearchPattern,LPVOID lpBuffer,int SpaceLen);

	QList<FunctionData> GetPossibleFunctionBeginning(int PID, quint64 StartOffset,quint64 Size,DWORD SearchPattern,LPVOID lpBuffer,int SpaceLen);

protected:
	void run();
};

#endif
