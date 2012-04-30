#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "ui_mainwindow.h"

#include <QMap>
#include <QMainWindow>
#include <ObdThread.h>
#include <QDeclarativeView>
#include <QDeclarativePropertyMap>
#include <QTimer>
#include <QFile>
#include "gaugewidget.h"
#include "egraph.h"
#include "settingswidget.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow();
	~MainWindow();
	/*Q_INVOKABLE void connect();
	Q_INVOKABLE void disconnect();
	Q_INVOKABLE void getTroubleCodes();
	Q_INVOKABLE void clearTroubleCodes();
	Q_INVOKABLE QString toUpper(QString string);
	Q_INVOKABLE QString toLower(QString string);
	Q_INVOKABLE QString mid(QString string,int start, int len);
	Q_INVOKABLE void saveComPort(QString port,int baud);
	*/
private:
	EGraph *graph;
	QMap<QString,int> m_graphPidMap;
	SettingsWidget *settingsWidget;
	ObdThread *obdThread;
	QMap<QString,double> m_pidTimeMap;
	/*QDeclarativeContext *context;
	ObdThread *obdThread;

	QStringList m_troubleCodeList;
	QMap<QString,double> m_pidTimeMap;
	*/
	Ui::MainWindow ui;
	QMap<QString,QTableWidgetItem*> m_readPidTableMap;
	void addReadPidRow(QString pid,int priority);
	QList<GaugeWidget*> m_gaugeWidgetList;
	QDeclarativePropertyMap propertyMap;
	QDeclarativeView *gaugeView;
	QTimer *pidsPerSecondTimer;
	int m_pidcount;
	QString m_port;
	int m_baud;
	void clearReadPidsTable();
	QMap<QString,ObdThread::RequestClass> m_pidToReqClassMap;
	bool m_permConnect;
	QList<QString> m_demoPidList;
	bool m_demoMode;
	int m_canDispStyle;
	QFile *m_canLogFile;
	int m_canMsgCount;
protected:
	void resizeEvent(QResizeEvent *evt);
	void changeEvent(QEvent *evt);
private slots:

	void graphScrollButtonStateChanged(int state);
	void graphZoomButtonStateChanged(int state);
	void obdRawCommLog(QString msg);
	void uiTroubleReadClicked();
	void uiTroubleClearClicked();
	void menu_actionExit();
	void menu_actionDisconnectClicked();
	void uiMonitorButtonClicked();
	void uiPidSelectSaveClicked();
	void rawConsoleReturnPressed();
	void uiCanSaveLogFileBrowseClicked();
	void uiPidSelectClearClicked();
	void uiPidSelectAllClicked();
	void uiReadReadinessButtonClicked();
	//void obdMonitorStatus(QList<QString> list);
	void obdMonitorStatus(QMap<ObdThread::CONTINUOUS_MONITOR,ObdThread::MONITOR_COMPLETE_STATUS> list);
	void pidsPerSecondTimerTick();
	void obdSupportedModes(QList<QString> list);
	void obdVoltage(double volts);
	void obdMfgString(QString string);
	void obdError(ObdThread::ObdError err);
        void connectButtonClicked();
	void menu_actionConnectClicked();
	void menu_settingsClicked();

	void uiPidSelectTableClicked(int row, int column);
	void uiCanStyleChanged(int index);
	void uiStartMonitorClicked();
	void uiStopMonitorClicked();
	void obdMonitorModeLine(QByteArray line);
	void obdSupportedPids(QList<QString> pidlist);
	void settings_saveComPort(QString port,int baud);
	void obdPidReceived(QString pid,QString val,int set, double time);
	void obdTroubleCodes(QString ecu,QList<QString> codes);
	void obdConsoleMessage(QString message);
	void obdConnected(QString version);
	void obdProtocolFound(QString protocol);
	void obdDisconnected();
	void obdSingleShotReply(QByteArray req,QByteArray reply);
	void obdOnBoardMonitoringReply(QList<unsigned char> midlist,QList<unsigned char> tidlist,QList<QString> vallist,QList<QString> minlist,QList<QString> maxlist,QList<QString> passlist);

	void on_canClearRedPushButton_clicked();
};
#endif //MAINWINDOW_H
