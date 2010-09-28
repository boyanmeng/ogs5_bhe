#include <QtGui/QApplication>
#include "mainwindow.h"
#include "Configure.h"
#include <OpenSG/OSGBaseFunctions.h>

int main(int argc, char *argv[])
{
	#ifdef OGS_USE_OPENSG
		OSG::osgInit(argc,argv);
	#endif

	QApplication a(argc, argv);
	setlocale(LC_NUMERIC,"C");
	MainWindow w;
	w.show();
	return a.exec();
}
