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
