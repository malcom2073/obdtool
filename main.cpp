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


#include <QApplication>
//#include "qmlwindow.h"
#include "mainwindow.h"

int main(int argc, char **argv)
{
	QApplication a(argc,argv);
	int raw = 0x7FFF;
	double output = ((0xFFFF - raw) + 1) * -1;
	qDebug() << output;
	output *= 0.00390625;
	qDebug() << QString::number(output,'f');
	//return 0;
//	if (argc >= 2)
//	{
//		if (QString(argv[1]) == QString("qml"))
//		{
//			QmlWindow q;
//			q.setGeometry(0,0,1280,800);
//			q.show();
//			return a.exec();
//		}
//	}
	MainWindow m;
	m.setGeometry(100,100,1280,800);
	m.show();
	return a.exec();
}
