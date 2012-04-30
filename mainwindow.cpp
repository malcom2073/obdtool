#include "mainwindow.h"
#include <QSettings>
#include <QDeclarativeItem>
#include "gaugeitem.h"
#include <QDeclarativeContext>
#include <QFileDialog>
#include <qjson/serializer.h>
MainWindow::MainWindow() : QMainWindow()
{
	ui.setupUi(this);
	m_canDispStyle = 2;
	m_demoMode = false;

	ui.canStyleComboBox->addItem("Sort by ParamID");
	ui.canStyleComboBox->addItem("Sort by Source");
	ui.canStyleComboBox->addItem("Sort by Param, Overwrite and highlight changes");
	ui.canStyleComboBox->setCurrentIndex(2);

	if (m_canDispStyle == 0)
	{
		ui.canMsgTableWidget->setRowCount(1);
	}
	else if (m_canDispStyle == 1)
	{
		ui.canMsgTableWidget->setRowCount(1);
	}
	else if (m_canDispStyle == 2)
	{
		ui.canMsgTableWidget->setColumnCount(8);
		ui.canMsgTableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem("CANID"));
		ui.canMsgTableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem("SOURCE"));
		ui.canMsgTableWidget->setHorizontalHeaderItem(2,new QTableWidgetItem("BYTE 0"));
		ui.canMsgTableWidget->setHorizontalHeaderItem(3,new QTableWidgetItem("BYTE 1"));
		ui.canMsgTableWidget->setHorizontalHeaderItem(4,new QTableWidgetItem("BYTE 2"));
		ui.canMsgTableWidget->setHorizontalHeaderItem(5,new QTableWidgetItem("BYTE 3"));
		ui.canMsgTableWidget->setHorizontalHeaderItem(6,new QTableWidgetItem("BYTE 4"));
		ui.canMsgTableWidget->setHorizontalHeaderItem(7,new QTableWidgetItem("BYTE 5"));
	}
	//Setup all the UI signals, connecting them to slots.
	connect(ui.actionSettings,SIGNAL(triggered()),this,SLOT(menu_settingsClicked()));
	connect(ui.actionConnect,SIGNAL(triggered()),this,SLOT(menu_actionConnectClicked()));
	connect(ui.readReadinessPushButton,SIGNAL(clicked()),this,SLOT(uiReadReadinessButtonClicked()));
	connect(ui.rawConsoleLineEdit,SIGNAL(returnPressed()),this,SLOT(rawConsoleReturnPressed()));
	//connect(ui.connectPushButton,SIGNAL(clicked()),this,SLOT(connectButtonClicked()));
	connect(ui.canSaveLogFileBrowseButton,SIGNAL(clicked()),this,SLOT(uiCanSaveLogFileBrowseClicked()));

	connect(ui.pidSelectNonePushButton,SIGNAL(clicked()),this,SLOT(uiPidSelectClearClicked()));
	connect(ui.pidSelectSavePushButton,SIGNAL(clicked()),this,SLOT(uiPidSelectSaveClicked()));
	connect(ui.pidSelectAllPushButton,SIGNAL(clicked()),this,SLOT(uiPidSelectAllClicked()));

	connect(ui.canStyleComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(uiCanStyleChanged(int)));

	connect(ui.connectPushButton,SIGNAL(clicked()),this,SLOT(menu_actionConnectClicked()));
	connect(ui.disconnectPushButton,SIGNAL(clicked()),this,SLOT(menu_actionDisconnectClicked()));

	connect(ui.action_Exit,SIGNAL(triggered()),this,SLOT(menu_actionExit()));
	connect(ui.monitorPushButton,SIGNAL(clicked()),this,SLOT(uiMonitorButtonClicked()));
	connect(ui.troubleReadPushButton,SIGNAL(clicked()),this,SLOT(uiTroubleReadClicked()));
	connect(ui.troubleClearPushButton,SIGNAL(clicked()),this,SLOT(uiTroubleClearClicked()));
	connect(ui.graphScrollCheckBox,SIGNAL(stateChanged(int)),this,SLOT(graphScrollButtonStateChanged(int)));
	connect(ui.graphZoomCheckBox,SIGNAL(stateChanged(int)),this,SLOT(graphZoomButtonStateChanged(int)));
	connect(ui.canMonitorStartPushButton,SIGNAL(clicked()),this,SLOT(uiStartMonitorClicked()));
	connect(ui.canMonitorStopPushButton,SIGNAL(clicked()),this,SLOT(uiStopMonitorClicked()));


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
	QObject::connect(obdThread,SIGNAL(monitorModeLine(QByteArray)),this,SLOT(obdMonitorModeLine(QByteArray)));

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

	ui.canProtocolComboBox->addItem("11:J1850 PWM");
	ui.canProtocolComboBox->addItem("12:J1850 VPW");

	ui.canProtocolComboBox->addItem("21:ISO 9141 (no header no autoinit)");
	ui.canProtocolComboBox->addItem("22:ISO 9141-2 (5 baud autoinit)");
	ui.canProtocolComboBox->addItem("23:ISO 14230 (no autoinit)");
	ui.canProtocolComboBox->addItem("24:ISO 14230 (5 baud autoinit)");
	ui.canProtocolComboBox->addItem("25:ISO 14230 (fast autoinit)");

	ui.canProtocolComboBox->addItem("31:HS CAN (ISO 11898, 11-bit Tx, 500kbps, var DLC)");
	ui.canProtocolComboBox->addItem("32:HS CAN (ISO 11898, 29-bit Tx, 500kbps, var DLC)");

	ui.canProtocolComboBox->addItem("33:HS CAN (ISO 15765, 11-bit Tx, 500kbps, DLC=8");
	ui.canProtocolComboBox->addItem("34:HS CAN (ISO 15765, 29-bit Tx, 500kbps, DLC=8");
	ui.canProtocolComboBox->addItem("35:HS CAN (ISO 15765, 11-bit Tx, 250kbps, DLC=8");
	ui.canProtocolComboBox->addItem("36:HS CAN (ISO 15765, 29-bit Tx, 250kbps, DLC=8");

	ui.canProtocolComboBox->addItem("41:J1939 (11-bit Tx)");
	ui.canProtocolComboBox->addItem("42:J1939 (29-bit Tx)");

	ui.canProtocolComboBox->addItem("51:MS CAN (ISO 11898, 11-bit Tx, 125kbps, var DLC)");
	ui.canProtocolComboBox->addItem("52:MS CAN (ISO 11898, 29-bit Tx, 125kbps, var DLC)");
	ui.canProtocolComboBox->addItem("53:MS CAN (ISO 15765, 11-bit Tx, 125kbps, DLC=8)");
	ui.canProtocolComboBox->addItem("54:MS CAN (ISO 15765, 29-bit Tx, 125kbps, DLC=8)");

	ui.canProtocolComboBox->addItem("61:SW CAN (ISO 11898, 11-bit Tx, 33.3kbps, var DLC)");
	ui.canProtocolComboBox->addItem("62:SW CAN (ISO 11898, 29-bit Tx, 33.3kbps, var DLC)");
	ui.canProtocolComboBox->addItem("63:SW CAN (ISO 15765, 11-bit Tx, 33.3kbps, DLC=8)");
	ui.canProtocolComboBox->addItem("64:SW CAN (ISO 15765, 29-bit Tx, 33.3kbps, DLC=8)");


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
void MainWindow::uiCanStyleChanged(int index)
{
	m_canDispStyle = index;
	ui.canMsgTableWidget->clear();
	ui.canMsgTableWidget->setColumnCount(0);
	if (m_canDispStyle == 0)
	{
		ui.canMsgTableWidget->setRowCount(1);
	}
	else if (m_canDispStyle == 1)
	{
		ui.canMsgTableWidget->setRowCount(1);
	}
	else if (m_canDispStyle == 2)
	{
		ui.canMsgTableWidget->setRowCount(0);
		ui.canMsgTableWidget->setColumnCount(8);
		ui.canMsgTableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem("CANID"));
		ui.canMsgTableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem("SOURCE"));
		ui.canMsgTableWidget->setHorizontalHeaderItem(2,new QTableWidgetItem("BYTE 0"));
		ui.canMsgTableWidget->setHorizontalHeaderItem(3,new QTableWidgetItem("BYTE 1"));
		ui.canMsgTableWidget->setHorizontalHeaderItem(4,new QTableWidgetItem("BYTE 2"));
		ui.canMsgTableWidget->setHorizontalHeaderItem(5,new QTableWidgetItem("BYTE 3"));
		ui.canMsgTableWidget->setHorizontalHeaderItem(6,new QTableWidgetItem("BYTE 4"));
		ui.canMsgTableWidget->setHorizontalHeaderItem(7,new QTableWidgetItem("BYTE 5"));
	}

}

void MainWindow::uiStartMonitorClicked()
{
	m_canMsgCount = 0;
	QString file = ui.canSaveLogFileLineEdit->text();
	if (file != "")
	{
		m_canLogFile = new QFile(file);
		m_canLogFile->open(QIODevice::ReadWrite | QIODevice::Append);
	}
	else
	{
		m_canLogFile = 0;
	}
	obdThread->setEcho(false);
	obdThread->setLineFeed(false);
	obdThread->MX_setProtocol(ui.canProtocolComboBox->currentText().split(":")[0].toInt());
	obdThread->startMonitorMode();
}

void MainWindow::uiStopMonitorClicked()
{
	obdThread->stopMonitorMode();
}
void MainWindow::uiCanSaveLogFileBrowseClicked()
{
	QFileDialog dialog;
	if (dialog.exec())
	{
		if (dialog.selectedFiles().size() > 0)
		{
			ui.canLoadLogFileLineEdit->setText(dialog.selectedFiles()[0]);
		}
	}
}

void MainWindow::obdMonitorModeLine(QByteArray buf)
{
	m_canMsgCount++;
	ui.canMsgLabel->setText("Messages: " + QString::number(m_canMsgCount));
	QJson::Serializer ser;
	if (m_canLogFile)
	{
		QVariantMap map;
		map["data"] = buf;
		map["direction"] = "in";
		map["mode"] = "11";
		map["time"] = QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch() / 1000.0,'f');
		m_canLogFile->write(ser.serialize(map).append("\n"));
	}
	/*if (!m_display)
	{
		return;
	}*/
	/*buf.clear();
	buf.append("10");
	buf.append("0A");
	buf.append("C0");
	buf.append("97");
	buf.append("45");*/
	QString strbuf = buf;
	QString msg = buf.replace(" ","").replace("\r","").replace("\n","");
	qDebug() << msg;
	QByteArray bytes;
	QString comp = "";
	QString newmsg = "";
	bool is11bit = false;
	int sourceid = 0;
	if (strbuf.split(" ")[0].size() == 2)
	{
		qDebug() << "29bit";
		for (int i=0;i<buf.size()-1;i+=2)
		{
			if (buf[i] != (char)32 && buf[i] != '\r' && buf[i] != '\n')
			{
				bytes.append(obdLib::byteArrayToByte(buf[i],buf[i+1]));
			}
		}
		//int priority = ((bytes[0] >> 2) & 0x7);
		int paramA = ((bytes[1] >> 1) & 0xF);
		int paramB = (((bytes[1]) & 0x1) << 3) + ((bytes[2] >> 5) & 0x7);
		int paramid = (paramA << 4) + paramB;
		sourceid = (unsigned char)bytes[3];
		comp = ((sourceid <= 0x0F) ? "0" : "") + QString::number(sourceid,16).toUpper() + ":" + ((paramid <= 0x0F) ? "0" : "") + QString::number(paramid);
		//XX:YY XX - sourceid YY - paramid
		//newmsg += ((sourceid <= 0x0F) ? "0" : "") + QString::number(sourceid,16).toUpper() + ":";
		for (int i=4;i<bytes.size();i++)
		{
			newmsg += (((unsigned char)bytes[i] <=0xF) ? "0" : "") + QString::number((unsigned char)bytes[i],16).toUpper();
		}
	}
	else if (strbuf.split(" ")[0].size() == 3)
	{
		is11bit = true;
		qDebug() << "11bit";
		for (int i=3;i<buf.size()-1;i+=2)
		{
			if (buf[i] != (char)32 && buf[i] != '\r' && buf[i] != '\n')
			{
				bytes.append(obdLib::byteArrayToByte(buf[i],buf[i+1]));
			}
		}
		unsigned char second = obdLib::byteArrayToByte(buf[1],buf[2]);
		unsigned char first = obdLib::byteArrayToByte('0',buf[0]);
		unsigned int final = (first << 8) + second;
		qDebug() << "Header:" << QString::number(final,16);
		comp = strbuf.mid(0,strbuf.indexOf(" "));
		for (int i=0;i<bytes.size();i++)
		{
			newmsg += (((unsigned char)bytes[i] <=0xF) ? "0" : "") + QString::number((unsigned char)bytes[i],16).toUpper();
		}
	}
	else
	{
		qDebug() << "WEEEE";
	}
	if (bytes.length() <= 3)
	{
		return;
	}


	qDebug() << "Compare:" << comp;
	//Style 1


	if (m_canDispStyle == 2)
	{
		newmsg = newmsg.mid(3);
/*ui.canMsgTableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem("CANID"));
  ui.canMsgTableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem("SOURCE"));
  ui.canMsgTableWidget->setHorizontalHeaderItem(2,new QTableWidgetItem("BYTE 0"));
  ui.canMsgTableWidget->setHorizontalHeaderItem(3,new QTableWidgetItem("BYTE 1"));
  ui.canMsgTableWidget->setHorizontalHeaderItem(4,new QTableWidgetItem("BYTE 2"));
  ui.canMsgTableWidget->setHorizontalHeaderItem(5,new QTableWidgetItem("BYTE 3"));
  ui.canMsgTableWidget->setHorizontalHeaderItem(6,new QTableWidgetItem("BYTE 4"));
  ui.canMsgTableWidget->setHorizontalHeaderItem(7,new QTableWidgetItem("BYTE 5"));*/
		int foundi = 0;
		bool found = false;
		for (int i=0;i<ui.canMsgTableWidget->rowCount();i++)
		{
			if (ui.canMsgTableWidget->item(i,0))
			{
				if (ui.canMsgTableWidget->item(i,0)->text() == comp)
				{
					found = true;
					foundi = i;
					break;
				}
			}
		}
		if (!found)
		{
			ui.canMsgTableWidget->setRowCount(ui.canMsgTableWidget->rowCount()+1);
			ui.canMsgTableWidget->setItem(ui.canMsgTableWidget->rowCount()-1,0,new QTableWidgetItem(comp));
			if (!is11bit)
			{
				QString sourceidstr = (((unsigned char)sourceid <=0xF) ? "0" : "") + QString::number((unsigned char)sourceid,16).toUpper();
				ui.canMsgTableWidget->setItem(ui.canMsgTableWidget->rowCount()-1,1,new QTableWidgetItem(sourceidstr));
			}
			for (int i=0;i<10;i+=2)
			{
				if (i < newmsg.size())
				{
					ui.canMsgTableWidget->setItem(ui.canMsgTableWidget->rowCount()-1,2+(i/2),new QTableWidgetItem(newmsg.mid(i,2)));
				}
				else
				{
					ui.canMsgTableWidget->setItem(ui.canMsgTableWidget->rowCount()-1,2+(i/2),new QTableWidgetItem(""));
				}
			}
		}
		else
		{
			for (int i=0;i<10;i+=2)
			{
				if (i < newmsg.size())
				{
				if (ui.canMsgTableWidget->item(foundi,2+(i/2))->text() == newmsg.mid(i,2))
				{
				}
				else
				{
					ui.canMsgTableWidget->setItem(foundi,2+(i/2),new QTableWidgetItem(newmsg.mid(i,2)));
					ui.canMsgTableWidget->item(foundi,2+(i/2))->setBackgroundColor(QColor::fromRgb(255,0,0));
				}
			}
			}
		}
	}
	else if (m_canDispStyle == 0 || m_canDispStyle == 1)
	{
		bool found = false;
		int foundi = -1;
		QString newcomp = "";
		if (comp.indexOf(":") != -1)
		{
			if (m_canDispStyle == 0)
			{
				newcomp = comp.split(":")[0];
			}
			else if (m_canDispStyle == 1)
			{
				newcomp = comp.split(":")[1];
			}
		}
		else
		{
			newcomp = comp;
		}
		for (int i=0;i<ui.canMsgTableWidget->columnCount();i++)
		{
			if (ui.canMsgTableWidget->horizontalHeaderItem(i)->text() == newcomp)
			{
				found = true;
				foundi = i;
				break;
			}

		}
		if (!found)
		{
			ui.canMsgTableWidget->setColumnCount(ui.canMsgTableWidget->columnCount()+1);
			ui.canMsgTableWidget->setColumnWidth(ui.canMsgTableWidget->columnCount()-1,150);
			ui.canMsgTableWidget->setHorizontalHeaderItem(ui.canMsgTableWidget->columnCount()-1,new QTableWidgetItem(newcomp));
			foundi = ui.canMsgTableWidget->columnCount()-1;
			qDebug() << "New Column:" << newcomp;
		}
		found = false;
		if (!ui.canMsgTableWidget->item(ui.canMsgTableWidget->rowCount()-1,foundi))
		{
			if (newmsg.length() > 0)
			{
				ui.canMsgTableWidget->setItem(ui.canMsgTableWidget->rowCount()-1,foundi,new QTableWidgetItem(newmsg));
			}
		}
		else
		{
			if (newmsg.length() > 0)
			{
				ui.canMsgTableWidget->setRowCount(ui.canMsgTableWidget->rowCount()+1);
				ui.canMsgTableWidget->setItem(ui.canMsgTableWidget->rowCount()-1,foundi,new QTableWidgetItem(newmsg));
			}
		}
	}
	/*for (int i=0;i<ui.tableWidget->rowCount();i++)
	{
		if (!ui.tableWidget->item(i,foundi))
		{
			if (newmsg.length() > 0)
			{
				ui.tableWidget->setItem(i,foundi,new QTableWidgetItem(newmsg));
				found = true;
			}
		}
	}
	if (!found)
	{
		if (newmsg > 0)
		{
			ui.tableWidget->setRowCount(ui.tableWidget->rowCount()+1);
			ui.tableWidget->setItem(ui.tableWidget->rowCount()-1,foundi,new QTableWidgetItem(newmsg));
		}
	}*/
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
	qDebug() << "Raw:" << msg;
}

void MainWindow::connectButtonClicked()
{
	obdThread->connect();
}

void MainWindow::changeEvent(QEvent *evt)
{
	Q_UNUSED(evt)
	/*ui.tabWidget->setGeometry(0,0,this->width(),this->height()-(ui.statusbar->height()+20));
	ui.connectionInfoTableWidget->setGeometry(0,0,this->width()-5,this->height()-(ui.statusbar->height()+70));
	ui.pidSelectTableWidget->setGeometry(0,0,this->width()-5,this->height()-(ui.statusbar->height()+45));
	ui.readPidsTableWidget->setGeometry(0,0,this->width()-5,this->height()-(ui.statusbar->height()+45));*/
	//QMainWindow:changeEvent(evt);
}
void MainWindow::pidsPerSecondTimerTick()
{
	ui.status_pidUpdateRateLabel->setText("Pid Rate: " + QString::number(m_pidcount) +" pids per second");
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
	Q_UNUSED(evt)
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
	Q_UNUSED(row);
	Q_UNUSED(column);
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
	obdThread->stopThread();
	//obdThread->terminate();
	//obdThread->exit();
	obdThread->wait(5000);
	delete obdThread;
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
void MainWindow::uiPidSelectAllClicked()
{
	for (int i=0;i<ui.pidSelectTableWidget->rowCount();i++)
	{
		if (ui.pidSelectTableWidget->item(i,0))
		{
			ui.pidSelectTableWidget->item(i,0)->setCheckState(Qt::Checked);
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
	Q_UNUSED(set);
	Q_UNUSED(time);
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

void MainWindow::on_canClearRedPushButton_clicked()
{
	for (int i=0;i<ui.canMsgTableWidget->rowCount();i++)
	{
		for (int j=0;j<ui.canMsgTableWidget->columnCount();j++)
		{
			if (ui.canMsgTableWidget->item(i,j))
			{
				ui.canMsgTableWidget->item(i,j)->setBackgroundColor(QColor::fromRgb(255,255,255));
			}
		}
	}
}
