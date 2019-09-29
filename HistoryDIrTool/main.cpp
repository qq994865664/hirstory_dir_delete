#include "HistoryDIrTool.h"
#include <QtWidgets/QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	HistoryDIrTool w;
	w.show();
	return a.exec();
}
