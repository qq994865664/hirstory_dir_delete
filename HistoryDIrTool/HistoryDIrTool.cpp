#include "HistoryDIrTool.h"
#include <QFileDialog>
#include <boost/filesystem.hpp>
#include <QDebug>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "KeyValue.h"

#pragma execution_character_set("utf-8")

HistoryDIrTool::HistoryDIrTool(QWidget* parent)
	: QDialog(parent)
{
	ui.setupUi(this);
}

string HistoryDIrTool::QStr2Str(QString qstr)
{
	return qstr.toUtf8().constData();
}

long long HistoryDIrTool::StandardToStamp(char* str_time)
{
	struct tm stm;
	int iY, iM, iD, iH, iMin, iS;

	memset(&stm, 0, sizeof(stm));
	iY = atoi(str_time);
	iM = atoi(str_time + 5);
	iD = atoi(str_time + 8);
	iH = atoi(str_time + 11);
	iMin = atoi(str_time + 14);
	iS = atoi(str_time + 17);

	stm.tm_year = iY - 1900;
	stm.tm_mon = iM - 1;
	stm.tm_mday = iD;
	stm.tm_hour = iH;
	stm.tm_min = iMin;
	stm.tm_sec = iS;

	//标准时间格式例如：2016:08:02 12:12:30
	return (int)mktime(&stm);
}

void HistoryDIrTool::openDirPath()
{
	qstr_dir_path = QFileDialog::getExistingDirectory(NULL, "Select Directory", ".");
	ui.textLog->append("path: " + qstr_dir_path);
	str_dir_path = QStr2Str(qstr_dir_path);
}

void HistoryDIrTool::dirParse()
{
	string str = "2014-03-21_18-19-20";
	int y, m, d, h, n, s;
	int nCount = sscanf(str.c_str(), "%4d-%2d-%2d_%2d-%2d-%2d", &y, &m, &d, &h, &n, &s);
	char szTime[32] = { 0 };

	if (nCount == 6)
	{
		sprintf(szTime, "%04d-%02d-%02d %02d:%02d:%02d", y, m, d, h, n, s);
		long long timeStamp = StandardToStamp(szTime);
	}
	else
	{
		ui.textLog->append("parse time error");
		return;
	}

	vct_str_dir_path.clear();

	boost::filesystem::path myPath(str_dir_path);
	boost::filesystem::directory_iterator endIter;
	for (boost::filesystem::directory_iterator iter(myPath); iter != endIter; iter++) {
		if (boost::filesystem::is_directory(*iter)) {
			QString qstr = iter->path().string().c_str();
			string str_dir_name;
			string str_path = iter->path().string();
			int ipos = str_path.rfind("\\");
			if (ipos != str_path.npos)
			{
				str_dir_name = str_path.substr(ipos + 1);

				int y, m, d, h, n, s;
				int nCount = sscanf(str_dir_name.c_str(), "%4d-%2d-%2d_%2d-%2d-%2d", &y, &m, &d, &h, &n, &s);
				char szTime[32] = { 0 };
				long long timeStamp;
				if (nCount == 6)
				{
					sprintf(szTime, "%04d-%02d-%02d %02d:%02d:%02d", y, m, d, h, n, s);
					timeStamp = StandardToStamp(szTime);
				}
				else
				{
					ui.textLog->append("parse time error");
					return;
				}
				CKeyValue keyvalue(timeStamp, str_path);
				vct_str_dir_path.push_back(keyvalue);
			}
			ui.textLog->append(qstr);
		}
	}
	//sort in timestamp
	sort(vct_str_dir_path.begin(), vct_str_dir_path.end(), CKeyValue::less_first);
}

void HistoryDIrTool::delDir()
{
	ui.textLog->append("delDir");
	vct_str_dir_path.clear();
}
