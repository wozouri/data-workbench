﻿#include "DAChartAxisSetWidget.h"
#include "ui_DAChartAxisSetWidget.h"
#include "qwt_plot.h"
#include "qwt_scale_div.h"
#include "qwt_scale_draw.h"
#include "qwt_scale_widget.h"
#include "qwt_date_scale_draw.h"
#include <QButtonGroup>
#include "DAChartUtil.h"
#include <QDebug>
namespace DA
{

DAChartAxisSetWidget::DAChartAxisSetWidget(QWidget* parent)
	: QWidget(parent), ui(new Ui::DAChartAxisSetWidget), m_chart(nullptr), m_axisID(QwtPlot::axisCnt)
{
	ui->setupUi(this);
	m_buttonGroup = new QButtonGroup(this);
	m_buttonGroup->addButton(ui->radioButtonNormal, NormalScale);
	m_buttonGroup->addButton(ui->radioButtonTimeScale, DateTimeScale);
	ui->radioButtonNormal->setChecked(true);
	ui->dateTimeScaleSetWidget->hide();

	connect(ui->checkBoxEnable, &QCheckBox::stateChanged, this, &DAChartAxisSetWidget::onEnableCheckBoxClicked);
	connect(ui->lineEditTitle, &QLineEdit::textChanged, this, &DAChartAxisSetWidget::onLineEditTextChanged);
	connect(ui->fontSetWidget, &DAFontEditPannelWidget::currentFontChanged, this, &DAChartAxisSetWidget::onAxisFontChanged);
	connect(ui->aligmentSetWidget,
			&DAAligmentEditWidget::alignmentChanged,
			this,
			&DAChartAxisSetWidget::onAxisLabelAligmentChanged);
	connect(ui->doubleSpinBoxRotation,
			static_cast< void (QDoubleSpinBox::*)(double v) >(&QDoubleSpinBox::valueChanged),
			this,
			&DAChartAxisSetWidget::onAxisLabelRotationChanged);
	connect(ui->spinBoxMargin,
			static_cast< void (QSpinBox::*)(int v) >(&QSpinBox::valueChanged),
			this,
			&DAChartAxisSetWidget::onAxisMarginValueChanged);
	connect(ui->doubleSpinBoxMax,
			static_cast< void (QDoubleSpinBox::*)(double v) >(&QDoubleSpinBox::valueChanged),
			this,
			&DAChartAxisSetWidget::onAxisMaxScaleChanged);
	connect(ui->doubleSpinBoxMin,
			static_cast< void (QDoubleSpinBox::*)(double v) >(&QDoubleSpinBox::valueChanged),
			this,
			&DAChartAxisSetWidget::onAxisMinScaleChanged);
	connect(m_buttonGroup,
			static_cast< void (QButtonGroup::*)(int) >(&QButtonGroup::buttonClicked),
			this,
			&DAChartAxisSetWidget::onScaleStyleChanged);
	enableWidget(false);
}

DAChartAxisSetWidget::~DAChartAxisSetWidget()
{
	delete ui;
}

void DAChartAxisSetWidget::onEnableCheckBoxClicked(int state)
{
	bool enable = (Qt::Checked == state);
	foreach (QObject* obj, children()) {
		if (obj->isWidgetType()) {
			qobject_cast< QWidget* >(obj)->setEnabled(enable);
		}
	}
	ui->checkBoxEnable->setEnabled(true);
	if (m_chart) {
		m_chart->enableAxis(m_axisID, enable);
	}
	emit enableAxis(Qt::Checked == state, m_axisID);
}

void DAChartAxisSetWidget::onLineEditTextChanged(const QString& text)
{
	if (m_chart) {
		DAChartUtil::setAxisTitle(m_chart, m_axisID, text);
	}
}

void DAChartAxisSetWidget::onAxisFontChanged(const QFont& font)
{
	if (m_chart) {
		DAChartUtil::setAxisFont(m_chart, m_axisID, font);
	}
}

void DAChartAxisSetWidget::onAxisLabelAligmentChanged(Qt::Alignment al)
{
	if (m_chart) {
		DAChartUtil::setAxisLabelAlignment(m_chart, m_axisID, al);
	}
}

void DAChartAxisSetWidget::onAxisLabelRotationChanged(double v)
{
	if (m_chart) {
		DAChartUtil::setAxisLabelRotation(m_chart, m_axisID, v);
	}
}

void DAChartAxisSetWidget::onAxisMarginValueChanged(int v)
{
	if (m_chart) {
		DAChartUtil::setAxisMargin(m_chart, m_axisID, v);
	}
}

void DAChartAxisSetWidget::onAxisMaxScaleChanged(double v)
{
	if (m_chart) {
		DAChartUtil::setAxisScaleMax(m_chart, m_axisID, v);
	}
}

void DAChartAxisSetWidget::onAxisMinScaleChanged(double v)
{
	if (m_chart) {
		DAChartUtil::setAxisScaleMin(m_chart, m_axisID, v);
	}
}

void DAChartAxisSetWidget::onScaleDivChanged()
{
	if (nullptr == m_chart) {
		return;
	}
	QwtInterval inv = m_chart->axisInterval(m_axisID);
	ui->doubleSpinBoxMin->setValue(inv.minValue());
	ui->doubleSpinBoxMax->setValue(inv.maxValue());
}

void DAChartAxisSetWidget::onScaleStyleChanged(int id)
{
	if (NormalScale == id) {
		ui->dateTimeScaleSetWidget->hide();
		if (m_chart) {
			DAChartUtil::setAxisNormalScale(m_chart, m_axisID);
		}
	} else {
		if (DateTimeScale == id) {
			ui->dateTimeScaleSetWidget->show();
			if (m_chart) {
				QString format = ui->dateTimeScaleSetWidget->getTimeFormat();
				DAChartUtil::setAxisDateTimeScale(m_chart, m_axisID, format);
			}
		}
	}
}

void DAChartAxisSetWidget::updateUI()
{
	bool enable = ui->checkBoxEnable->isChecked();
	foreach (QObject* obj, children()) {
		if (obj->isWidgetType()) {
			qobject_cast< QWidget* >(obj)->setEnabled(enable);
		}
	}
	ui->checkBoxEnable->setEnabled(true);
}

void DAChartAxisSetWidget::updateAxisValue(QwtPlot* chart, int axisID)
{
	enableWidget(nullptr != chart);
	if (nullptr == chart) {
		resetAxisValue();
		return;
	}
	bool b = chart->axisEnabled(axisID);
	ui->checkBoxEnable->setChecked(b);
	onEnableCheckBoxClicked(b ? Qt::Checked : Qt::Unchecked);
	ui->lineEditTitle->setText(chart->axisTitle(axisID).text());
	ui->fontSetWidget->setCurrentFont(chart->axisFont(axisID));

	QwtInterval inv = chart->axisInterval(axisID);
	ui->doubleSpinBoxMin->setValue(inv.minValue());
	ui->doubleSpinBoxMax->setValue(inv.maxValue());
	ui->doubleSpinBoxMin->setDecimals(inv.minValue() < 0.01 ? 5 : 2);  // 显示小数点的位数调整
	ui->doubleSpinBoxMax->setDecimals(inv.minValue() < 0.01 ? 5 : 2);

	QwtScaleWidget* ax = chart->axisWidget(axisID);
	if (nullptr == ax) {
		ui->radioButtonNormal->setChecked(true);
		return;
	}
	QwtScaleDraw* sd = ax->scaleDraw();
	if (sd) {
		ui->doubleSpinBoxRotation->setValue(sd->labelRotation());
		ui->labelAligment->setAlignment(sd->labelAlignment());
	}
	ui->spinBoxMargin->setValue(ax->margin());
	QwtDateScaleDraw* dsd = dynamic_cast< QwtDateScaleDraw* >(sd);

	if (dsd) {
		ui->radioButtonTimeScale->setChecked(true);
		ui->dateTimeScaleSetWidget->setTimeFormatText(dsd->dateFormat(QwtDate::Second));
	} else {
		ui->radioButtonNormal->setChecked(true);
	}
}

void DAChartAxisSetWidget::resetAxisValue()
{
	ui->checkBoxEnable->setChecked(false);
	ui->lineEditTitle->setText("");
	ui->fontSetWidget->setCurrentFont(QFont());
	ui->doubleSpinBoxMin->setValue(0);
	ui->doubleSpinBoxMax->setValue(0);
	ui->radioButtonNormal->setChecked(true);
	ui->doubleSpinBoxRotation->setValue(0);
	ui->labelAligment->setAlignment(Qt::AlignLeft);
	ui->spinBoxMargin->setValue(0);
	ui->radioButtonTimeScale->setChecked(false);
	ui->dateTimeScaleSetWidget->setTimeFormatText("");
}

void DAChartAxisSetWidget::enableWidget(bool enable)
{
	ui->checkBoxEnable->setEnabled(enable);
	ui->lineEditTitle->setEnabled(enable);
	ui->fontSetWidget->setEnabled(enable);
	ui->doubleSpinBoxMin->setEnabled(enable);
	ui->doubleSpinBoxMax->setEnabled(enable);
	ui->radioButtonNormal->setEnabled(enable);
	ui->doubleSpinBoxRotation->setEnabled(enable);
	ui->labelAligment->setEnabled(enable);
	ui->spinBoxMargin->setEnabled(enable);
	ui->radioButtonTimeScale->setEnabled(enable);
	ui->dateTimeScaleSetWidget->setEnabled(enable);
}

void DAChartAxisSetWidget::connectChartAxis()
{
	if (nullptr == m_chart) {
		return;
	}
	QwtScaleWidget* aw = m_chart->axisWidget(m_axisID);
	if (aw) {
		connect(aw, &QwtScaleWidget::scaleDivChanged, this, &DAChartAxisSetWidget::onScaleDivChanged);
	}
}

void DAChartAxisSetWidget::disconnectChartAxis()
{
	if (nullptr == m_chart) {
		return;
	}
	QwtScaleWidget* aw = m_chart->axisWidget(m_axisID);
	if (aw) {
		disconnect(aw, &QwtScaleWidget::scaleDivChanged, this, &DAChartAxisSetWidget::onScaleDivChanged);
	}
}

QwtPlot* DAChartAxisSetWidget::getChart() const
{
	return m_chart;
}

void DAChartAxisSetWidget::setChart(QwtPlot* chart, int axisID)
{
	if (chart && m_chart && (m_chart != chart)) {
		disconnectChartAxis();
	}

	m_chart = nullptr;  // 先设置为null，使得槽函数不动作
	updateAxisValue(chart, axisID);
	m_chart  = chart;
	m_axisID = axisID;
	connectChartAxis();
}

void DAChartAxisSetWidget::updateAxisValue()
{
	updateAxisValue(m_chart, m_axisID);
}

}  // end DA
