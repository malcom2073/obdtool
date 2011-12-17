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
