﻿#include "DAAppRibbonArea.h"
#include "AppMainWindow.h"
#include <QActionGroup>
// SARibbon
#include "SARibbonMainWindow.h"
#include "SARibbonBar.h"
#include "SARibbonButtonGroupWidget.h"
#include "SARibbonCategory.h"
#include "SARibbonPannel.h"
#include "SARibbonContextCategory.h"
#include "SARibbonQuickAccessBar.h"
#include "SARibbonButtonGroupWidget.h"
#include "SARibbonMenu.h"
#include "SARibbonCtrlContainer.h"
#include "SARibbonApplicationButton.h"
#include "SARibbonLineWidgetContainer.h"
// stl
// Qt
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QSignalBlocker>
#include <QStandardPaths>
#include <QFontComboBox>
#include <QComboBox>
#include <QInputDialog>
#include <QMenu>
// ui
#include "DAAppRibbonApplicationMenu.h"

#if DA_ENABLE_PYTHON
// Py
#include "DAPyScripts.h"
#include "pandas/DAPyDataFrame.h"
#include "numpy/DAPyDType.h"
// Widget
#include "DAPyDTypeComboBox.h"
#include "DADataOperateOfDataFrameWidget.h"
#endif

// api
#include "DAAppUI.h"
#include "DAAppCommand.h"
#include "DAAppCore.h"
#include "DAAppDockingArea.h"
#include "DAAppActions.h"
// Qt-Advanced-Docking-System
#include "DockManager.h"
#include "DockAreaWidget.h"
// command
// Widget
#include "DADataManageWidget.h"
#include "DAFigureWidget.h"
#include "DAChartWidget.h"
// Dialog
// DACommonWidgets
#include "DAFontEditPannelWidget.h"
#include "DAShapeEditPannelWidget.h"
// Workflow
// project

// 快速链接信号槽
#define DAAPPRIBBONAREA_ACTION_BIND(actionname, functionname)                                                          \
    connect(actionname, &QAction::triggered, this, &DAAppRibbonArea::functionname)

#define DAAPPRIBBONAREA_COMMON_SETTING_CPP(MiddleName, ShapeEditPannelWidget, FontEditWidget)                          \
    QPen DAAppRibbonArea::get##MiddleName##Pen() const                                                                 \
    {                                                                                                                  \
        return ShapeEditPannelWidget->getBorderPen();                                                                  \
    }                                                                                                                  \
    QBrush DAAppRibbonArea::get##MiddleName##Brush() const                                                             \
    {                                                                                                                  \
        return ShapeEditPannelWidget->getBackgroundBrush();                                                            \
    }                                                                                                                  \
    QFont DAAppRibbonArea::get##MiddleName##Font() const                                                               \
    {                                                                                                                  \
        return FontEditWidget->getCurrentFont();                                                                       \
    }                                                                                                                  \
    QColor DAAppRibbonArea::get##MiddleName##FontColor() const                                                         \
    {                                                                                                                  \
        return FontEditWidget->getCurrentFontColor();                                                                  \
    }                                                                                                                  \
    void DAAppRibbonArea::set##MiddleName##Pen(const QPen& v)                                                          \
    {                                                                                                                  \
        QSignalBlocker b(ShapeEditPannelWidget);                                                                       \
        ShapeEditPannelWidget->setBorderPen(v);                                                                        \
    }                                                                                                                  \
    void DAAppRibbonArea::set##MiddleName##Brush(const QBrush& v)                                                      \
    {                                                                                                                  \
        QSignalBlocker b(ShapeEditPannelWidget);                                                                       \
        ShapeEditPannelWidget->setBackgroundBrush(v);                                                                  \
    }                                                                                                                  \
    void DAAppRibbonArea::set##MiddleName##Font(const QFont& v)                                                        \
    {                                                                                                                  \
        QSignalBlocker b(FontEditWidget);                                                                              \
        FontEditWidget->setCurrentFont(v);                                                                             \
    }                                                                                                                  \
    void DAAppRibbonArea::set##MiddleName##FontColor(const QColor& v)                                                  \
    {                                                                                                                  \
        QSignalBlocker b(FontEditWidget);                                                                              \
        FontEditWidget->setCurrentFontColor(v);                                                                        \
    }

//===================================================
// using DA namespace -- 禁止在头文件using！！
//===================================================

using namespace DA;

//===================================================
// DAAppRibbonArea
//===================================================

DAAppRibbonArea::DAAppRibbonArea(DAUIInterface* u) : DARibbonAreaInterface(u)
{
    DAAppUI* appui = qobject_cast< DAAppUI* >(u);
    m_app          = qobject_cast< AppMainWindow* >(appui->mainWindow());
    m_actions      = qobject_cast< DAAppActions* >(u->getActionInterface());
    m_appCmd       = qobject_cast< DAAppCommand* >(u->getCommandInterface());
    buildMenu();
    buildRibbon();
    buildRedoUndo();
    resetText();
}

DAAppRibbonArea::~DAAppRibbonArea()
{
}

/**
 * @brief 构建所有的action
 */
void DAAppRibbonArea::buildMenu()
{
    m_menuInsertRow = new SARibbonMenu(m_app);
    m_menuInsertRow->setObjectName("menuInsertRow");
    m_menuInsertRow->addAction(m_actions->actionInsertRowAbove);
    m_menuInsertColumn = new SARibbonMenu(m_app);
    m_menuInsertColumn->setObjectName("menuInsertColumn");
    m_menuInsertColumn->addAction(m_actions->actionInsertColumnLeft);
    m_menuChartLegendProperty                 = new SARibbonMenu(m_app);
    m_actionOfMenuChartLegendAlignmentSection = m_menuChartLegendProperty->addSection(tr("Location"));
    m_menuChartLegendProperty->addAction(m_actions->actionChartLegendAlignmentInTopLeft);
    m_menuChartLegendProperty->addAction(m_actions->actionChartLegendAlignmentInTop);
    m_menuChartLegendProperty->addAction(m_actions->actionChartLegendAlignmentInTopRight);
    m_menuChartLegendProperty->addAction(m_actions->actionChartLegendAlignmentInRight);
    m_menuChartLegendProperty->addAction(m_actions->actionChartLegendAlignmentInBottomRight);
    m_menuChartLegendProperty->addAction(m_actions->actionChartLegendAlignmentInBottom);
    m_menuChartLegendProperty->addAction(m_actions->actionChartLegendAlignmentInBottomLeft);
    m_menuChartLegendProperty->addAction(m_actions->actionChartLegendAlignmentInLeft);
    //
    m_menuChartLegendProperty->addSection(tr("Property"));
    // 构建最大列数的窗口
    auto addControlWidgetInChartLegendMenu = [ this ](QWidget* w, const QString& text) -> SARibbonCtrlContainer* {
        SARibbonCtrlContainer* container = new SARibbonCtrlContainer(this->m_menuChartLegendProperty);
        container->setContainerWidget(w);
        container->setEnableShowTitle(true);
        container->setText(text);
        QWidgetAction* act = new QWidgetAction(this->m_menuChartLegendProperty);
        act->setDefaultWidget(container);
        this->m_menuChartLegendProperty->addAction(act);
        return container;
    };
    m_spinboxChartLegendMaxColumns       = new QSpinBox(m_menuChartLegendProperty);
    m_ctrlContainerChartLegendMaxColumns = addControlWidgetInChartLegendMenu(m_spinboxChartLegendMaxColumns,
                                                                             tr("Max Columns"));  // cn:列数
    m_spinboxChartLegendMargin           = new QSpinBox(m_menuChartLegendProperty);
    m_ctrlContainerChartLegendMargin = addControlWidgetInChartLegendMenu(m_spinboxChartLegendMargin, tr("Margin"));  // cn:图例边缘
    m_spinboxChartLegendSpacing = new QSpinBox(m_menuChartLegendProperty);
    m_ctrlContainerChartLegendSpacing = addControlWidgetInChartLegendMenu(m_spinboxChartLegendSpacing, tr("Spacing"));  // cn:图例间隔
    m_spinboxChartLegendItemMargin         = new QSpinBox(m_menuChartLegendProperty);
    m_ctrlContainerChartLegendItemMargin   = addControlWidgetInChartLegendMenu(m_spinboxChartLegendItemMargin,
                                                                             tr("Item Margin"));  // cn:条目边缘
    m_spinboxChartLegendItemSpacing        = new QSpinBox(m_menuChartLegendProperty);
    m_ctrlContainerChartLegendItemSpacing  = addControlWidgetInChartLegendMenu(m_spinboxChartLegendItemSpacing,
                                                                              tr("Item Spacing"));  // cn:条目间隔
    m_spinboxChartLegendBorderRadius       = new QDoubleSpinBox(m_menuChartLegendProperty);
    m_ctrlContainerChartLegendBorderRadius = addControlWidgetInChartLegendMenu(m_spinboxChartLegendBorderRadius,
                                                                               tr("Border Radius"));  // cn:圆角
    //
    m_menuTheme = new SARibbonMenu(m_app);
    m_menuTheme->setObjectName("menuTheme");
    m_menuTheme->setIcon(QIcon(":/Icon/Icon/theme.svg"));
    m_menuTheme->addAction(m_actions->actionRibbonThemeOffice2013);
    m_menuTheme->addAction(m_actions->actionRibbonThemeOffice2016Blue);
    m_menuTheme->addAction(m_actions->actionRibbonThemeOffice2021Blue);
    m_menuTheme->addAction(m_actions->actionRibbonThemeDark);
}

void DAAppRibbonArea::retranslateUi()
{
    resetText();
}

void DAAppRibbonArea::resetText()
{
    ribbonBar()->applicationButton()->setText(tr(" File "));  // 文件

    m_categoryMain->setCategoryName(tr("Main"));               // cn:主页
    m_pannelMainFileOpt->setPannelName(tr("File Operation"));  // cn:文件操作
    m_pannelSetting->setPannelName(tr("Config"));              // cn:配置
    m_pannelMainWorkflowOpt->setPannelName(tr("Workflow"));    // cn:工作流
    m_pannelMainDataOpt->setPannelName(tr("Data Operation"));  // cn:数据操作
    m_categoryData->setCategoryName(tr("Data"));               // cn:数据
    m_pannelDataOperate->setPannelName(tr("Data Operation"));  // cn:数据操作

    m_categoryView->setCategoryName(tr("View"));         // cn:视图
    m_pannelViewMainView->setPannelName(tr("Display"));  // cn:视图显示

    m_contextDataFrame->setContextTitle(tr("DataFrame"));        ///< DataFrame
    m_categoryDataframeOperate->setCategoryName(tr("Operate"));  ///< DataFrame -> Operate
    m_pannelDataframeOperateAxes->setPannelName(tr("Axes"));     ///< DataFrame -> Operate -> Axes
    m_pannelDataframeOperateDType->setPannelName(tr("Type"));    ///< DataFrame -> Type
#if DA_ENABLE_PYTHON
    m_comboxColumnTypesContainer->setPrefix(tr("Type"));  ///< DataFrame -> Type -> Type
#endif
    m_pannelDataframeOperateStatistic->setPannelName(tr("Statistic"));  ///< DataFrame -> Statistic

    // 编辑标签
    m_categoryEdit->setCategoryName(tr("Edit"));  // cn:编辑

    m_contextWorkflow->setContextTitle(tr("Workflow"));  // cn:工作流

    m_categoryWorkflowGraphicsEdit->setCategoryName(tr("Workflow Edit"));  // cn:工作流编辑
    m_pannelWorkflowItem->setPannelName(tr("Item"));                       // cn:图元
    m_pannelWorkflowText->setPannelName(tr("Text"));                       // cn:文本
    m_pannelWorkflowBackground->setPannelName(tr("Background"));           // cn:背景
    m_pannelWorkflowView->setPannelName(tr("View"));                       // cn:视图

    m_categoryWorkflowRun->setCategoryName(tr("Workflow Run"));  // cn:工作流运行
    m_pannelWorkflowRun->setPannelName(tr("Run"));               // cn:运行
    //
    m_categoryFigure->setCategoryName(tr("Figure"));             // cn:绘图
    m_pannelFigureSetting->setPannelName(tr("Figure Setting"));  // cn:绘图设置
    m_pannelChartAdd->setPannelName(tr("Add Chart"));            // cn:添加绘图
    // 绘图上下文标签
    m_contextChart->setContextTitle(tr("Chart"));                        // cn:绘图
    m_categoryChartEdit->setCategoryName(tr("Chart Edit"));              // cn:绘图编辑
    m_actionOfMenuChartLegendAlignmentSection->setText(tr("Location"));  // cn:方位
    m_menuChartLegendProperty->setTitle(tr("Legend"));
    m_ctrlContainerChartLegendMaxColumns->setText(tr("Max Columns"));      // cn:列数
    m_ctrlContainerChartLegendMargin->setText(tr("Margin"));               // cn:图例边缘
    m_ctrlContainerChartLegendSpacing->setText(tr("Spacing"));             // cn:图例间隔
    m_ctrlContainerChartLegendItemMargin->setText(tr("Item Margin"));      // cn:条目边缘
    m_ctrlContainerChartLegendItemSpacing->setText(tr("Item Spacing"));    // cn:条目间隔
    m_ctrlContainerChartLegendBorderRadius->setText(tr("Border Radius"));  // cn:圆角
    // 其他
    m_menuTheme->setTitle(tr("theme"));               // cn:主题
    m_menuTheme->setToolTip(tr("set ribbon theme"));  // cn:设置主题
    //
    m_app->setWindowTitle(tr("Data Work Flow"));
}

/**
 * @brief 构建ribbon
 */
void DAAppRibbonArea::buildRibbon()
{
    ribbonBar()->showMinimumModeButton();
    buildRibbonMainCategory();
    buildRibbonDataCategory();
    buildRibbonViewCategory();
    buildRibbonEditCategory();
    buildRibbonFigureCategory();
    buildRibbonQuickAccessBar();
    // 上下文标签
    buildContextCategoryDataFrame();
    buildContextCategoryWorkflow();
    buildContextCategoryChart();
    //
    buildApplicationMenu();
    //
    buildRightButtonBar();
}

/**
 * @brief 构建主页标签
 * 主页的category objname = da-ribbon-category-main
 */
void DAAppRibbonArea::buildRibbonMainCategory()
{
    m_categoryMain = new SARibbonCategory(app());
    m_categoryMain->setObjectName(QStringLiteral("da-ribbon-category-main"));

    //--------Common--------------------------------------------------

    m_pannelMainFileOpt = new SARibbonPannel(m_categoryMain);
    m_pannelMainFileOpt->setObjectName(QStringLiteral("da-ribbon-pannel-main.common"));
    m_pannelMainFileOpt->addLargeAction(m_actions->actionOpen);
    m_pannelMainFileOpt->addSmallAction(m_actions->actionSave);
    m_pannelMainFileOpt->addSmallAction(m_actions->actionSaveAs);
    // todo:暂时屏蔽掉插入工程功能
    //    m_pannelMainFileOpt->addSeparator();
    //    m_pannelMainFileOpt->addSmallAction(m_actions->actionAppendProject);
    m_categoryMain->addPannel(m_pannelMainFileOpt);

    //--------Data Opt--------------------------------------------------
    // 这里演示通过addPannel的重载函数来创建pannel
    m_pannelMainDataOpt = m_categoryMain->addPannel("Data Opt");
    ;
    m_pannelMainDataOpt->setObjectName(QStringLiteral("da-pannel-main.data-opt"));
    m_pannelMainDataOpt->addLargeAction(m_actions->actionAddData);

    // Chart Opt
    m_pannelMainChartOpt = new SARibbonPannel(m_categoryMain);
    m_pannelMainChartOpt->setObjectName(QStringLiteral("da-pannel-main.chart-opt"));
    m_pannelMainChartOpt->addLargeAction(m_actions->actionAddFigure);
    m_categoryMain->addPannel(m_pannelMainChartOpt);
    //--------Workflow Opt----------------------------------------------
    m_pannelMainWorkflowOpt = m_categoryMain->addPannel(tr("Workflow"));
    m_pannelMainWorkflowOpt->setObjectName(QStringLiteral("da-pannel-main.workflow"));
    m_pannelMainWorkflowOpt->addLargeAction(m_actions->actionWorkflowRun);
    m_pannelMainWorkflowOpt->addLargeAction(m_actions->actionWorkflowTerminate);
    //--------Setting--------------------------------------------------

    m_pannelSetting = new SARibbonPannel(m_categoryMain);
    m_pannelSetting->setObjectName(QStringLiteral("da-pannel-main.setting"));
    m_pannelSetting->addLargeAction(m_actions->actionSetting);
    m_pannelSetting->addLargeAction(m_actions->actionPluginManager);
    m_categoryMain->addPannel(m_pannelSetting);
    //----------------------------------------------------------

    ribbonBar()->addCategoryPage(m_categoryMain);  // 主页
}

/**
 * @brief 构建数据标签
 * objectname=da-ribbon-category-data
 */
void DAAppRibbonArea::buildRibbonDataCategory()
{
    m_categoryData = new SARibbonCategory(app());
    m_categoryData->setObjectName(QStringLiteral("da-ribbon-category-data"));

    //--------DataOperate--------------------------------------------------

    m_pannelDataOperate = new SARibbonPannel(m_categoryData);
    m_pannelDataOperate->setObjectName(QStringLiteral("da-pannel-data.data-opt"));
    m_pannelDataOperate->addLargeAction(m_actions->actionAddData);
    m_pannelDataOperate->addLargeAction(m_actions->actionRemoveData);
    m_categoryData->addPannel(m_pannelDataOperate);

    //----------------------------------------------------------

    ribbonBar()->addCategoryPage(m_categoryData);
}

/**
 * @brief 构建视图标签
 * objectname=da-ribbon-category-view
 */
void DAAppRibbonArea::buildRibbonViewCategory()
{
    m_categoryView = new SARibbonCategory(app());
    m_categoryView->setObjectName(QStringLiteral("da-ribbon-category-view"));

    //--------MainView--------------------------------------------------

    m_pannelViewMainView = new SARibbonPannel(m_categoryView);
    m_pannelViewMainView->setObjectName(QStringLiteral("da-pannel-view.main"));
    m_pannelViewMainView->addLargeAction(m_actions->actionShowWorkFlowArea);
    m_pannelViewMainView->addMediumAction(m_actions->actionShowWorkFlowManagerArea);
    m_pannelViewMainView->addLargeAction(m_actions->actionShowChartArea);
    m_pannelViewMainView->addMediumAction(m_actions->actionShowChartManagerArea);
    m_pannelViewMainView->addLargeAction(m_actions->actionShowDataArea);
    m_pannelViewMainView->addSeparator();
    m_pannelViewMainView->addSmallAction(m_actions->actionShowMessageLogView);
    m_pannelViewMainView->addSmallAction(m_actions->actionShowSettingWidget);
    m_categoryView->addPannel(m_pannelViewMainView);

    //----------------------------------------------------------

    ribbonBar()->addCategoryPage(m_categoryView);  // 视图
}

/**
 * @brief 构建ribbon的QuickAccessBar
 */
void DAAppRibbonArea::buildRibbonQuickAccessBar()
{
}

/**
 * @brief 构建DataFrame上下文标签
 * objectname=da-ribbon-contextcategory-dataframe
 */
void DAAppRibbonArea::buildContextCategoryDataFrame()
{
    m_contextDataFrame = ribbonBar()->addContextCategory(tr("DataFrame"));
    m_contextDataFrame->setObjectName(QStringLiteral("da-ribbon-contextcategory-dataframe"));
    m_categoryDataframeOperate = m_contextDataFrame->addCategoryPage(tr("Operate"));
    // Axes pannel
    m_pannelDataframeOperateAxes = m_categoryDataframeOperate->addPannel(tr("Axes"));
    m_actions->actionInsertRow->setMenu(m_menuInsertRow);
    m_pannelDataframeOperateAxes->addLargeAction(m_actions->actionInsertRow, QToolButton::MenuButtonPopup);
    m_actions->actionInsertColumnRight->setMenu(m_menuInsertColumn);
    m_pannelDataframeOperateAxes->addLargeAction(m_actions->actionInsertColumnRight, QToolButton::MenuButtonPopup);
    m_pannelDataframeOperateAxes->addLargeAction(m_actions->actionRemoveCell);
    m_pannelDataframeOperateAxes->addMediumAction(m_actions->actionRemoveRow);
    m_pannelDataframeOperateAxes->addMediumAction(m_actions->actionRemoveColumn);
    m_pannelDataframeOperateAxes->addSeparator();
    m_pannelDataframeOperateAxes->addLargeAction(m_actions->actionRenameColumns);
    m_pannelDataframeOperateAxes->addLargeAction(m_actions->actionChangeToIndex);
    // Type pannel
    m_pannelDataframeOperateDType = m_categoryDataframeOperate->addPannel(tr("Type"));
#if DA_ENABLE_PYTHON
    m_comboxColumnTypesContainer = new SARibbonLineWidgetContainer(m_pannelDataframeOperateDType);
    m_comboxColumnTypes          = new DAPyDTypeComboBox(m_comboxColumnTypesContainer);
    m_comboxColumnTypes->setMinimumWidth(Qt5Qt6Compat_fontMetrics_width(m_app->fontMetrics(), "timedelta64(scoll)"));  // 设置最小宽度
    m_comboxColumnTypesContainer->setPrefix(tr("Type"));
    m_comboxColumnTypesContainer->setWidget(m_comboxColumnTypes);
    m_pannelDataframeOperateDType->addWidget(m_comboxColumnTypesContainer, SARibbonPannelItem::Medium);
#endif
    m_castActionsButtonGroup = new SARibbonButtonGroupWidget();
    m_castActionsButtonGroup->addAction(m_actions->actionCastToNum);
    m_castActionsButtonGroup->addAction(m_actions->actionCastToString);
    m_castActionsButtonGroup->addSeparator();
    m_castActionsButtonGroup->addAction(m_actions->actionCastToDatetime);
    m_pannelDataframeOperateDType->addWidget(m_castActionsButtonGroup, SARibbonPannelItem::Medium);
    // Statistic Pannel
    m_pannelDataframeOperateStatistic = m_categoryDataframeOperate->addPannel(tr("Statistic"));
    m_pannelDataframeOperateStatistic->addLargeAction(m_actions->actionCreateDataDescribe);
}

/**
 * @brief 构建Edit标签
 * da-ribbon-category-edit
 * pannel:da-ribbon-pannel-edit-workflow
 */
void DAAppRibbonArea::buildRibbonEditCategory()
{
    m_categoryEdit = new SARibbonCategory(app());
    m_categoryEdit->setObjectName(QStringLiteral("da-ribbon-category-edit"));
    //--------MainView--------------------------------------------------

    m_pannelEditWorkflow = new SARibbonPannel(m_categoryEdit);
    m_pannelEditWorkflow->setObjectName(QStringLiteral("da-pannel-edit.workflow"));
    m_pannelEditWorkflow->addLargeAction(m_actions->actionWorkflowNew);
    m_pannelEditWorkflow->addSeparator();
    m_pannelEditWorkflow->addLargeAction(m_actions->actionWorkflowStartDrawRect);
    m_pannelEditWorkflow->addLargeAction(m_actions->actionWorkflowStartDrawText);

    m_editShapeEditPannelWidget = new DAShapeEditPannelWidget(m_pannelEditWorkflow);
    m_pannelEditWorkflow->addWidget(m_editShapeEditPannelWidget, SARibbonPannelItem::Large);
    m_pannelEditWorkflow->addSeparator();
    m_editFontEditPannel = new DAFontEditPannelWidget(m_pannelEditWorkflow);
    m_pannelEditWorkflow->addWidget(m_editFontEditPannel, SARibbonPannelItem::Large);
    m_categoryEdit->addPannel(m_pannelEditWorkflow);
    //----------------------------------------------------------

    ribbonBar()->addCategoryPage(m_categoryEdit);  // 编辑

    // connect
    connect(m_editShapeEditPannelWidget, &DAShapeEditPannelWidget::borderPenChanged, this, &DAAppRibbonArea::selectedPen);
    connect(m_editShapeEditPannelWidget, &DAShapeEditPannelWidget::backgroundBrushChanged, this, &DAAppRibbonArea::selectedBrush);
    connect(m_editFontEditPannel, &DAFontEditPannelWidget::currentFontChanged, this, &DAAppRibbonArea::selectedFont);
    connect(m_editFontEditPannel, &DAFontEditPannelWidget::currentFontColorChanged, this, &DAAppRibbonArea::selectedFontColor);
}

void DAAppRibbonArea::buildRibbonFigureCategory()
{
    m_categoryFigure = ribbonBar()->addCategoryPage(tr("Figure"));  // cn:绘图
    m_categoryFigure->setObjectName(QStringLiteral("da-ribbon-category-figure"));
    m_pannelFigureSetting = new SARibbonPannel(m_categoryFigure);
    m_pannelFigureSetting->setObjectName(QStringLiteral("da-pannel-figure.fig_setting"));
    m_pannelFigureSetting->addLargeAction(m_actions->actionAddFigure);
    m_pannelFigureSetting->addLargeAction(m_actions->actionFigureResizeChart);
    m_pannelFigureSetting->addLargeAction(m_actions->actionFigureNewXYAxis);  // 新建坐标系
    m_categoryFigure->addPannel(m_pannelFigureSetting);

    m_pannelChartAdd = new SARibbonPannel(m_categoryFigure);
    m_pannelChartAdd->setObjectName(QStringLiteral("da-pannel-figure.chart-add"));
    m_pannelChartAdd->addLargeAction(m_actions->actionChartAddCurve);
    m_pannelChartAdd->addLargeAction(m_actions->actionChartAddScatter2D);
    m_categoryFigure->addPannel(m_pannelChartAdd);
}

/**
 * @brief 构建Workflow的上下文标签
 * @note 注意buildContextCategoryWorkflowEdit和buildContextCategoryWorkflowRun必须在此函数之后调用
 */
void DAAppRibbonArea::buildContextCategoryWorkflow()
{
    m_contextWorkflow = ribbonBar()->addContextCategory(tr("Workflow"));
    m_contextWorkflow->setObjectName(QStringLiteral("da-ribbon-contextcategory-workflow"));
    buildContextCategoryWorkflowEdit_();
    buildContextCategoryWorkflowRun_();
    ribbonBar()->showContextCategory(m_contextWorkflow);
}

/**
 * @brief 构建Workflow-编辑的上下文标签
 */
void DAAppRibbonArea::buildContextCategoryWorkflowEdit_()
{

    m_categoryWorkflowGraphicsEdit = m_contextWorkflow->addCategoryPage(tr("Workflow Edit"));
    m_categoryWorkflowGraphicsEdit->setObjectName(QStringLiteral("da-ribbon-category-workflow.edit"));
    // 条目pannel
    //  Item
    m_pannelWorkflowItem = m_categoryWorkflowGraphicsEdit->addPannel(tr("Item"));
    m_pannelWorkflowItem->setObjectName(QStringLiteral("da-pannel-context.workflow.item"));
    m_pannelWorkflowItem->addLargeAction(m_actions->actionWorkflowNew);
    m_pannelWorkflowItem->addSeparator();
    m_workflowShapeEditPannelWidget = new DAShapeEditPannelWidget(m_pannelWorkflowItem);
    m_pannelWorkflowItem->addWidget(m_workflowShapeEditPannelWidget, SARibbonPannelItem::Large);
    m_pannelWorkflowItem->addSeparator();
    m_workflowFontEditPannel = new DAFontEditPannelWidget(m_pannelWorkflowItem);
    m_pannelWorkflowItem->addWidget(m_workflowFontEditPannel, SARibbonPannelItem::Large);
    // Text
    m_pannelWorkflowText = m_categoryWorkflowGraphicsEdit->addPannel(tr("Text"));
    m_pannelWorkflowText->setObjectName(QStringLiteral("da-pannel-context.workflow.text"));
    m_pannelWorkflowText->addLargeAction(m_actions->actionWorkflowStartDrawRect);
    m_pannelWorkflowText->addLargeAction(m_actions->actionWorkflowStartDrawText);
    // Background
    m_pannelWorkflowBackground = m_categoryWorkflowGraphicsEdit->addPannel(tr("Background"));
    m_pannelWorkflowBackground->setObjectName(QStringLiteral("da-pannel-context.workflow.background"));
    m_pannelWorkflowBackground->addLargeAction(m_actions->actionWorkflowAddBackgroundPixmap);
    m_pannelWorkflowBackground->addMediumAction(m_actions->actionWorkflowLockBackgroundPixmap);
    m_pannelWorkflowBackground->addMediumAction(m_actions->actionWorkflowEnableItemMoveWithBackground);

    // View
    m_pannelWorkflowView = m_categoryWorkflowGraphicsEdit->addPannel(tr("View"));
    m_pannelWorkflowView->setObjectName(QStringLiteral("da-pannel-context.workflow.view"));
    m_pannelWorkflowView->addLargeAction(m_actions->actionWorkflowShowGrid);
    m_pannelWorkflowView->addSeparator();
    m_pannelWorkflowView->addLargeAction(m_actions->actionWorkflowWholeView);
    m_pannelWorkflowView->addMediumAction(m_actions->actionWorkflowZoomIn);
    m_pannelWorkflowView->addMediumAction(m_actions->actionWorkflowZoomOut);
    m_pannelWorkflowView->addSeparator();
    m_pannelWorkflowView->addMediumAction(m_actions->actionItemGrouping);
    m_pannelWorkflowView->addMediumAction(m_actions->actionItemUngroup);
    m_pannelWorkflowView->addLargeAction(m_actions->actionWorkflowEnableItemLinkageMove);
    //
    // connect
    connect(m_workflowShapeEditPannelWidget,
            &DAShapeEditPannelWidget::borderPenChanged,
            this,
            &DAAppRibbonArea::selectedWorkflowItemPen);
    connect(m_workflowShapeEditPannelWidget,
            &DAShapeEditPannelWidget::backgroundBrushChanged,
            this,
            &DAAppRibbonArea::selectedWorkflowItemBrush);
    connect(m_workflowFontEditPannel,
            &DAFontEditPannelWidget::currentFontChanged,
            this,
            &DAAppRibbonArea::selectedWorkflowItemFont);
    connect(m_workflowFontEditPannel,
            &DAFontEditPannelWidget::currentFontColorChanged,
            this,
            &DAAppRibbonArea::selectedWorkflowItemFontColor);
}

/**
 * @brief 构建workflow-运行的上下文标签
 */
void DAAppRibbonArea::buildContextCategoryWorkflowRun_()
{
    m_categoryWorkflowRun = m_contextWorkflow->addCategoryPage(tr("Workflow Run"));
    m_categoryWorkflowRun->setObjectName(QStringLiteral("da-ribbon-category-workflow.run"));
    // Run
    m_pannelWorkflowRun = m_categoryWorkflowRun->addPannel(tr("Run"));
    m_pannelWorkflowRun->setObjectName(QStringLiteral("da-pannel-context.workflow.run"));
    m_pannelWorkflowRun->addLargeAction(m_actions->actionWorkflowRun);
    m_pannelWorkflowRun->addLargeAction(m_actions->actionWorkflowTerminate);
}

/**
 * @brief 构建chart上下文
 */
void DAAppRibbonArea::buildContextCategoryChart()
{
    m_contextChart = ribbonBar()->addContextCategory(tr("Chart Edit"));  // cn:绘图编辑
    m_contextChart->setObjectName(QStringLiteral("da-ribbon-contextcategory-chart"));
    m_categoryChartEdit = m_contextChart->addCategoryPage(tr("Chart Edit"));
    m_categoryChartEdit->setObjectName(QStringLiteral("da-ribbon-category-chart.edit"));
    // fig edit
    m_pannelFigureSettingForContext = new SARibbonPannel(m_categoryChartEdit);
    m_pannelFigureSettingForContext->setObjectName(QStringLiteral("da-pannel-context-chartedit.fig_setting"));
    m_pannelFigureSettingForContext->addLargeAction(m_actions->actionAddFigure);
    m_pannelFigureSettingForContext->addLargeAction(m_actions->actionFigureResizeChart);
    m_pannelFigureSettingForContext->addLargeAction(m_actions->actionFigureNewXYAxis);  // 新建坐标系
    m_categoryChartEdit->addPannel(m_pannelFigureSettingForContext);
    // chart edit
    m_pannelChartSetting = new SARibbonPannel(m_categoryChartEdit);
    m_pannelChartSetting->setObjectName(QStringLiteral("da-pannel-context-chartedit.chart_setting"));
    // grid
    m_pannelChartSetting->addLargeAction(m_actions->actionChartEnableGrid);
    m_chartGridDirActionsButtonGroup = new SARibbonButtonGroupWidget(m_pannelChartSetting);
    m_chartGridDirActionsButtonGroup->addAction(m_actions->actionChartEnableGridX);
    m_chartGridDirActionsButtonGroup->addAction(m_actions->actionChartEnableGridY);
    m_pannelChartSetting->addSmallWidget(m_chartGridDirActionsButtonGroup);
    m_chartGridMinActionsButtonGroup = new SARibbonButtonGroupWidget(m_pannelChartSetting);
    m_chartGridMinActionsButtonGroup->addAction(m_actions->actionChartEnableGridXMin);
    m_chartGridMinActionsButtonGroup->addAction(m_actions->actionChartEnableGridYMin);
    m_pannelChartSetting->addSmallWidget(m_chartGridMinActionsButtonGroup);
    // pan
    m_pannelChartSetting->addLargeAction(m_actions->actionChartEnablePan);
    // 缩放
    m_pannelChartSetting->addLargeAction(m_actions->actionChartEnableZoom);
    m_pannelChartSetting->addMediumAction(m_actions->actionChartZoomIn);
    m_pannelChartSetting->addMediumAction(m_actions->actionChartZoomOut);
    m_pannelChartSetting->addLargeAction(m_actions->actionChartZoomAll);
    // picker
    m_pannelChartSetting->addLargeAction(m_actions->actionChartEnablePickerCross);
    m_pannelChartSetting->addLargeAction(m_actions->actionChartEnablePickerXY);
    m_pannelChartSetting->addLargeAction(m_actions->actionChartEnablePickerY);
    // legend
    m_actions->actionChartEnableLegend->setMenu(m_menuChartLegendProperty);
    m_pannelChartSetting->addLargeAction(m_actions->actionChartEnableLegend, QToolButton::MenuButtonPopup);

    m_categoryChartEdit->addPannel(m_pannelChartSetting);
}

void DAAppRibbonArea::buildApplicationMenu()
{
    mApplicationMenu = new DAAppRibbonApplicationMenu(app());
    mApplicationMenu->addAction(m_actions->actionOpen);
    mApplicationMenu->addAction(m_actions->actionSave);
    mApplicationMenu->addAction(m_actions->actionSaveAs);
    SARibbonApplicationButton* appBtn = qobject_cast< SARibbonApplicationButton* >(ribbonBar()->applicationButton());
    if (nullptr == appBtn) {
        return;
    }
    appBtn->setMenu(mApplicationMenu);
}

void DAAppRibbonArea::buildRightButtonBar()
{
    ribbonBar()->activeRightButtonGroup();
    SARibbonButtonGroupWidget* rbar = ribbonBar()->rightButtonGroup();
    rbar->addMenu(m_menuTheme);
}

/**
   @fn getEditPen
   @brief 获取编辑的画笔
   @return
 */

/**
   @fn getEditBrush
   @brief 画刷
   @return
 */

/**
   @fn getEditFont
   @brief 字体
   @return
 */

/**
   @fn getEditFontColor
   @brief 字体颜色
   @return
 */

/**
   @fn setEditPen
   @brief 设置画笔
   @param p
 */
DAAPPRIBBONAREA_COMMON_SETTING_CPP(Edit, m_editShapeEditPannelWidget, m_editFontEditPannel)
DAAPPRIBBONAREA_COMMON_SETTING_CPP(WorkFlowEdit, m_workflowShapeEditPannelWidget, m_workflowFontEditPannel)

AppMainWindow* DAAppRibbonArea::app() const
{
    return (m_app);
}

SARibbonBar* DAAppRibbonArea::ribbonBar() const
{
    return (app()->ribbonBar());
}

/**
 * @brief mian标签
 * @return
 */
SARibbonCategory* DAAppRibbonArea::getRibbonCategoryMain() const
{
    return (m_categoryMain);
}

/**
 * @brief 通过DACommandInterface构建redo/undo的action
 * @param cmd
 */
void DAAppRibbonArea::buildRedoUndo()
{
    QUndoGroup& undoGroup = m_appCmd->undoGroup();
    // 设置redo,undo的action
    m_actions->actionRedo = undoGroup.createRedoAction(this);
    m_actions->actionRedo->setObjectName("actionRedo");
    m_actions->actionRedo->setIcon(QIcon(":/Icon/Icon/redo.svg"));
    m_actions->actionUndo = undoGroup.createUndoAction(this);
    m_actions->actionUndo->setObjectName("actionUndo");
    m_actions->actionUndo->setIcon(QIcon(":/Icon/Icon/undo.svg"));
    SARibbonQuickAccessBar* bar = ribbonBar()->quickAccessBar();
    if (!bar) {
        return;
    }
    bar->addAction(m_actions->actionUndo);
    bar->addAction(m_actions->actionRedo);
}

void DAAppRibbonArea::updateActionLockBackgroundPixmapCheckStatue(bool c)
{
    //    QSignalBlocker l(m_actionLockBackgroundPixmap);
    //    m_actionLockBackgroundPixmap->setChecked(c);
}

/**
 * @brief 更新绘图相关的ribbon
 * @param fig
 */
void DAAppRibbonArea::updateFigureAboutRibbon(DAFigureWidget* fig)
{
    if (nullptr == fig) {
        qDebug() << "updateFigureAboutRibbon(fig:nullptr)";
        return;
    }
    DAChartWidget* chart = fig->getCurrentChart();
    updateChartAboutRibbon(chart);
}

/**
 * @brief 更新图表相关的ribbon
 * @param chart
 */
void DAAppRibbonArea::updateChartAboutRibbon(DAChartWidget* chart)
{
    if (nullptr == chart) {
        qDebug() << "updateChartAboutRibbon(chart:nullptr)";
        return;
    }
    qDebug() << "updateChartAboutRibbon";
    updateChartGridAboutRibbon(chart);
    updateChartZoomPanAboutRibbon(chart);
    updateChartPickerAboutRibbon(chart);
    updateChartLegendAboutRibbon(chart);
}

/**
 * @brief 更新Ribbon图表网格相关的界面
 * @param chart
 */
void DAAppRibbonArea::updateChartGridAboutRibbon(DAChartWidget* chart)
{
    if (nullptr == chart) {
        return;
    }
    m_actions->actionChartEnableGrid->setChecked(chart->isEnableGrid());
    m_actions->actionChartEnableGridX->setChecked(chart->isEnableGridX());
    m_actions->actionChartEnableGridY->setChecked(chart->isEnableGridY());
    m_actions->actionChartEnableGridXMin->setChecked(chart->isEnableGridXMin());
    m_actions->actionChartEnableGridYMin->setChecked(chart->isEnableGridYMin());
    bool c = m_actions->actionChartEnableGrid->isChecked();
    m_actions->actionChartEnableGridX->setEnabled(c);
    m_actions->actionChartEnableGridY->setEnabled(c);
    m_actions->actionChartEnableGridXMin->setEnabled(c);
    m_actions->actionChartEnableGridYMin->setEnabled(c);
}

/**
 * @brief 更新Ribbon图表缩放相关的界面
 * @param chart
 */
void DAAppRibbonArea::updateChartZoomPanAboutRibbon(DAChartWidget* chart)
{
    if (nullptr == chart) {
        return;
    }
    m_actions->actionChartEnableZoom->setChecked(chart->isEnableZoomer());
    m_actions->actionChartEnablePan->setChecked(chart->isEnablePanner());
}

/**
 * @brief 更新绘图的picker状态
 * @param chart
 */
void DAAppRibbonArea::updateChartPickerAboutRibbon(DAChartWidget* chart)
{
    if (nullptr == chart) {
        return;
    }
    m_actions->actionChartEnablePickerCross->setChecked(chart->isEnableCrossPicker());
    m_actions->actionChartEnablePickerY->setChecked(chart->isEnableYDataPicker());
    m_actions->actionChartEnablePickerXY->setChecked(chart->isEnableXYDataPicker());
}

/**
 * @brief 更新绘图的legend状态
 * @param chart
 */
void DAAppRibbonArea::updateChartLegendAboutRibbon(DAChartWidget* chart)
{
    if (nullptr == chart) {
        return;
    }
    QwtPlotLegendItem* legend = chart->getLegend();
    if (nullptr == legend) {
        m_actions->actionChartEnableLegend->setChecked(false);
        return;
    }
    m_actions->actionChartEnableLegend->setChecked(legend->isVisible());
    Qt::Alignment al    = legend->alignmentInCanvas();
    bool needIteActions = true;
    QAction* ca         = m_actions->actionGroupChartLegendAlignment->checkedAction();
    if (ca) {
        if (ca->data().toInt() == static_cast< int >(al)) {
            // 状态不变，无需迭代
            needIteActions = false;
        }
    }
    if (needIteActions) {
        // 需要迭代所有情况
        QList< QAction* > acts = m_actions->actionGroupChartLegendAlignment->actions();
        for (QAction* a : qAsConst(acts)) {
            if (a->data().toInt() == static_cast< int >(al)) {
                a->setChecked(true);
                break;
            }
        }
    }
    QSignalBlocker b1(m_spinboxChartLegendMaxColumns);
    m_spinboxChartLegendMaxColumns->setValue(legend->maxColumns());
    QSignalBlocker b2(m_spinboxChartLegendMargin);
    m_spinboxChartLegendMargin->setValue(legend->margin());
    QSignalBlocker b3(m_spinboxChartLegendSpacing);
    m_spinboxChartLegendSpacing->setValue(legend->spacing());
    QSignalBlocker b4(m_spinboxChartLegendItemMargin);
    m_spinboxChartLegendItemMargin->setValue(legend->itemMargin());
    QSignalBlocker b5(m_spinboxChartLegendItemSpacing);
    m_spinboxChartLegendItemSpacing->setValue(legend->itemSpacing());
    QSignalBlocker b6(m_spinboxChartLegendBorderRadius);
    m_spinboxChartLegendBorderRadius->setValue(legend->borderRadius());
}

/**
 * @brief 显示上下文(会把其他上下文隐藏)
 * @param type
 */
void DAAppRibbonArea::showContextCategory(DAAppRibbonArea::ContextCategoryType type)
{
    SARibbonBar* ribbon = ribbonBar();
    switch (type) {
    case ContextCategoryData: {
        ribbon->showContextCategory(m_contextDataFrame);
        ribbon->hideContextCategory(m_contextWorkflow);
        ribbon->hideContextCategory(m_contextChart);
    } break;
    case ContextCategoryWorkflow: {
        ribbon->showContextCategory(m_contextWorkflow);
        ribbon->hideContextCategory(m_contextDataFrame);
        ribbon->hideContextCategory(m_contextChart);
    } break;
    case ContextCategoryChart: {
        ribbon->hideContextCategory(m_contextDataFrame);
        ribbon->hideContextCategory(m_contextWorkflow);
        ribbon->showContextCategory(m_contextChart);
    } break;
    case AllContextCategory: {
        ribbon->showContextCategory(m_contextDataFrame);
        ribbon->showContextCategory(m_contextWorkflow);
        ribbon->showContextCategory(m_contextChart);
    } break;
    default:
        break;
    }
}

/**
 * @brief 隐藏上下文
 * @param type
 */
void DAAppRibbonArea::hideContextCategory(DAAppRibbonArea::ContextCategoryType type)
{
    SARibbonBar* ribbon = ribbonBar();
    switch (type) {
    case ContextCategoryData: {
        ribbon->hideContextCategory(m_contextDataFrame);
    } break;
    case ContextCategoryWorkflow: {
        ribbon->hideContextCategory(m_contextWorkflow);
    } break;
    case ContextCategoryChart: {
        ribbon->hideContextCategory(m_contextChart);
    } break;
    case AllContextCategory: {
        ribbon->hideContextCategory(m_contextDataFrame);
        ribbon->hideContextCategory(m_contextWorkflow);
        ribbon->hideContextCategory(m_contextChart);
    } break;
    default:
        break;
    }
}
#if DA_ENABLE_PYTHON
/**
 * @brief 设置DataFrame的类型，[Context Category - dataframe] [Type] -> Type
 * @param d
 */
void DAAppRibbonArea::setDataframeOperateCurrentDType(const DAPyDType& d)
{
    // 先阻塞
    QSignalBlocker blocker(m_comboxColumnTypes);
    Q_UNUSED(blocker);
    m_comboxColumnTypes->setCurrentDType(d);
}
#endif
