#include "HistoryDIrTool.h"
#include <QFileDialog>
#include <boost/filesystem.hpp>
#include <QDebug>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "KeyValue.h"
#include <windows.h>
#include <boost/filesystem.hpp>
#include <boost/thread/thread.hpp> 
#include <QTimer>

using namespace boost::filesystem;

#pragma execution_character_set("utf-8")

static bool isParseComplete = false;
static bool isDeleteComplete = false;
static bool isHistory = false;
static boost::thread thrd_scan;
static boost::thread thrd_del;

HistoryDIrTool::HistoryDIrTool(QWidget* parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	vct_str_dir_path_delete.clear();
	vct_str_dir_path_temp.clear();
	vct_str_dir_path_remain.clear();
	m_timer = NULL;
	InitTimer();
}

HistoryDIrTool::~HistoryDIrTool()
{
	if (m_timer)
		delete m_timer;
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

void HistoryDIrTool::InitTimer()
{
	if (NULL == m_timer)
		m_timer = new QTimer;
	//设置定时器是否为单次触发。默认为 false 多次触发
	m_timer->setSingleShot(false);

	//定时器触发信号槽
	QObject::connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimeOut()));
}

void HistoryDIrTool::openDirPath()
{
	isParseComplete = false;
	vct_str_dir_path_temp.clear();
	vct_str_dir_path_delete.clear();
	vct_str_dir_path_remain.clear();

	qstr_dir_path = QFileDialog::getExistingDirectory(NULL, u8"选择目录", ".");

	if (qstr_dir_path.isEmpty())
		return;

	SplitLine();
	ui.textLog->append(u8"打开目录: " + qstr_dir_path);

	char szPath[MAX_PATH] = { 0 };
	QByteArray ba = qstr_dir_path.toLocal8Bit();
	memcpy(szPath, ba.data(), ba.size() + 1);

	str_dir_path = szPath;
}

void HistoryDIrTool::DirectoryParse()
{
	ui.textLog->append(QString(u8"找到下面路径"));
	SplitLineSingle();
}

void HistoryDIrTool::ScanDirectory(const path& full_path)
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
									//ui.textLog->append("Parse directory error");
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
						ScanDirectory(item_begin->path());
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
		ui.textLog->append(u8"请先打开目标路径");
		return;
	}

	if (ui.spinBoxDelCount->value() == 0)
	{
		SplitLine();
		ui.textLog->append(u8"请先选择保留条数");
		return;
	}
	ui.textLog->append(u8"正在努力解析目录，请耐心等待...");
	//启动或重启定时器, 并设置定时器时间：毫秒
	m_timer->start(5000);
	thrd_scan = boost::thread(boost::bind(&HistoryDIrTool::RunScanDir, this));
	/*thrd_scan.join();*/
}

void HistoryDIrTool::delDir()
{
	if (!isDeleteComplete)
	{
		ui.textLog->append(u8"正在删除目录，请耐心等待...");
		ui.textLog->append(u8"在删除操作完成前，请勿关闭程序，请耐心等待...");
		//m_timer->start(200);
		thrd_del = boost::thread(boost::bind(&HistoryDIrTool::RunDelete, this));
	}
	else
	{
		SplitLine();
		ui.textLog->append(u8"请先进行解析目录");
	}
	
	//thrd_del.join();
}

void HistoryDIrTool::RunScanDir()
{
	vct_str_dir_path_remain.clear();
	vct_str_dir_path_delete.clear();
	isParseComplete = false;
	isDeleteComplete = true;
	boost::filesystem::path myPath(str_dir_path);
	ScanDirectory(myPath);
	isDeleteComplete = false;
	isParseComplete = true;
}

void HistoryDIrTool::RunDelete()
{
	if (!isDeleteComplete)
	{
		if (!vct_str_dir_path_delete.empty())
		{
			for (long iDelete = 0; iDelete < vct_str_dir_path_delete.size(); iDelete++)
			{
				string str_path_delete = vct_str_dir_path_delete[iDelete].c_str();
				if (str_path_delete.length() >= MAX_PATH)
				{
					continue;
				}
				//try
				{
					boost::filesystem::path delPath(str_path_delete.c_str());
					bool isExists = boost::filesystem::exists(delPath);
					if (!isExists)
						continue;

					boost::system::error_code ec;
					boost::filesystem::remove_all(delPath,ec);
					boost::this_thread::sleep(boost::posix_time::milliseconds(100));

					char c[MAX_PATH] = { 0 };
					string str_result = ec.message();
					sprintf(c, "%s", str_result.c_str());
					ui.textLog->append(QString::fromLocal8Bit(c));
					ui.textLog->update();
				}
				//catch (exception* e)
				//{
				//	ui.textLog->append(u8"删除文件失败: " + QString(str_path_delete.c_str()));
				//	ui.textLog->append("Error Code");
				//	ui.textLog->append(e->what());
				//}
			}
			vct_str_dir_path_delete.clear();
			vct_str_dir_path_temp.clear();
			vct_str_dir_path_remain.clear();
			str_dir_path.clear();
		}
		isDeleteComplete = true;
		SplitLine();
		ui.textLog->append(QString(u8"删除操作已完成."));
	}
}

void HistoryDIrTool::onTimeOut()
{
	if (isParseComplete)
	{
		isParseComplete = false;
		//判断定时器是否运行
		if (m_timer->isActive())
			m_timer->stop();   //停止定时器

		if (vct_str_dir_path_delete.empty())
		{
			SplitLine();
			char c[256] = { 0 };
			sprintf(c, u8"当前保留条数: %d 情况下,没有匹配到要删除的目录.", ui.spinBoxDelCount->value());
			ui.textLog->append(c);
			return;
		}
		SplitLine();
		ui.textLog->append(u8"目录分析完成.");
		ui.textLog->append(u8"以下目录将被删除:");
		for (int iDelete = 0; iDelete < vct_str_dir_path_delete.size(); iDelete++)
		{
			ui.textLog->append(QString::fromLocal8Bit(vct_str_dir_path_delete[iDelete].c_str()));
		}
		SplitLineSingle();
		ui.textLog->append(u8"以下目录将被保留:");
		for (int iRemain = 0; iRemain < vct_str_dir_path_remain.size(); iRemain++)
		{
			ui.textLog->append(QString::fromLocal8Bit(vct_str_dir_path_remain[iRemain].c_str()));
		}
	}
	else
	{
		ui.textLog->append(u8"正在努力解析目录，请耐心等待...");
	}
}
