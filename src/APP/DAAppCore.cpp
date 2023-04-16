﻿#include "DAAppCore.h"
#include <QFileInfo>
#include <QApplication>
#include <QDebug>
#include <QDir>
// API
#include "DAAppUI.h"
#include "DAAppRibbonArea.h"
#include "DAAppDataManager.h"
#include "DACommandInterface.h"
#include "DAAppProject.h"
#include "DAAppCommand.h"
// DA Python
#include "DAPyInterpreter.h"
#include "DAPybind11QtTypeCast.h"
#include "DAPybind11InQt.h"
#include "DAPyScripts.h"
//===================================================
// using DA namespace -- 禁止在头文件using！！
//===================================================

using namespace DA;

//===================================================
// DAAppCore
//===================================================
DAAppCore::DAAppCore(QObject* p)
    : DACoreInterface(p), mAppUI(nullptr), mAppCmd(nullptr), mIsPythonInterpreterInitialized(false), mProject(nullptr)
{
    mDataManager = nullptr;
}

DAAppCore& DAAppCore::getInstance()
{
    static DAAppCore s_core;
    return (s_core);
}

bool DAAppCore::initialized()
{
    //初始化python环境
    qDebug() << "begin app core initialized";
    initializePythonEnv();
    qDebug() << "core have been initialized Python Env";
    //初始化数据
    mDataManager = new DAAppDataManager(this, this);
    qDebug() << "core have been initialized App Data Manager";
    mProject = new DAAppProject(this, this);
    return true;
}

DAUIInterface* DAAppCore::getUiInterface() const
{
    return mAppUI;
}

DAProjectInterface* DAAppCore::getProjectInterface() const
{
    return mProject;
}

void DAAppCore::createUi(SARibbonMainWindow* mainwindow)
{
    mAppUI = new DAAppUI(mainwindow, this);
    mAppUI->createUi();
    mAppCmd = mAppUI->getAppCmd();
    if (mDataManager) {
        //把dataManager的undo stack 注册
        if (mAppCmd) {
            mAppCmd->setDataManagerStack(mDataManager->getUndoStack());
        }
    }
}

/**
 * @brief python内核是否初始化成功
 * @return
 */
bool DAAppCore::isPythonInterpreterInitialized() const
{
    return mIsPythonInterpreterInitialized;
}

DADataManagerInterface* DAAppCore::getDataManagerInterface() const
{
    return mDataManager;
}

/**
 * @brief 初始化python环境
 * @return
 */
bool DAAppCore::initializePythonEnv()
{
    mIsPythonInterpreterInitialized = false;
    try {
        DA::DAPyInterpreter& python = DA::DAPyInterpreter::getInstance();
        //初始化python环境
        QString pypath = getPythonInterpreterPath();
        qInfo() << tr("Python interpreter path is %1").arg(pypath);
        python.setPythonHomePath(pypath);
        python.initializePythonInterpreter();
        //初始化环境成功后，加入脚本路径

        DA::DAPyScripts& scripts = DA::DAPyScripts::getInstance();
        QString scriptPath       = getPythonScriptsPath();
        qInfo() << tr("Python scripts path is %1").arg(scriptPath);
        scripts.appendSysPath(scriptPath);
        if (!scripts.initScripts()) {
            qCritical() << tr("Scripts initialize error");
            return false;
        }

    } catch (const std::exception& e) {
        qCritical() << tr("Initialize python environment error:%1").arg(e.what());
        return false;
    }
    mIsPythonInterpreterInitialized = true;
    return true;
}

/**
 * @brief 获取DAAppUI，省去qobject_cast
 * @return
 */
DAAppUI* DAAppCore::getAppUi()
{
    return mAppUI;
}
/**
 * @brief 获取工程
 * @return
 */
DAAppProject* DAAppCore::getAppProject()
{
    return mProject;
}
/**
 * @brief 直接获取数据
 * @return
 */
DAAppDataManager* DAAppCore::getAppDatas()
{
    return mDataManager;
}
/**
 * @brief 直接获取DAAppCommand
 *
 * @note 此函数必须在@ref createUi 之后调用才有实际结果
 * @return
 */
DAAppCommand* DAAppCore::getAppCmd()
{
    return mAppCmd;
}

/**
 * @brief 获取Python环境路径
 * @return
 */
QString DAAppCore::getPythonInterpreterPath()
{
    QString appabsPath = QApplication::applicationDirPath();
    return QDir::toNativeSeparators(appabsPath + "/Python");
}

/**
 * @brief 获取python脚本的位置
 * @return
 */
QString DAAppCore::getPythonScriptsPath()
{
    QString appabsPath = QApplication::applicationDirPath();
    return QDir::toNativeSeparators(appabsPath + "/PyScripts");
}
