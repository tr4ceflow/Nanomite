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
#ifndef QTDLGREGISTERS_H
#define QTDLGREGISTERS_H

#include "clsDebugger\clsDebugger.h"

#include <QDockWidget>

#include "ui_qtDLGRegisters.h"

#include <stdint.h>

typedef struct _Binary80 {
	unsigned int sign : 1;
	unsigned int exponent : 15;
	unsigned long long mantissa : 64;

	_Binary80() {
		sign = 0;
		exponent = 0;
		mantissa = 0;
	}
} Binary80;

enum {
	eExtendedPrecisionBias = 16383
};

class qtDLGRegisters : public QDockWidget, public Ui_qtDLGRegisters
{
	Q_OBJECT

public:
	qtDLGRegisters(QWidget *parent = 0);
	~qtDLGRegisters();

	void LoadRegView(clsDebugger *coreDebugger);

public slots:
	void LoadRegView();

signals:
	void OnDisplayDisassembly(quint64 dwEIP);


private:
	void PrintValueInTable(QTableWidget *pTable, QString regName, QString regValue);

	double readFloat80(const uint8_t buffer[10]);

	bool checkCorrectnessValue(const QString &regVal);

	Binary80* toExtendedPrecision(const QString &regVal);

private:
	typedef struct {
		DWORD64 low;
		DWORD64 high;
	} uint128_t;

	int m_iSelectedRow;

	QString m_savedValue;

private slots:
	void OnContextMenu(QPoint);
	void OnChangeRequest(QTableWidgetItem *pItem);
	void MenuCallback(QAction* pAction);

	void slot_changeRegisterValue(QTableWidgetItem *item);
};

#endif // QTDLGREGISTERS_H
