#include "HistoryDIrTool.h"
#include <QtWidgets/QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	HistoryDIrTool w;
	w.setWindowIcon(QIcon(":/HistoryDIrTool/Resources/tools.ico"));
	w.show();
	return a.exec();
}
