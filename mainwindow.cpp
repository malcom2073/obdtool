#include "mainwindow.h"
#include <QSettings>
#include <QDeclarativeItem>
#include "gaugeitem.h"
#include <QDeclarativeContext>

MainWindow::MainWindow() : QMainWindow()
{
	ui.setupUi(this);

	m_demoMode = false;

	//Setup all the UI signals, connecting them to slots.
	connect(ui.actionSettings,SIGNAL(triggered()),this,SLOT(menu_settingsClicked()));
	connect(ui.actionConnect,SIGNAL(triggered()),this,SLOT(menu_actionConnectClicked()));
	connect(ui.readReadinessPushButton,SIGNAL(clicked()),this,SLOT(uiReadReadinessButtonClicked()));
	connect(ui.rawConsoleLineEdit,SIGNAL(returnPressed()),this,SLOT(rawConsoleReturnPressed()));
	//connect(ui.connectPushButton,SIGNAL(clicked()),this,SLOT(connectButtonClicked()));
	connect(ui.pidSelectNonePushButton,SIGNAL(clicked()),this,SLOT(uiPidSelectClearClicked()));
	connect(ui.pidSelectSavePushButton,SIGNAL(clicked()),this,SLOT(uiPidSelectSaveClicked()));
	connect(ui.connectPushButton,SIGNAL(clicked()),this,SLOT(menu_actionConnectClicked()));
	connect(ui.disconnectPushButton,SIGNAL(clicked()),this,SLOT(menu_actionDisconnectClicked()));
	connect(ui.action_Exit,SIGNAL(triggered()),this,SLOT(menu_actionExit()));
	connect(ui.monitorPushButton,SIGNAL(clicked()),this,SLOT(uiMonitorButtonClicked()));
	connect(ui.troubleReadPushButton,SIGNAL(clicked()),this,SLOT(uiTroubleReadClicked()));
	connect(ui.troubleClearPushButton,SIGNAL(clicked()),this,SLOT(uiTroubleClearClicked()));
	connect(ui.graphScrollCheckBox,SIGNAL(stateChanged(int)),this,SLOT(graphScrollButtonStateChanged(int)));
	connect(ui.graphZoomCheckBox,SIGNAL(stateChanged(int)),this,SLOT(graphZoomButtonStateChanged(int)));


	//OBDThread setup and signal connections.
	obdThread = new ObdThread(this);
	QObject::connect(obdThread,SIGNAL(pidReply(QString,QString,int,double)),this,SLOT(obdPidReceived(QString,QString,int,double)));
	QObject::connect(obdThread,SIGNAL(troubleCodesReply(QString,QList<QString>)),this,SLOT(obdTroubleCodes(QString,QList<QString>)));
	QObject::connect(obdThread,SIGNAL(consoleMessage(QString)),this,SLOT(obdConsoleMessage(QString)));
	QObject::connect(obdThread,SIGNAL(connected(QString)),this,SLOT(obdConnected(QString)));
	QObject::connect(obdThread,SIGNAL(disconnected()),this,SLOT(obdDisconnected()));
	QObject::connect(obdThread,SIGNAL(singleShotReply(QByteArray,QByteArray)),this,SLOT(obdSingleShotReply(QByteArray,QByteArray)));
	QObject::connect(obdThread,SIGNAL(protocolReply(QString)),this,SLOT(obdProtocolFound(QString)));
	QObject::connect(obdThread,SIGNAL(supportedPidsReply(QList<QString>)),this,SLOT(obdSupportedPids(QList<QString>)));
	QObject::connect(obdThread,SIGNAL(liberror(ObdThread::ObdError)),this,SLOT(obdError(ObdThread::ObdError)));
	QObject::connect(obdThread,SIGNAL(supportedModesReply(QList<QString>)),this,SLOT(obdSupportedModes(QList<QString>)));
	QObject::connect(obdThread,SIGNAL(mfgStringReply(QString)),this,SLOT(obdMfgString(QString)));
	QObject::connect(obdThread,SIGNAL(voltageReply(double)),this,SLOT(obdVoltage(double)));
	//QObject::connect(obdThread,SIGNAL(monitorTestReply(QList<QString>)),this,SLOT(obdMonitorStatus(QList<QString>)));
	QObject::connect(obdThread,SIGNAL(monitorTestReply(QMap<ObdThread::CONTINUOUS_MONITOR,ObdThread::MONITOR_COMPLETE_STATUS>)),this,SLOT(obdMonitorStatus(QMap<ObdThread::CONTINUOUS_MONITOR,ObdThread::MONITOR_COMPLETE_STATUS>)));
	QObject::connect(obdThread,SIGNAL(onBoardMonitoringReply(QList<unsigned char>,QList<unsigned char>,QList<QString>,QList<QString>,QList<QString>,QList<QString>)),this,SLOT(obdOnBoardMonitoringReply(QList<unsigned char>,QList<unsigned char>,QList<QString>,QList<QString>,QList<QString>,QList<QString>)));
	QObject::connect(obdThread,SIGNAL(rawCommsMessage(QString)),this,SLOT(obdRawCommLog(QString)));

	obdThread->start();
//monitorTestReply(QMap<CONTINUOUS_MONITOR,MONITOR_COMPLETE_STATUS> monitorlist)

	//Timer to count how quickly we are reading pids.
	pidsPerSecondTimer = new QTimer(this);
	connect(pidsPerSecondTimer,SIGNAL(timeout()),this,SLOT(pidsPerSecondTimerTick()));
	pidsPerSecondTimer->start(1000);


	//Connection summary table
	ui.connectionInfoTableWidget->setRowCount(16);
	ui.connectionInfoTableWidget->setColumnCount(2);
	ui.connectionInfoTableWidget->setColumnWidth(0,220);
	ui.connectionInfoTableWidget->setColumnWidth(1,400);
	ui.connectionInfoTableWidget->horizontalHeader()->hide();
	ui.connectionInfoTableWidget->verticalHeader()->hide();
	ui.connectionInfoTableWidget->setItem(0,0,new QTableWidgetItem("Com Port"));
	ui.connectionInfoTableWidget->setItem(1,0,new QTableWidgetItem("Baud Rate"));
	//ui.connectionInfoTableWidget->setItem(2,0,new QTableWidgetItem("Region Information"));
	ui.connectionInfoTableWidget->hideRow(2);
	ui.connectionInfoTableWidget->setItem(3,0,new QTableWidgetItem("Connection State"));
	ui.connectionInfoTableWidget->setItem(4,0,new QTableWidgetItem("Interface"));
	ui.connectionInfoTableWidget->setItem(5,0,new QTableWidgetItem("Scan Tool"));
	ui.connectionInfoTableWidget->setItem(6,0,new QTableWidgetItem("OBD Protocol"));
	ui.connectionInfoTableWidget->setItem(7,0,new QTableWidgetItem("Vehicle Battery Voltage"));
	ui.connectionInfoTableWidget->setItem(8,0,new QTableWidgetItem("OBD Requirements"));
	ui.connectionInfoTableWidget->hideRow(8);
	ui.connectionInfoTableWidget->setItem(9,0,new QTableWidgetItem("MIL Status"));
	ui.connectionInfoTableWidget->hideRow(9);
	ui.connectionInfoTableWidget->setItem(10,0,new QTableWidgetItem("DTC's present"));
	ui.connectionInfoTableWidget->hideRow(10);
	ui.connectionInfoTableWidget->setItem(11,0,new QTableWidgetItem("Engine"));
	ui.connectionInfoTableWidget->hideRow(11);
	ui.connectionInfoTableWidget->setItem(12,0,new QTableWidgetItem("Service $05 - O2 Sensors"));
	ui.connectionInfoTableWidget->setItem(13,0,new QTableWidgetItem("Service $06 - On Board Monitoring"));
	ui.connectionInfoTableWidget->setItem(14,0,new QTableWidgetItem("Service $08 - In Use Performance Tracking"));
	ui.connectionInfoTableWidget->setItem(15,0,new QTableWidgetItem("Service $09 - Vehicle Information"));


	//Create a bunch of new items in the table, so we can just setText later, rather than having to allocate new items
	//This table never changes size so this is acceptable.
	for (int i=0;i<16;i++)
	{
		ui.connectionInfoTableWidget->setItem(i,1,new QTableWidgetItem());
	}

	//Checkbox selection of pids to request
	ui.pidSelectTableWidget->setRowCount(0);
	ui.pidSelectTableWidget->setColumnCount(3);
	ui.pidSelectTableWidget->setColumnWidth(0,100);
	ui.pidSelectTableWidget->setColumnWidth(1,200);
	ui.pidSelectTableWidget->setColumnWidth(2,50);
	ui.pidSelectTableWidget->setHorizontalHeaderLabels(QStringList() << "PID" << "Description" << "Priority");
	ui.pidSelectTableWidget->verticalHeader()->hide();

	//connect(ui.pidSelectTableWidget,SIGNAL(itemClicked(QTableWidgetItem*)))

	//Load com port and baud rate, set them in OBDThread and the connection summary table
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
	m_port = port;
	m_baud = baud;
	ui.connectionInfoTableWidget->item(0,1)->setText(port);
	ui.connectionInfoTableWidget->item(1,1)->setText(QString::number(baud));


	//QML Based gaugeview. Load qml from resource, or from a file.
	gaugeView = new QDeclarativeView(ui.gaugesTab);
	qmlRegisterType<GaugeItem>("GaugeImage",0,1,"GaugeImage");
	gaugeView->rootContext()->setContextProperty("propertyMap",&propertyMap);
	gaugeView->setGeometry(0,0,this->width()-5,this->height()-(ui.statusbar->height()+30));
	gaugeView->setSource(QUrl("gaugeview.qml"));
	gaugeView->show();

	//gaugeView->engine()->setProperty("propertyMap",propertyMap);


	//Add the list of lables for status output to the status bar.
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


	graph = new EGraph(ui.graphsTab);
	graph->setGeometry(0,0,800,600);
	graph->addGraph("RPM");
	graph->setMax(0,7000);
	graph->setMin(0,0);
	graph->addGraph("Speed");
	graph->setMax(1,255);
	m_graphPidMap["010D"] = 1;
	m_graphPidMap["010C"] = 0;
	graph->show();




	ui.nonconTableWidget->setColumnCount(3);
	ui.nonconTableWidget->setHorizontalHeaderLabels(QStringList() << "Non continuous monitoring tests" << "Supported" << "Completed");
	ui.nonconTableWidget->verticalHeader()->hide();
	ui.nonconTableWidget->setColumnWidth(0,200);
	ui.nonconTableWidget->setColumnWidth(1,75);
	ui.nonconTableWidget->setColumnWidth(2,75);

	ui.nonconTableWidget->setRowCount(8);
	ui.nonconTableWidget->setItem(0,0,new QTableWidgetItem("Catalyst"));
	ui.nonconTableWidget->setItem(1,0,new QTableWidgetItem("Heated catalyst"));
	ui.nonconTableWidget->setItem(2,0,new QTableWidgetItem("Evaporative system"));
	ui.nonconTableWidget->setItem(3,0,new QTableWidgetItem("Secondary air system"));
	ui.nonconTableWidget->setItem(4,0,new QTableWidgetItem("A/C System refridgerant"));
	ui.nonconTableWidget->setItem(5,0,new QTableWidgetItem("Oxygen sensor"));
	ui.nonconTableWidget->setItem(6,0,new QTableWidgetItem("Oxygen sensor heater"));
	ui.nonconTableWidget->setItem(7,0,new QTableWidgetItem("EGR System"));


	ui.troubleStoredTableWidget->setColumnCount(3);
	ui.troubleStoredTableWidget->setColumnWidth(0,100);
	ui.troubleStoredTableWidget->setColumnWidth(1,100);
	ui.troubleStoredTableWidget->setColumnWidth(2,500);
	ui.troubleStoredTableWidget->verticalHeader()->hide();
	ui.troubleStoredTableWidget->setHorizontalHeaderLabels(QStringList() << "ECU#" << "Code" << "Description");

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

	m_permConnect = false;


	/*ui.readinessTab->setEnabled(false);
	ui.onBoardMonitoringTab->setEnabled(false);
	ui.troubleCodesTab->setEnabled(false);
	ui.graphsTab->setEnabled(false);*/


	if (m_demoMode)
	{
	ui.tabWidget->removeTab(2);
	ui.tabWidget->removeTab(3);
	ui.tabWidget->removeTab(3);
	ui.tabWidget->removeTab(3);
	ui.tabWidget->removeTab(3);
	}
	m_demoPidList.append("0104");
	m_demoPidList.append("0105");
	m_demoPidList.append("0106");
	m_demoPidList.append("0107");
	m_demoPidList.append("0108");
	m_demoPidList.append("0109");
	m_demoPidList.append("010C");
	m_demoPidList.append("010D");
	m_demoPidList.append("010F");
	m_demoPidList.append("0110");
	m_demoPidList.append("0111");
	m_demoPidList.append("011F");

}
void MainWindow::graphScrollButtonStateChanged(int state)
{
	if (state == 0)
	{
		graph->setScrollWithData(false);
	}
	else if (state == 2)
	{
		graph->setScrollWithData(true);
	}
}

void MainWindow::graphZoomButtonStateChanged(int state)
{
	if (state == 0)
	{
		graph->setShowAllData(false);
	}
	else if (state == 2)
	{
		graph->setShowAllData(true);
	}
}

void MainWindow::uiTroubleClearClicked()
{
	obdThread->sendClearTroubleCodes();
}

void MainWindow::uiTroubleReadClicked()
{
	ui.troubleStoredTableWidget->setRowCount(0);
	obdThread->sendReqTroubleCodes();
}

void MainWindow::uiMonitorButtonClicked()
{
	obdThread->sendReqOnBoardMonitors();
}

void MainWindow::menu_actionExit()
{
	obdThread->disconnect();
	this->close();
}

void MainWindow::uiReadReadinessButtonClicked()
{
	obdThread->sendReqMonitorStatus();
}
void MainWindow::rawConsoleReturnPressed()
{
	//obdThread->
}
void MainWindow::menu_actionDisconnectClicked()
{
	obdThread->disconnect();
}
void MainWindow::obdRawCommLog(QString msg)
{
	ui.debugTextBrowser->append(msg.replace("\r","").replace("\n",""));
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
void MainWindow::obdOnBoardMonitoringReply(QList<unsigned char> midlist,QList<unsigned char> tidlist,QList<QString> vallist,QList<QString> minlist,QList<QString> maxlist,QList<QString> passlist)
{
	//qDebug() << "MainWindow::obdOnBoardMonitoringReply";
	ui.mode06TableWidget->setRowCount(midlist.size());
	ui.mode06TableWidget->setColumnCount(7);
	ui.mode06TableWidget->setColumnWidth(0,200);
	ui.mode06TableWidget->setColumnWidth(1,200);
	ui.mode06TableWidget->setColumnWidth(2,100);
	ui.mode06TableWidget->setColumnWidth(3,100);
	ui.mode06TableWidget->setColumnWidth(4,100);
	ui.mode06TableWidget->setColumnWidth(5,100);
	ui.mode06TableWidget->setColumnWidth(6,100);
	ui.mode06TableWidget->verticalHeader()->hide();
	ui.mode06TableWidget->setHorizontalHeaderLabels(QStringList() << "OBDMID" << "TID" << "Value" << "Min" << "Max" << "Passed");
	for (int i=0;i<midlist.size();i++)
	{
		ui.mode06TableWidget->setItem(i,0,new QTableWidgetItem(""));
		ui.mode06TableWidget->setItem(i,1,new QTableWidgetItem(""));
		ui.mode06TableWidget->setItem(i,2,new QTableWidgetItem(vallist[i]));
		ui.mode06TableWidget->setItem(i,3,new QTableWidgetItem(minlist[i]));
		ui.mode06TableWidget->setItem(i,4,new QTableWidgetItem(maxlist[i]));
		ui.mode06TableWidget->setItem(i,5,new QTableWidgetItem(""));
		ui.mode06TableWidget->setItem(i,6,new QTableWidgetItem(""));
		ObdInfo::ModeSixInfo midinfo = obdThread->getInfo()->getInfoFromByte(midlist[i]);
		ObdInfo::ModeSixInfo tidinfo = obdThread->getInfo()->getTestFromByte(tidlist[i]);
		qDebug() << QString::number(midlist[i],16) << QString::number(tidlist[i],16) << vallist[i] << minlist[i] << maxlist[i];
		//ui.mode06TableWidget->item(i,6)->setText(midinfo.);
		if (tidinfo.id == 0)
		{
			//qDebug() << midinfo.description << "MFG Test";
			ui.mode06TableWidget->item(i,0)->setText(QString::number(midlist[i],16) + ":" + midinfo.description);
			ui.mode06TableWidget->item(i,1)->setText(QString::number(tidlist[i],16) + ":" + "Manufacturer Test");
		}
		else
		{
			//qDebug() << midinfo.description << tidinfo.description;
			ui.mode06TableWidget->item(i,0)->setText(QString::number(midlist[i],16) + ":" + midinfo.description);
			ui.mode06TableWidget->item(i,1)->setText(QString::number(tidlist[i],16) + ":" + tidinfo.description);
		}
		ui.mode06TableWidget->item(i,6)->setText(passlist[i]);
	}
}
void MainWindow::clearReadPidsTable()
{
	m_readPidTableMap.clear();
	ui.readPidsTableWidget->clearContents();
	ui.readPidsTableWidget->setRowCount(0);
	QMap<QString, ObdThread::RequestClass>::const_iterator i = m_pidToReqClassMap.constBegin();
	while (i != m_pidToReqClassMap.constEnd())
	{
		obdThread->removeRequest(i.value());
	    //cout << i.key() << ": " << i.value() << endl;

	    ++i;
	}

	//m_pidToReqClassMap.clear();
}

void MainWindow::addReadPidRow(QString pid,int priority)
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
	ObdThread::RequestClass req;
	req.mode = p->mode;
	req.pid = p->pid;
	req.repeat = true;
	req.priority = priority;
	req.wait = 0;
	req.type = ObdThread::MODE_PID;
	obdThread->addRequest(req);
	m_pidToReqClassMap[pid] = req;

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
				//addReadPidRow(ui.pidSelectTableWidget->item(i,0)->text(),ui.pidSelectTableWidget->item(i,2)->text().toInt());
				//We no longer do this here. We only add rows when you hit save.
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

void MainWindow::obdMonitorStatus(QMap<ObdThread::CONTINUOUS_MONITOR,ObdThread::MONITOR_COMPLETE_STATUS> list)
{
	ui.conTableWidget->item(0,1)->setText(((list[ObdThread::MISFIRE] == ObdThread::UNAVAILABLE) ? "Unavailable" : ((list[ObdThread::MISFIRE] == ObdThread::COMPLETE) ? "Complete" : "Incomplete")));
	ui.conTableWidget->item(1,1)->setText(((list[ObdThread::FUEL_SYSTEM] == ObdThread::UNAVAILABLE) ? "Unavailable" : ((list[ObdThread::FUEL_SYSTEM] == ObdThread::COMPLETE) ? "Complete" : "Incomplete")));
	ui.conTableWidget->item(2,1)->setText(((list[ObdThread::COMPONENTS] == ObdThread::UNAVAILABLE) ? "Unavailable" : ((list[ObdThread::COMPONENTS] == ObdThread::COMPLETE) ? "Complete" : "Incomplete")));

	ui.nonconTableWidget->item(0,1)->setText(((list[ObdThread::CATALYST] == ObdThread::UNAVAILABLE) ? "Unavailable" : ((list[ObdThread::CATALYST] == ObdThread::COMPLETE) ? "Complete" : "Incomplete")));
	ui.nonconTableWidget->item(1,1)->setText(((list[ObdThread::HEATED_CATALYST] == ObdThread::UNAVAILABLE) ? "Unavailable" : ((list[ObdThread::HEATED_CATALYST] == ObdThread::COMPLETE) ? "Complete" : "Incomplete")));
	ui.nonconTableWidget->item(2,1)->setText(((list[ObdThread::EVAPORATIVE_SYSTEM] == ObdThread::UNAVAILABLE) ? "Unavailable" : ((list[ObdThread::EVAPORATIVE_SYSTEM] == ObdThread::COMPLETE) ? "Complete" : "Incomplete")));
	ui.nonconTableWidget->item(3,1)->setText(((list[ObdThread::SECONDARY_AIR_SYSTEM] == ObdThread::UNAVAILABLE) ? "Unavailable" : ((list[ObdThread::SECONDARY_AIR_SYSTEM] == ObdThread::COMPLETE) ? "Complete" : "Incomplete")));
	ui.nonconTableWidget->item(4,1)->setText(((list[ObdThread::AC_REFRIGERANT] == ObdThread::UNAVAILABLE) ? "Unavailable" : ((list[ObdThread::AC_REFRIGERANT] == ObdThread::COMPLETE) ? "Complete" : "Incomplete")));
	ui.nonconTableWidget->item(5,1)->setText(((list[ObdThread::OXYGEN_SENSOR] == ObdThread::UNAVAILABLE) ? "Unavailable" : ((list[ObdThread::OXYGEN_SENSOR] == ObdThread::COMPLETE) ? "Complete" : "Incomplete")));
	ui.nonconTableWidget->item(6,1)->setText(((list[ObdThread::OXYGEN_SENSOR_HEATER] == ObdThread::UNAVAILABLE) ? "Unavailable" : ((list[ObdThread::OXYGEN_SENSOR_HEATER] == ObdThread::COMPLETE) ? "Complete" : "Incomplete")));
	ui.nonconTableWidget->item(7,1)->setText(((list[ObdThread::EGR_SYSTEM] == ObdThread::UNAVAILABLE) ? "Unavailable" : ((list[ObdThread::EGR_SYSTEM] == ObdThread::COMPLETE) ? "Complete" : "Incomplete")));
}

MainWindow::~MainWindow()
{
}
void MainWindow::menu_settingsClicked()
{
	settingsWidget = new SettingsWidget();
	connect(settingsWidget,SIGNAL(saveSettings(QString,int)),this,SLOT(settings_saveComPort(QString,int)));
	settingsWidget->setSettings(m_port,m_baud);
	settingsWidget->show();
}
void MainWindow::obdSupportedPids(QList<QString> pidlist)
{
	ui.pidSelectTableWidget->clearContents();
	ui.pidSelectTableWidget->setRowCount(0);
	for (int i=0;i<pidlist.size();i++)
	{
		if (m_demoMode)
		{
			if (!m_demoPidList.contains(pidlist[i]))
			{
				continue;
			}
		}
		ui.pidSelectTableWidget->setRowCount(ui.pidSelectTableWidget->rowCount()+1);
		ObdInfo::Pid *p = obdThread->getInfo()->getPidFromString(pidlist[i]);
		if (p)
		{
			ui.pidSelectTableWidget->setItem(ui.pidSelectTableWidget->rowCount()-1,0,new QTableWidgetItem(pidlist[i]));
			if (!p->bitencoded)
			{
			ui.pidSelectTableWidget->item(ui.pidSelectTableWidget->rowCount()-1,0)->setCheckState(Qt::Checked);
			}

			if (obdThread->getInfo()->getPidFromString(pidlist[i]))
			{
				ui.pidSelectTableWidget->setItem(ui.pidSelectTableWidget->rowCount()-1,1,new QTableWidgetItem(p->description));
				ui.pidSelectTableWidget->setItem(ui.pidSelectTableWidget->rowCount()-1,2,new QTableWidgetItem("1"));
			}
		}
		else
		{
			ui.pidSelectTableWidget->setItem(ui.pidSelectTableWidget->rowCount()-1,0,new QTableWidgetItem(pidlist[i]));
			ui.pidSelectTableWidget->setItem(ui.pidSelectTableWidget->rowCount()-1,1,new QTableWidgetItem("INVALID"));
			ui.pidSelectTableWidget->setItem(ui.pidSelectTableWidget->rowCount()-1,2,new QTableWidgetItem("1"));
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
	m_port = port;
	m_baud = baud;
	obdThread->setPort(port);
	obdThread->setBaud(baud);
	ui.status_comPortLabel->setText("Com port: " + port);
	ui.status_comBaudLabel->setText("Baud Rate: " + QString::number(baud));
	ui.connectionInfoTableWidget->item(0,1)->setText(port);
	ui.connectionInfoTableWidget->item(1,1)->setText(QString::number(baud));
	settingsWidget->deleteLater();
}
void MainWindow::uiPidSelectClearClicked()
{
	for (int i=0;i<ui.pidSelectTableWidget->rowCount();i++)
	{
		if (ui.pidSelectTableWidget->item(i,0)->checkState() == Qt::Checked)
		{
		ui.pidSelectTableWidget->item(i,0)->setCheckState(Qt::Unchecked);
		}
	}
}

void MainWindow::uiPidSelectSaveClicked()
{
	clearReadPidsTable();
	for (int i=0;i<ui.pidSelectTableWidget->rowCount();i++)
	{
		if (ui.pidSelectTableWidget->item(i,0)->checkState() == Qt::Checked)
		{
			addReadPidRow(ui.pidSelectTableWidget->item(i,0)->text(),ui.pidSelectTableWidget->item(i,2)->text().toInt());
		}
	}
}

void MainWindow::obdPidReceived(QString pid,QString val,int set, double time)
{
	//qDebug() << pid << val;
	propertyMap.setProperty(pid.toAscii(),val);
	//0105_DURATION
	if (m_pidTimeMap.contains(pid))
	{
		double newtime = QDateTime::currentMSecsSinceEpoch() - m_pidTimeMap[pid];
		//qDebug() << pid << val << set << QString::number(QDateTime::currentMSecsSinceEpoch(),'f') << newtime << QDateTime::currentMSecsSinceEpoch();
		propertyMap.setProperty(QString(pid + "_DURATION").toAscii(),newtime);
	}
	m_pidTimeMap[pid] = QDateTime::currentMSecsSinceEpoch();

	if (m_readPidTableMap.contains(pid))
	{
		m_readPidTableMap[pid]->setText(val);
	}
	if (m_graphPidMap.contains(pid))
	{
		graph->addValue(m_graphPidMap[pid],val.toDouble());
	}
	m_pidcount++;
}
void MainWindow::obdConnected(QString version)
{
	ui.connectionInfoTableWidget->item(3,1)->setText("Connected");
	ui.connectionInfoTableWidget->item(4,1)->setText(version);
	//obdThread->switchBaud();
	//obdThread->sendReqOnBoardMonitors();
	if (m_permConnect)
	{
		obdThread->sendReqVoltage();
		obdThread->sendReqSupportedModes();
		obdThread->sendReqSupportedPids();
		obdThread->sendReqMfgString();
	}
	ui.status_comStatusLabel->setText("Status: Connected");
	ui.status_comInterfaceLabel->setText("Interface: " + version);
	//ui.status_comBaudLabel->setText()
}
void MainWindow::menu_actionConnectClicked()
{
	m_permConnect = true;
	obdThread->connect();
	ui.status_comStatusLabel->setText("Status: Connecting");
}
void MainWindow::obdDisconnected()
{
	ui.connectionInfoTableWidget->item(3,1)->setText("Disconnected");
}

void MainWindow::obdConsoleMessage(QString message)
{
	ui.debugTextBrowser->append(message);
}

void MainWindow::obdTroubleCodes(QString ecu,QList<QString> codes)
{
	qDebug() << "Codes:" << codes.size();
	if (codes.size() > 0)
	{
		ui.connectionInfoTableWidget->item(9,1)->setText("Illuminated");
		ui.connectionInfoTableWidget->item(10,1)->setText(QString::number(codes.size()));
		int counter = ui.troubleStoredTableWidget->rowCount();
		ui.troubleStoredTableWidget->setRowCount(counter + codes.size());
		for (int i=0;i<codes.size();i++)
		{
			ui.troubleStoredTableWidget->setItem(counter+i,0,new QTableWidgetItem(ecu));
			ui.troubleStoredTableWidget->setItem(counter+i,1,new QTableWidgetItem(codes[i]));
		}
	}
	else
	{
		ui.troubleStoredTableWidget->setRowCount(1);
		ui.troubleStoredTableWidget->setItem(0,0,new QTableWidgetItem());
		ui.troubleStoredTableWidget->setItem(0,1,new QTableWidgetItem("No Codes"));
		ui.connectionInfoTableWidget->item(9,1)->setText("Clear");
		ui.connectionInfoTableWidget->item(10,1)->setText(0);
	}
}
void MainWindow::obdError(ObdThread::ObdError err)
{
	qDebug() << "Error";
	if (err == ObdThread::UNABLE_TO_OPEN_COM_PORT)
	{
		ui.status_comStatusLabel->setText("Status: Unable to open Com Port");
	}
}
