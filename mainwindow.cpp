#include "mainwindow.h"
#include <QSettings>
#include <QDeclarativeItem>
#include "gaugeitem.h"

MainWindow::MainWindow() : QMainWindow()
{
	//0x67
	//0xe1
	//0x65


	ui.setupUi(this);
	connect(ui.actionSettings,SIGNAL(triggered()),this,SLOT(menu_settingsClicked()));
	connect(ui.actionConnect,SIGNAL(triggered()),this,SLOT(menu_actionConnectClicked()));
	connect(ui.readReadinessPushButton,SIGNAL(clicked()),this,SLOT(uiReadReadinessButtonClicked()));
	obdThread = new ObdThread(this);
	QObject::connect(obdThread,SIGNAL(pidReceived(QString,QString,int,double)),this,SLOT(obdPidReceived(QString,QString,int,double)));
	QObject::connect(obdThread,SIGNAL(troubleCodes(QList<QString>)),this,SLOT(obdTroubleCodes(QList<QString>)));
	QObject::connect(obdThread,SIGNAL(consoleMessage(QString)),this,SLOT(obdConsoleMessage(QString)));
	QObject::connect(obdThread,SIGNAL(connected(QString)),this,SLOT(obdConnected(QString)));
	QObject::connect(obdThread,SIGNAL(disconnected()),this,SLOT(obdDisconnected()));
	QObject::connect(obdThread,SIGNAL(singleShotResponse(QByteArray,QByteArray)),this,SLOT(obdSingleShotReply(QByteArray,QByteArray)));
	QObject::connect(obdThread,SIGNAL(protocolFound(QString)),this,SLOT(obdProtocolFound(QString)));
	QObject::connect(obdThread,SIGNAL(supportedPids(QList<QString>)),this,SLOT(obdSupportedPids(QList<QString>)));
	QObject::connect(obdThread,SIGNAL(liberror(ObdThread::ObdError)),this,SLOT(obdError(ObdThread::ObdError)));
	QObject::connect(obdThread,SIGNAL(supportedModes(QList<QString>)),this,SLOT(obdSupportedModes(QList<QString>)));
	QObject::connect(obdThread,SIGNAL(mfgString(QString)),this,SLOT(obdMfgString(QString)));
	QObject::connect(obdThread,SIGNAL(voltage(double)),this,SLOT(obdVoltage(double)));
	QObject::connect(obdThread,SIGNAL(monitorTestResults(QList<QString>)),this,SLOT(obdMonitorStatus(QList<QString>)));
	obdThread->start();

	pidsPerSecondTimer = new QTimer(this);
	connect(pidsPerSecondTimer,SIGNAL(timeout()),this,SLOT(pidsPerSecondTimerTick()));
	pidsPerSecondTimer->start(1000);
	ui.connectionInfoTableWidget->setRowCount(16);
	ui.connectionInfoTableWidget->setColumnCount(2);
	ui.connectionInfoTableWidget->setColumnWidth(0,220);
	ui.connectionInfoTableWidget->setColumnWidth(1,400);
	ui.connectionInfoTableWidget->horizontalHeader()->hide();
	ui.connectionInfoTableWidget->verticalHeader()->hide();
	ui.connectionInfoTableWidget->setItem(0,0,new QTableWidgetItem("Com Port"));
	ui.connectionInfoTableWidget->setItem(1,0,new QTableWidgetItem("Baud Rate"));
	ui.connectionInfoTableWidget->setItem(2,0,new QTableWidgetItem("Region Information"));
	ui.connectionInfoTableWidget->setItem(3,0,new QTableWidgetItem("Connection State"));
	ui.connectionInfoTableWidget->setItem(4,0,new QTableWidgetItem("Interface"));
	ui.connectionInfoTableWidget->setItem(5,0,new QTableWidgetItem("Scan Tool"));
	ui.connectionInfoTableWidget->setItem(6,0,new QTableWidgetItem("OBD Protocol"));
	ui.connectionInfoTableWidget->setItem(7,0,new QTableWidgetItem("Vehicle Battery Voltage"));
	ui.connectionInfoTableWidget->setItem(8,0,new QTableWidgetItem("OBD Requirements"));
	ui.connectionInfoTableWidget->setItem(9,0,new QTableWidgetItem("MIL Status"));
	ui.connectionInfoTableWidget->setItem(10,0,new QTableWidgetItem("DTC's present"));
	ui.connectionInfoTableWidget->setItem(11,0,new QTableWidgetItem("Engine"));
	ui.connectionInfoTableWidget->setItem(12,0,new QTableWidgetItem("Service $05 - O2 Sensors"));
	ui.connectionInfoTableWidget->setItem(13,0,new QTableWidgetItem("Service $06 - On Board Monitoring"));
	ui.connectionInfoTableWidget->setItem(14,0,new QTableWidgetItem("Service $08 - In Use Performance Tracking"));
	ui.connectionInfoTableWidget->setItem(15,0,new QTableWidgetItem("Service $09 - Vehicle Information"));

	for (int i=0;i<16;i++)
	{
		ui.connectionInfoTableWidget->setItem(i,1,new QTableWidgetItem());
	}

	ui.pidSelectTableWidget->setRowCount(0);
	ui.pidSelectTableWidget->setColumnCount(2);
	ui.pidSelectTableWidget->setColumnWidth(0,100);
	ui.pidSelectTableWidget->setColumnWidth(1,200);
	//ui.pidSelectTableWidget->horizontalHeader()->hide();
	ui.pidSelectTableWidget->setHorizontalHeaderLabels(QStringList() << "PID" << "Description");
	ui.pidSelectTableWidget->verticalHeader()->hide();

	//connect(ui.pidSelectTableWidget,SIGNAL(itemClicked(QTableWidgetItem*)))

	QList<QString> pidlist;
	pidlist.append("010C");
	pidlist.append("010D");
	pidlist.append("010F");
	pidlist.append("0104");
	pidlist.append("0105");
	//obdSupportedPids(pidlist);



	QSettings settings("IFS","obdqmltool");
	settings.beginGroup("settings");
	QString port = settings.value("comport","").toString();
	int baud = settings.value("baudrate",0).toInt();
	settings.endGroup();
	qDebug() << port << baud;
	ui.status_comPortLabel->setText("Com port: " + port);
	ui.status_comBaudLabel->setText("Baud Rate: " + QString::number(baud));
	obdThread->setPort(port);
	obdThread->setBaud(baud);

	ui.connectionInfoTableWidget->item(0,1)->setText(port);
	ui.connectionInfoTableWidget->item(1,1)->setText(QString::number(baud));
	//ui.connectionInfoTableWidget->setItem(7,0,new QTableWidgetItem(""));

	gaugeView = new QDeclarativeView(ui.gaugesTab);
	qmlRegisterType<GaugeItem>("GaugeImage",0,1,"GaugeImage");
	gaugeView->setGeometry(0,0,this->width()-5,this->height()-(ui.statusbar->height()+30));
	gaugeView->setSource(QUrl("gaugeview.qml"));
	gaugeView->show();
	ui.statusbar->addWidget(ui.status_comPortLabel);
	ui.statusbar->addWidget(ui.status_comBaudLabel);
	ui.statusbar->addWidget(ui.status_comStatusLabel);
	ui.statusbar->addWidget(ui.status_comInterfaceLabel);
	ui.statusbar->addWidget(ui.status_comProtocolLabel);
	ui.statusbar->addWidget(ui.status_pidUpdateRateLabel);

	ui.readPidsTableWidget->setColumnCount(7);
	ui.readPidsTableWidget->setColumnWidth(0,50);
	ui.readPidsTableWidget->setColumnWidth(1,200);
	ui.readPidsTableWidget->setColumnWidth(2,100);
	ui.readPidsTableWidget->setColumnWidth(3,75);
	ui.readPidsTableWidget->setColumnWidth(4,75);
	ui.readPidsTableWidget->setColumnWidth(5,75);
	ui.readPidsTableWidget->setColumnWidth(6,75);

	ui.readPidsTableWidget->setHorizontalHeaderLabels(QStringList() << "PID" << "Description" << "Value" << "Units" << "Minimum" << "Maximum" << "Average");
	ui.readPidsTableWidget->verticalHeader()->hide();

	obdThread->addRequest(0x01,0x0C,1,0);
	obdThread->addRequest(0x01,0x0D,2,0);
	obdThread->addRequest(0x01,0x05,5,0);
	obdThread->addRequest(0x01,0x10,1,0);
	obdThread->addRequest(0x01,0x11,1,0);
	obdThread->addRequest(0x01,0x04,2,0);
	obdThread->addRequest(0x01,0x31,100,0);

	addReadPidRow("010C");
	addReadPidRow("010D");
	addReadPidRow("0105");
	addReadPidRow("0110");
	addReadPidRow("0111");
	addReadPidRow("0104");
	addReadPidRow("0131");

	graph = new EGraph(ui.graphsTab);
	graph->setGeometry(0,0,800,600);
	graph->addGraph("RPM");
	graph->setMax(0,7000);
	graph->setMin(0,0);
	graph->addGraph("Speed");
	graph->setMax(1,255);
	m_graphPidMap["010D"] = 0;
	m_graphPidMap["010C"] = 1;
	graph->show();

	connect(ui.connectPushButton,SIGNAL(clicked()),this,SLOT(connectButtonClicked()));



	ui.nonconTableWidget->setColumnCount(3);
	ui.nonconTableWidget->setHorizontalHeaderLabels(QStringList() << "Non continuous monitoring tests" << "Supported" << "Completed");
	ui.nonconTableWidget->verticalHeader()->hide();
	ui.nonconTableWidget->setColumnWidth(0,200);
	ui.nonconTableWidget->setColumnWidth(1,50);
	ui.nonconTableWidget->setColumnWidth(2,50);

	ui.nonconTableWidget->setRowCount(8);
	ui.nonconTableWidget->setItem(0,0,new QTableWidgetItem("Catalyst"));
	ui.nonconTableWidget->setItem(1,0,new QTableWidgetItem("Heated catalyst"));
	ui.nonconTableWidget->setItem(2,0,new QTableWidgetItem("Evaporative system"));
	ui.nonconTableWidget->setItem(3,0,new QTableWidgetItem("Secondary air system"));
	ui.nonconTableWidget->setItem(4,0,new QTableWidgetItem("A/C System refridgerant"));
	ui.nonconTableWidget->setItem(5,0,new QTableWidgetItem("Oxygen sensor"));
	ui.nonconTableWidget->setItem(6,0,new QTableWidgetItem("Oxygen sensor heater"));
	ui.nonconTableWidget->setItem(7,0,new QTableWidgetItem("EGR System"));

	for (int i=0;i<8;i++)
	{
		ui.nonconTableWidget->setItem(i,1,new QTableWidgetItem());
		ui.nonconTableWidget->setItem(i,2,new QTableWidgetItem());
	}

	ui.conTableWidget->setColumnCount(3);
	ui.conTableWidget->setHorizontalHeaderLabels(QStringList() << "Continuous monitoring tests" << "Supported" << "Completed");
	ui.conTableWidget->verticalHeader()->hide();
	ui.conTableWidget->setColumnWidth(0,200);
	ui.conTableWidget->setColumnWidth(1,75);
	ui.conTableWidget->setColumnWidth(2,75);

	ui.conTableWidget->setRowCount(3);
	ui.conTableWidget->setItem(0,0,new QTableWidgetItem("Misfire"));
	ui.conTableWidget->setItem(1,0,new QTableWidgetItem("Fuel System"));
	ui.conTableWidget->setItem(2,0,new QTableWidgetItem("Comprehensive Component"));

	for (int i=0;i<3;i++)
	{
		ui.conTableWidget->setItem(i,1,new QTableWidgetItem());
		ui.conTableWidget->setItem(i,2,new QTableWidgetItem());
	}

	ui.monitorStatusTableWidget->setColumnCount(2);
	ui.monitorStatusTableWidget->setHorizontalHeaderLabels(QStringList() << "Monitor Status" << "Status");
	ui.monitorStatusTableWidget->verticalHeader()->hide();
	ui.monitorStatusTableWidget->setColumnWidth(0,200);
	ui.monitorStatusTableWidget->setColumnWidth(1,75);
	QList<QString> tmplist;
	tmplist.append("1:1");
	tmplist.append("1:0");
	tmplist.append("1:1");
	tmplist.append("0:0");
	tmplist.append("1:0");
	tmplist.append("0:1");
	tmplist.append("1:1");
	tmplist.append("0:1");
	tmplist.append("0:1");
	tmplist.append("1:1");
	tmplist.append("0:1");
	//tmplist.append("1:0");
	//obdMonitorStatus(tmplist);


	unsigned char one = 0x00;//m_obd->byteArrayToByte(vect2[0].toAscii(),vect2[1].toAscii());
	unsigned char two = 0x67;//m_obd->byteArrayToByte(vect2[2].toAscii(),vect2[3].toAscii());
	unsigned char three = 0xe1; //m_obd->byteArrayToByte(vect2[4].toAscii(),vect2[5].toAscii());
	unsigned char four = 0x65; //m_obd->byteArrayToByte(vect2[6].toAscii(),vect2[7].toAscii());
	QList<QString> resultlist;
	QString misfire = QString(((two >> 0) & 1) ? "1" : "0") + ":" + (((two >> 4) & 1) ? "1" : "0");
	QString fuelsystem = QString(((two >> 1) & 1) ? "1" : "0") + ":" + (((two >> 5) & 1) ? "1" : "0");
	QString component = QString(((two >> 2) & 1) ? "1" : "0") + ":" + (((two >> 6) & 1) ? "1" : "0");
	//	QString reserved = QString(((two >> 3) & 1) ? "1" : "0") + ":" + (((two >> 7) & 1) ? "1" : "0");
	QString catalyst = QString(((three >> 0) & 1) ? "1" : "0") + ":" + (((four >> 0) & 1) ? "1" : "0");
	QString heatedcat =QString (((three >> 1) & 1) ? "1" : "0") + ":" + (((four >> 1) & 1) ? "1" : "0");
	QString evapsys = QString(((three >> 2) & 1) ? "1" : "0") + ":" + (((four >> 2) & 1) ? "1" : "0");
	QString secondair = QString(((three >> 3) & 1) ? "1" : "0") + ":" + (((four >> 3) & 1) ? "1" : "0");
	QString acrefrig = QString(((three >> 4) & 1) ? "1" : "0") + ":" + (((four >> 4) & 1) ? "1" : "0");
	QString oxygensensor = QString(((three >> 5) & 1) ? "1" : "0") + ":" + (((four >> 5) & 1) ? "1" : "0");
	QString oxygenheater = QString(((three >> 6) & 1) ? "1" : "0") + ":" + (((four >> 6) & 1) ? "1" : "0");
	QString egrsystem = QString(((three >> 7) & 1) ? "1" : "0") + ":" + (((four >> 7) & 1) ? "1" : "0");

	resultlist.append(misfire);
	resultlist.append(fuelsystem);
	resultlist.append(component);
	//resultlist.append(reserved);
	resultlist.append(catalyst);
	resultlist.append(heatedcat);
	resultlist.append(evapsys);
	resultlist.append(secondair);
	resultlist.append(acrefrig);
	resultlist.append(oxygensensor);
	resultlist.append(oxygenheater);
	resultlist.append(egrsystem);
	//obdMonitorStatus(resultlist);


}
void MainWindow::uiReadReadinessButtonClicked()
{
	obdThread->reqMonitorStatus();


}

void MainWindow::connectButtonClicked()
{
    obdThread->connect();
}

void MainWindow::changeEvent(QEvent *evt)
{
	/*ui.tabWidget->setGeometry(0,0,this->width(),this->height()-(ui.statusbar->height()+20));
	ui.connectionInfoTableWidget->setGeometry(0,0,this->width()-5,this->height()-(ui.statusbar->height()+70));
	ui.pidSelectTableWidget->setGeometry(0,0,this->width()-5,this->height()-(ui.statusbar->height()+45));
	ui.readPidsTableWidget->setGeometry(0,0,this->width()-5,this->height()-(ui.statusbar->height()+45));*/
	//QMainWindow:changeEvent(evt);
}
void MainWindow::pidsPerSecondTimerTick()
{
	ui.status_pidUpdateRateLabel->setText("Pid Rate: " + QString::number(m_pidcount));
	m_pidcount=0;
}

void MainWindow::addReadPidRow(QString pid)
{
	ObdInfo::Pid *p = obdThread->getInfo()->getPidFromString(pid);
	if (!p)
	{
		qDebug() << "Not valid" << pid;
		return;
	}
	ui.readPidsTableWidget->setRowCount(ui.readPidsTableWidget->rowCount()+1);
	int index = ui.readPidsTableWidget->rowCount()-1;
	ui.readPidsTableWidget->setItem(index,0,new QTableWidgetItem(pid));

	ui.readPidsTableWidget->setItem(index,1,new QTableWidgetItem(p->description));
	QTableWidgetItem *val = new QTableWidgetItem();
	ui.readPidsTableWidget->setItem(index,2,val);
	m_readPidTableMap[pid] = val;
	ui.readPidsTableWidget->setItem(index,3,new QTableWidgetItem(p->unit));
	ui.readPidsTableWidget->setItem(index,4,new QTableWidgetItem(QString::number(p->min)));
	ui.readPidsTableWidget->setItem(index,5,new QTableWidgetItem(QString::number(p->max)));
	ui.readPidsTableWidget->setItem(index,6,new QTableWidgetItem("avg"));
}
void MainWindow::resizeEvent(QResizeEvent *evt)
{
	ui.tabWidget->setGeometry(0,0,this->width(),this->height()-(ui.statusbar->height()+20));
}

void MainWindow::obdSingleShotReply(QByteArray req,QByteArray reply)
{
	qDebug() << "Raw Reply" << req << reply;
	if (req.startsWith("AT@1"))
	{

	}
	if (req.startsWith("ATRV"))
	{

	}
}
void MainWindow::obdVoltage(double volts)
{
	ui.connectionInfoTableWidget->item(7,1)->setText(QString::number(volts));
}

void MainWindow::obdMfgString(QString string)
{
	ui.connectionInfoTableWidget->item(5,1)->setText(string);
}

void MainWindow::obdSupportedModes(QList<QString> list)
{
	if (list.contains("05"))
	{
		ui.connectionInfoTableWidget->item(12,1)->setText("Supported");
	}
	else
	{
		ui.connectionInfoTableWidget->item(12,1)->setText("Not supported");
	}
	if (list.contains("06"))
	{
		ui.connectionInfoTableWidget->item(13,1)->setText("Supported");
	}
	else
	{
		ui.connectionInfoTableWidget->item(13,1)->setText("Not Supported");
	}
	if (list.contains("08"))
	{
		ui.connectionInfoTableWidget->item(14,1)->setText("Supported");
	}
	else
	{
		ui.connectionInfoTableWidget->item(14,1)->setText("Not Supported");
	}
	if (list.contains("09"))
	{
		ui.connectionInfoTableWidget->item(15,1)->setText("Supported");
	}
	else
	{
		ui.connectionInfoTableWidget->item(15,1)->setText("Not Supported");
	}
}

void MainWindow::uiPidSelectTableClicked(int row, int column)
{
	for (int i=0;i<ui.pidSelectTableWidget->rowCount();i++)
	{
		if (ui.pidSelectTableWidget->item(i,0)->checkState() == Qt::Checked)
		{
			if (!m_readPidTableMap.contains(ui.pidSelectTableWidget->item(i,0)->text()))
			{
				//Pid is not currently on our list! Let's add it.
				addReadPidRow(ui.pidSelectTableWidget->item(i,0)->text());
			}
		}
		else
		{
			if (!m_readPidTableMap.contains(ui.pidSelectTableWidget->item(i,0)->text()))
			{
			}
		}
	}
}
void MainWindow::obdMonitorStatus(QList<QString> list)
{
	for (int i=0;i<list.size();i++)
	{
		if (list[i].size() != 3)
		{
			qDebug() << "Invalid obdMonitorStatus returned:" << list[i] << "line" << i;
			return;
		}
	}
	if (list.size() != 11)
	{
		qDebug() << "Invalid list size returned for obdMonitorStatus." << list.size();
		return;
	}
	//ui.conTableWidget->item(0,1)->setText(((list[0][0] == '0') ? "N" : "Y"));
	//ui.conTableWidget->item(0,2)->setText((list[0][0]=='0') ? "" : (list[0][2] == '1') ? "N" : "Y");
	for (int i=0;i<3;i++)
	{
		ui.conTableWidget->item(i,1)->setText(((list[i][0] == '0') ? "N" : "Y"));
		ui.conTableWidget->item(i,2)->setText((list[i][0]=='0') ? "" : (list[i][2] == '1') ? "N" : "Y");
	}
	for (int i=3;i<list.size();i++)
	{
		ui.nonconTableWidget->item(i-3,1)->setText(((list[i][0] == '0') ? "N" : "Y"));
		ui.nonconTableWidget->item(i-3,2)->setText((list[i][0]=='0') ? "" : (list[i][2] == '1') ? "N" : "Y");
	}
	/*if (list[0][0] == '0')
	{
		ui.conTableWidget->item(0,1)->setText("N");
		//Misfire supported
	}
	else
	{
		ui.conTableWidget->item(0,1)->setText("Y");
		if (list[0][2] == '1')
		{
			//Misfire incomplete
			ui.conTableWidget->item(0,2)->setText("N");
		}
		else
		{
			ui.conTableWidget->item(0,2)->setText("Y");
		}

	}
	if (list[1][0] == '0')
	{
		ui.conTableWidget->item(1,1)->setText("N");
	}*/
}

MainWindow::~MainWindow()
{
//	obdThread->disconnect();
//	while (obdThread->isRunning());
//	delete obdThread;
}
void MainWindow::menu_settingsClicked()
{
	settingsWidget = new SettingsWidget();
	connect(settingsWidget,SIGNAL(saveSettings(QString,int)),this,SLOT(settings_saveComPort(QString,int)));
	//QMdiSubWindow *win = ui.mdiArea->addSubWindow(settingsWidget);
	//win->setGeometry(100,100,311,156);
	settingsWidget->show();
}
void MainWindow::obdSupportedPids(QList<QString> pidlist)
{
	ui.pidSelectTableWidget->clearContents();
	ui.pidSelectTableWidget->setRowCount(pidlist.size());
	for (int i=0;i<pidlist.size();i++)
	{
		ui.pidSelectTableWidget->setItem(i,0,new QTableWidgetItem(pidlist[i]));
		ui.pidSelectTableWidget->item(i,0)->setCheckState(Qt::Checked);
		if (obdThread->getInfo()->getPidFromString(pidlist[i]))
		{
			ui.pidSelectTableWidget->setItem(i,1,new QTableWidgetItem(obdThread->getInfo()->getPidFromString(pidlist[i])->description));
		}
	}
}
void MainWindow::obdProtocolFound(QString protocol)
{
	ui.connectionInfoTableWidget->item(6,1)->setText(protocol);
	ui.status_comProtocolLabel->setText("Protocol: " + protocol);
}

void MainWindow::settings_saveComPort(QString port,int baud)
{
	QSettings settings("IFS","obdqmltool");
	settings.beginGroup("settings");
	//QString port = settings.value("comport","").toString();
	//int baud = settings.value("baudrate",0).toInt();
	settings.setValue("comport",port);
	settings.setValue("baudrate",baud);
	settings.endGroup();

	obdThread->setPort(port);
	obdThread->setBaud(baud);
	ui.status_comPortLabel->setText("Com port: " + port);
	ui.status_comBaudLabel->setText("Baud Rate: " + QString::number(baud));
	ui.connectionInfoTableWidget->item(0,1)->setText(port);
	ui.connectionInfoTableWidget->item(1,1)->setText(QString::number(baud));
	settingsWidget->deleteLater();
}
void MainWindow::obdPidReceived(QString pid,QString val,int set, double time)
{
	qDebug() << pid << val;
	if (m_readPidTableMap.contains(pid))
	{
		m_readPidTableMap[pid]->setText(val);
	}
	m_pidcount++;
}
void MainWindow::obdConnected(QString version)
{
	ui.connectionInfoTableWidget->item(3,1)->setText("Connected");
	ui.connectionInfoTableWidget->item(4,1)->setText(version);
	//obdThread->switchBaud();
	//obdThread->reqVoltage();
	//obdThread->reqSupportedModes();
	//obdThread->reqMfgString();
	ui.status_comStatusLabel->setText("Status: Connected");
	ui.status_comInterfaceLabel->setText("Interface: " + version);
	//ui.status_comBaudLabel->setText()
}
void MainWindow::menu_actionConnectClicked()
{
	obdThread->connect();
	ui.status_comStatusLabel->setText("Status: Connecting");
}

void MainWindow::obdDisconnected()
{
	ui.connectionInfoTableWidget->item(3,1)->setText("Disconnected");
}

void MainWindow::obdConsoleMessage(QString message)
{
}

void MainWindow::obdTroubleCodes(QList<QString> codes)
{
	qDebug() << "Codes:" << codes.size();
	ui.connectionInfoTableWidget->item(9,1)->setText("Clear");
	ui.connectionInfoTableWidget->item(10,1)->setText("0");
}
void MainWindow::obdError(ObdThread::ObdError err)
{
	qDebug() << "Error";
	if (err == ObdThread::UNABLE_TO_OPEN_COM_PORT)
	{
		ui.status_comStatusLabel->setText("Status: Unable to open Com Port");
	}
}
