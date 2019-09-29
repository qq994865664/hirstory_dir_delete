#pragma once

#include <QtWidgets/QDialog>
#include "ui_HistoryDIrTool.h"
#include <string.h>
#include "KeyValue.h"

using namespace std;

class HistoryDIrTool : public QDialog
{
	Q_OBJECT

public:
	HistoryDIrTool(QWidget *parent = Q_NULLPTR);
	string QStr2Str(QString qstr);
	long long StandardToStamp(char* str_time);

private slots:
	void openDirPath();
	void dirParse();
	void delDir();

private:
	QString qstr_dir_path;
	string str_dir_path;

	vector<CKeyValue> vct_str_dir_path;

private:
	Ui::HistoryDIrToolClass ui;
};


