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


#include "qmlwindow.h"
#include <QImage>
#include <QDeclarativeEngine>
#include <QProcess>
#include <QDateTime>
//#include <QGLFormat>
#include <QSettings>
QmlWindow::QmlWindow() : QDeclarativeView()
{
	QProcess proc;
	proc.setProcessChannelMode(QProcess::MergedChannels);
	proc.setReadChannel(QProcess::StandardOutput);
	proc.start("C:\\Program Files\\WinSCP\\WinSCP.com");
	proc.waitForStarted();
	proc.waitForReadyRead();
	proc.waitForFinished();
	QString all = proc.readAllStandardOutput();
	qDebug() << all;
	qmlRegisterType<GaugeItem>("GaugeImage",0,1,"GaugeImage");
	setWindowFlags(Qt::FramelessWindowHint);
	setOptimizationFlags(QGraphicsView::DontSavePainterState);
	setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
	scene()->setItemIndexMethod(QGraphicsScene::NoIndex);
	//QGLFormat format = QGLFormat::defaultFormat();
	//format.setSampleBuffers(false);
	//format.setDoubleBuffer(true);
	//setViewport(new QGLWidget(format));
	context = rootContext();
	//context->setContextProperty("backgroundColor",QColor(Qt::yellow));
	//context->setContextProperty("playlistListModel",QVariant::fromValue(m_playList));
	context->setContextProperty("window",this);
	context->setContextProperty("propertyMap",&propertyMap);
	//setCurrentSongText("Test");
	//engine()->addImportPath(".");
	//engine()->addPluginPath(".");
	setSource(QUrl::fromLocalFile("main.qml"));
	//QImage img("Gauge/gauge.png");


	QObject::connect(engine(),SIGNAL(quit()),this,SLOT(close()));
	obdThread = new ObdThread(this);
	QObject::connect(obdThread,SIGNAL(pidReceived(QString,QString,int,double)),this,SLOT(obdPidReceived(QString,QString,int,double)));
	QObject::connect(obdThread,SIGNAL(troubleCodes(QList<QString>)),this,SLOT(obdTroubleCodes(QList<QString>)));
	QObject::connect(obdThread,SIGNAL(consoleMessage(QString)),this,SLOT(obdConsoleMessage(QString)));
	QObject::connect(obdThread,SIGNAL(connected(QString)),this,SLOT(obdConnected(QString)));
	QObject::connect(obdThread,SIGNAL(disconnected()),this,SLOT(obdDisconnected()));
	QSettings settings("IFS","obdqmltool");
	settings.beginGroup("settings");
	//settings.setProperty("comport",port);
	//settings.setProperty("baudrate",baud);
	QString port = settings.value("comport","").toString();
	int baud = settings.value("baudrate",0).toInt();
	qDebug() << "Settings read:" << port << baud;
	settings.endGroup();
	//text: propertyMap["setup_combaud_textinput"]
	propertyMap.setProperty("setup_combaud_textinput",baud);
	propertyMap.setProperty("setup_comport_textinput",port);
	obdThread->setPort(port);
	obdThread->setBaud(baud);
	obdThread->start();
	obdThread->addRequest(0x01,0x0C,1,0);
	obdThread->addRequest(0x01,0x0D,2,0);
	obdThread->addRequest(0x01,0x05,50,0);
	obdThread->addRequest(0x01,0x04,2,0);
	obdThread->addRequest(0x01,0x10,1,0);
	obdThread->addRequest(0x01,0x11,1,0);
	propertyMap.setProperty("troublemodel",m_troubleCodeList);

}

QString QmlWindow::toUpper(QString string)
{
	return string.toUpper();
}
QString QmlWindow::toLower(QString string)
{
	return string.toLower();
}
QString QmlWindow::mid(QString string,int start, int len)
{
	return string.mid(start,len);
}
void QmlWindow::saveComPort(QString port,int baud)
{
	QSettings settings("IFS","obdqmltool");
	settings.beginGroup("settings");
	settings.setValue("comport",port);
	settings.setValue("baudrate",baud);
	settings.endGroup();
	obdThread->setPort(port);
	obdThread->setBaud(baud);
	propertyMap.setProperty("setup_combaud_textinput",baud);
	propertyMap.setProperty("setup_comport_textinput",port);
	qDebug() << "Settings saved:" << port << baud;
}
void QmlWindow::connect()
{
	obdThread->connect();
}

void QmlWindow::disconnect()
{
	obdThread->disconnect();
}

QmlWindow::~QmlWindow()
{
//	obdThread->disconnect();
//	while (obdThread->isRunning());
	delete obdThread;
}

void QmlWindow::obdPidReceived(QString pid,QString val,int set, double time)
{
	Q_UNUSED(set);
	Q_UNUSED(time);
	if (m_pidTimeMap.contains(pid))
	{
		double newtime = QDateTime::currentMSecsSinceEpoch() - m_pidTimeMap[pid];
		//qDebug() << pid << val << set << QString::number(QDateTime::currentMSecsSinceEpoch(),'f') << newtime << QDateTime::currentMSecsSinceEpoch();
		propertyMap.setProperty(QString(pid + "_DURATION").toLatin1(),newtime);
        }
        m_pidTimeMap[pid] = QDateTime::currentMSecsSinceEpoch();
	propertyMap.setProperty(pid.toLatin1(),val);
}
void QmlWindow::obdConnected(QString protocol)
{
	Q_UNUSED(protocol);
	propertyMap.setProperty("connectedMessage","Connected");
}

void QmlWindow::obdDisconnected()
{
	propertyMap.setProperty("connectedMessage","Disconnected");
}

void QmlWindow::obdConsoleMessage(QString message)
{
	propertyMap.setProperty("consoleMessage",message);
}

void QmlWindow::obdTroubleCodes(QList<QString> codes)
{
	m_troubleCodeList.append(codes);
	propertyMap.setProperty("troublemodel",m_troubleCodeList);
}

void QmlWindow::getTroubleCodes()
{
	obdThread->sendReqTroubleCodes();
}

void QmlWindow::clearTroubleCodes()
{
	obdThread->sendClearTroubleCodes();
}
