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
