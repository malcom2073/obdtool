
#include <QApplication>
#include "qmlwindow.h"
#include "mainwindow.h"

int main(int argc, char **argv)
{
	QApplication a(argc,argv);
	int raw = 0x7FFF;
	double output = ((0xFFFF - raw) + 1) * -1;
	qDebug() << output;
	output *= 0.00390625;
	qDebug() << QString::number(output,'f');
	//return 0;
	if (argc >= 2)
	{
		if (QString(argv[1]) == QString("qml"))
		{
			QmlWindow q;
			q.setGeometry(0,0,1280,800);
			q.show();
			return a.exec();
		}
	}
	MainWindow m;
	m.setGeometry(100,100,1280,800);
	m.show();
	return a.exec();
}
