#include "HistoryDIrTool.h"
#include <QFileDialog>
#include <boost/filesystem.hpp>
#include <QDebug>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "KeyValue.h"

#include <boost/filesystem.hpp>

using namespace boost::filesystem;

#pragma execution_character_set("utf-8")

static bool isParse = false;
static bool isHistory = false;

HistoryDIrTool::HistoryDIrTool(QWidget* parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	vct_str_dir_path_delete.clear();
	vct_str_dir_path_temp.clear();
	vct_str_dir_path_remain.clear();
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

void HistoryDIrTool::SplitLine()
{
	ui.textLog->append("=====================================");
}

void HistoryDIrTool::SplitLineSingle()
{
	ui.textLog->append("-------------------------------------");
}

void HistoryDIrTool::openDirPath()
{
	isParse = false;
	vct_str_dir_path_temp.clear();
	vct_str_dir_path_delete.clear();
	vct_str_dir_path_remain.clear();

	qstr_dir_path = QFileDialog::getExistingDirectory(NULL, "Select Directory", ".");
	SplitLine();
	ui.textLog->append("Open path: " + qstr_dir_path);
	str_dir_path = QStr2Str(qstr_dir_path);

	return;
}

void HistoryDIrTool::DirectoryParse()
{
	ui.textLog->append("Found the path below");
	SplitLineSingle();
}

void HistoryDIrTool::PrintAllFile(const path& full_path)
{
	if (boost::filesystem::exists(full_path))
	{
		boost::filesystem::directory_iterator item_end;
		for (boost::filesystem::directory_iterator item_begin(full_path); item_begin != item_end; item_begin++)
		{
			if (is_directory(*item_begin))
			{
				if (isHistory)
				{
					vct_str_dir_path_temp.clear();
					boost::filesystem::path myPath(full_path);
					boost::filesystem::directory_iterator endIter;
					for (boost::filesystem::directory_iterator iter(myPath); iter != endIter; iter++)
					{
						if (boost::filesystem::is_directory(*iter))
						{
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
									ui.textLog->append("Parse directory error");
									continue;;
								}
								CKeyValue keyvalue(timeStamp, str_path);
								vct_str_dir_path_temp.push_back(keyvalue);
							}
						}
					}
					//排序
					sort(vct_str_dir_path_temp.begin(), vct_str_dir_path_temp.end(), CKeyValue::less_first);
					isHistory = false;
					int iDelete;
					int iRemain = ui.spinBoxDelCount->value();
					if (iRemain <= vct_str_dir_path_temp.size())
					{
						for (iDelete = 0; iDelete < vct_str_dir_path_temp.size() - iRemain; iDelete++)
						{
							vct_str_dir_path_delete.push_back(vct_str_dir_path_temp[iDelete].second);
						}
						for (; iDelete < vct_str_dir_path_temp.size(); iDelete++)
						{
							vct_str_dir_path_remain.push_back(vct_str_dir_path_temp[iDelete].second);
						}
					}
				}
				else
				{
					string str_path = item_begin->path().string();
					int ipos = str_path.rfind("\\");
					if (ipos != str_path.npos)
					{
						string str_dir_name = str_path.substr(ipos + 1);

						if (strcmp(str_dir_name.c_str(), "history") == 0)
						{
							isHistory = true;
						}
						QString qstr = item_begin->path().string().c_str();
						qDebug() << qstr << endl;
						PrintAllFile(item_begin->path());
					}
				}

			}
		}
	}
}

void HistoryDIrTool::dirParse()
{
	ui.textLog->clear();

	if (str_dir_path.empty())
	{
		SplitLine();
		ui.textLog->append("Please open directory path first");
		return;
	}

	if (ui.spinBoxDelCount->value() == 0)
	{
		SplitLine();
		ui.textLog->append("Please select remain num first.");
		return;
	}

	vct_str_dir_path_remain.clear();
	vct_str_dir_path_delete.clear();

	boost::filesystem::path myPath(str_dir_path);
	PrintAllFile(myPath);

	SplitLine();
	ui.textLog->append("Directory will be delete below:");
	for (int iDelete = 0; iDelete < vct_str_dir_path_delete.size(); iDelete++)
	{
		ui.textLog->append(QString(vct_str_dir_path_delete[iDelete].c_str()));
	}
	SplitLineSingle();
	ui.textLog->append("Remain below:");
	for (int iRemain = 0; iRemain < vct_str_dir_path_remain.size(); iRemain++)
	{
		ui.textLog->append(QString(vct_str_dir_path_remain[iRemain].c_str()));
	}

	isParse = true;
}

void HistoryDIrTool::delDir()
{
	if (isParse)
	{
		if (!vct_str_dir_path_delete.empty())
		{
			SplitLine();
			for (int iDelete = 0; iDelete < vct_str_dir_path_delete.size(); iDelete++)
			{
				char c[256] = { 0 };
				sprintf(c, "Delete: %s.", vct_str_dir_path_delete[iDelete].c_str());
				ui.textLog->append(QString(c));
				boost::filesystem::path delPath(vct_str_dir_path_delete[iDelete].c_str());
				bool isExists = boost::filesystem::exists(delPath);
				if (!isExists)
					continue;
				boost::filesystem::remove_all(delPath);
			}
			vct_str_dir_path_delete.clear();
			vct_str_dir_path_temp.clear();
			vct_str_dir_path_remain.clear();
			str_dir_path.clear();
		}

	}
	else
	{
		SplitLine();
		ui.textLog->append("Please parse directory first.");
	}
	isParse = false;
}
