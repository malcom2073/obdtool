#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>
#include "ui_settingswidget.h"
class SettingsWidget : public QWidget
{
	Q_OBJECT

public:
	SettingsWidget(QWidget *parent = 0);
	void setSettings(QString comport,int baud);
	~SettingsWidget();
signals:
	void saveSettings(QString comport,int baud);
private:
	Ui::SettingsWidget ui;
private slots:
	void saveButtonClicked();
	void cancelButtonClicked();
};

#endif // SETTINGSWIDGET_H
