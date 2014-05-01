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
#ifndef CLSHEXVIEWWORKER_H
#define CLSHEXVIEWWORKER_H
#define NOMINMAX
#include <Windows.h>

#include <QThread>
#include <QMap>

struct HexData
{
	QString hexString;
	QString asciiData;
	DWORD64 hexOffset;
	int PID;
};

class clsHexViewWorker : public QThread
{
	Q_OBJECT

public:
	QMap<DWORD64,HexData> dataList;
	
	clsHexViewWorker(int PID, HANDLE hProc, DWORD64 startOffset, DWORD64 dataLength);
	~clsHexViewWorker();

private:
	DWORD64 m_startOffset,
			m_dataLength;
	HANDLE	m_hProc;
	int		m_PID;

protected:
	void run();
};

#endif
