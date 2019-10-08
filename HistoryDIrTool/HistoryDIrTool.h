#pragma once

#include <QtWidgets/QDialog>
#include "ui_HistoryDIrTool.h"
#include <string.h>
#include "KeyValue.h"
#include <boost/filesystem.hpp>
#include <QTime>

using namespace boost::filesystem;

using namespace std;

class HistoryDIrTool : public QDialog
{
	Q_OBJECT

public:
	HistoryDIrTool(QWidget *parent = Q_NULLPTR);
	~HistoryDIrTool();
	string QStr2Str(QString qstr);
	long long StandardToStamp(char* str_time);
	void SplitLine();
	void SplitLineSingle();

	void InitTimer();
	

private slots:
	void openDirPath();

	void DirectoryParse();
	void ScanDirectory(const path& full_path);

	void dirParse();
	void delDir();

	void RunScanDir();
	void RunDelete();

	void onTimeOut();


private:
	QTimer* m_timer;
	QString qstr_dir_path;
	string str_dir_path;

	vector<CKeyValue> vct_str_dir_path_temp;
	vector<string>	vct_str_dir_path_delete;
	vector<string>	vct_str_dir_path_remain;

private:
	Ui::HistoryDIrToolClass ui;
};


