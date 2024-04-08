﻿#ifndef DATAANALYSCONTROLLER_H
#define DATAANALYSCONTROLLER_H
#include <QObject>
#include "DAData.h"
#include "DACoreInterface.h"
#include "DADockingAreaInterface.h"
namespace DA
{
class DADataManager;
class DADataManageWidget;
}
class DataAnalysisActions;
class DataAnalysExecutor;
class DialogSpectrumSetting;
class DataAnalysController : public QObject
{
	Q_OBJECT
public:
	DataAnalysController(DA::DACoreInterface* core, DataAnalysisActions* actions, QObject* p = nullptr);
	~DataAnalysController();
	// 获取当前选中的数据
	DA::DAData getCurrentSelectData() const;

private:
	void initConnect();
    DialogSpectrumSetting* getSpectrumSettingDialog();
    // 获取当前界面选择的序列，如果没有，返回none
    DA::DAPySeries getCurrentSelectSeries();
private slots:
	// 数据处理---------------------------------------------
	//  |-信号分析
	//     -频谱
	void onActionSpectrumTriggered();

private:
	DA::DACoreInterface* mCore { nullptr };
	DataAnalysisActions* mActions { nullptr };
    DA::DADataManager* mDataMgr { nullptr };
	DA::DADockingAreaInterface* mDockingArea { nullptr };
	DA::DADataManageWidget* mDataManagerWidget { nullptr };
    DialogSpectrumSetting* mDialogSpectrumSetting { nullptr };
    std::unique_ptr< DataAnalysExecutor > mExecutor;
};

#endif  // DATAANALYSCONTROLLER_H
