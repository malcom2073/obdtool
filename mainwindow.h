#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "ui_mainwindow.h"

#include <QMap>
#include <QMainWindow>
#include <ObdThread.h>
#include <QDeclarativeView>
#include <QDeclarativePropertyMap>
#include <QTimer>
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
	void clearReadPidsTable();
	QMap<QString,ObdThread::RequestClass> m_pidToReqClassMap;
protected:
	void resizeEvent(QResizeEvent *evt);
	void changeEvent(QEvent *evt);
private slots:
	void menu_actionDisconnectClicked();
	void uiPidSelectSaveClicked();
	void rawConsoleReturnPressed();
	void uiPidSelectClearClicked();
	void uiReadReadinessButtonClicked();
	void obdMonitorStatus(QList<QString> list);
	void pidsPerSecondTimerTick();
	void obdSupportedModes(QList<QString> list);
	void obdVoltage(double volts);
	void obdMfgString(QString string);
	void obdError(ObdThread::ObdError err);
        void connectButtonClicked();
	void menu_actionConnectClicked();
	void uiPidSelectTableClicked(int row, int column);
	void obdSupportedPids(QList<QString> pidlist);
	void menu_settingsClicked();
	void settings_saveComPort(QString port,int baud);
	void obdPidReceived(QString pid,QString val,int set, double time);
	void obdTroubleCodes(QList<QString> codes);
	void obdConsoleMessage(QString message);
	void obdConnected(QString version);
	void obdProtocolFound(QString protocol);
	void obdDisconnected();
	void obdSingleShotReply(QByteArray req,QByteArray reply);
	void obdOnBoardMonitoringReply(QList<unsigned char> midlist,QList<unsigned char> tidlist,QList<QString> vallist,QList<QString> minlist,QList<QString> maxlist);

};
#endif //MAINWINDOW_H
