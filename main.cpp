
#include <QApplication>
#include "qmlwindow.h"
#include "mainwindow.h"

int main(int argc, char **argv)

{
	QApplication a(argc,argv);
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
