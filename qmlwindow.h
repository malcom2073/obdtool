/**************************************************************************
*   Copyright (C) 2010 by Michael Carpenter (malcom2073)                  *
*   malcom2073@gmail.com                                                  *
*                                                                         *
*   This file is a part of OBDToolbox                                     *
*                                                                         *
*   OBDToolbox is free software: you can redistribute it and/or modify    *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation, either version 3 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   OBDToolbox is distributed in the hope that it will be useful,         *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with OBDToolbox.  If not, see <http://www.gnu.org/licenses/>.   *
***************************************************************************/

#ifndef QMLWINDOW_H
#define QMLWINDOW_H


#include <QDeclarativeView>
#include <QDeclarativeContext>

#include <ObdThread.h>
#include <QDeclarativePropertyMap>
#include <QMap>
#include "gaugeitem.h"

class QmlWindow : public QDeclarativeView
{
	Q_OBJECT
public:
	QmlWindow();
	~QmlWindow();
	Q_INVOKABLE void connect();
	Q_INVOKABLE void disconnect();
	Q_INVOKABLE void getTroubleCodes();
	Q_INVOKABLE void clearTroubleCodes();
	Q_INVOKABLE QString toUpper(QString string);
	Q_INVOKABLE QString toLower(QString string);
	Q_INVOKABLE QString mid(QString string,int start, int len);
	Q_INVOKABLE void saveComPort(QString port,int baud);
private:
	QDeclarativeContext *context;
	ObdThread *obdThread;
	QDeclarativePropertyMap propertyMap;
	QStringList m_troubleCodeList;
	QMap<QString,double> m_pidTimeMap;
private slots:
	void obdPidReceived(QString pid,QString val,int set, double time);
	void obdTroubleCodes(QList<QString> codes);
	void obdConsoleMessage(QString message);
	void obdConnected(QString protocol);
	void obdDisconnected();
};

#endif // QMLWINDOW_H
