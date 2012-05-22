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

#include "settingswidget.h"

SettingsWidget::SettingsWidget(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);
    connect(ui.savePushButton,SIGNAL(clicked()),this,SLOT(saveButtonClicked()));
    connect(ui.cancelPushButton,SIGNAL(clicked()),this,SLOT(cancelButtonClicked()));
}

SettingsWidget::~SettingsWidget()
{

}
void SettingsWidget::saveButtonClicked()
{
	QString port = ui.comPortLineEdit->text();
	int baud = ui.baudRateLineEdit->text().toInt();
	saveSettings(port,baud);
	this->hide();
}
void SettingsWidget::setSettings(QString comport,int baud)
{
	ui.comPortLineEdit->setText(comport);
	ui.baudRateLineEdit->setText(QString::number(baud));
}

void SettingsWidget::cancelButtonClicked()
{
	this->hide();
}
