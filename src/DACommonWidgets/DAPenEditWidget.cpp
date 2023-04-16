﻿#include "DAPenEditWidget.h"
#include <QVariant>
#include <QApplication>
#include <QSignalBlocker>
#include <QHBoxLayout>
#include <QSpinBox>
#include "DAColorPickerButton.h"
#include "DAPenStyleComboBox.h"
#include "colorWidgets/SAColorMenu.h"
namespace DA
{
class DAPenEditWidget::PrivateData
{
    DA_DECLARE_PUBLIC(DAPenEditWidget)
public:
    PrivateData(DAPenEditWidget* p);

public:
    QPen mPen;
    QHBoxLayout* horizontalLayout;
    DAColorPickerButton* colorButton;
    DAPenStyleComboBox* comboBox;
    QSpinBox* spinBoxWidth;
    SAColorMenu* mColorMenu { nullptr };
};

//===================================================
// DAPenEditWidgetPrivate
//===================================================

DAPenEditWidget::PrivateData::PrivateData(DAPenEditWidget* p) : q_ptr(p)
{
    if (p->objectName().isEmpty())
        p->setObjectName(QStringLiteral("DAPenEditWidget"));
    p->resize(166, 25);
    horizontalLayout = new QHBoxLayout(p);
    horizontalLayout->setSpacing(1);
    horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
    horizontalLayout->setContentsMargins(1, 1, 1, 1);
    colorButton = new DAColorPickerButton(p);
    colorButton->setAutoRaise(true);
    colorButton->setColor(QColor());
    colorButton->setObjectName(QStringLiteral("pushButtonColor"));
    colorButton->setPopupMode(QToolButton::MenuButtonPopup);
    mColorMenu = new SAColorMenu(p);
    mColorMenu->bindToColorToolButton(colorButton);
    horizontalLayout->addWidget(colorButton);

    comboBox = new DAPenStyleComboBox(p);
    comboBox->setObjectName(QStringLiteral("comboBox"));
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(comboBox->sizePolicy().hasHeightForWidth());
    comboBox->setSizePolicy(sizePolicy);

    horizontalLayout->addWidget(comboBox);

    spinBoxWidth = new QSpinBox(p);
    spinBoxWidth->setObjectName(QStringLiteral("spinBoxWidth"));

    horizontalLayout->addWidget(spinBoxWidth);
}

//===================================================
// DAPenEditWidget
//===================================================

DAPenEditWidget::DAPenEditWidget(QWidget* parent) : QWidget(parent), DA_PIMPL_CONSTRUCT
{
    QPen p;
    p.setStyle(Qt::SolidLine);
    p.setColor(QColor());
    p.setWidth(1);
    setCurrentPen(p);
    init();
}

DAPenEditWidget::DAPenEditWidget(const QPen& p, QWidget* parent) : QWidget(parent), d_ptr(new PrivateData(this))
{
    setCurrentPen(p);
    init();
}

DAPenEditWidget::~DAPenEditWidget()
{
}

/**
 * @brief 设置画笔,设置画笔会触发penChanged信号
 * @param p
 */
void DAPenEditWidget::setCurrentPen(const QPen& p)
{
    QSignalBlocker bl(d_ptr->colorButton);
    QSignalBlocker bl2(d_ptr->comboBox);
    QSignalBlocker bl3(d_ptr->spinBoxWidth);
    Q_UNUSED(bl);
    Q_UNUSED(bl2);
    Q_UNUSED(bl3);
    d_ptr->mPen = p;
    d_ptr->colorButton->setColor(d_ptr->mPen.color());
    d_ptr->colorButton->setEnabled(d_ptr->mPen.style() != Qt::NoPen);
    d_ptr->comboBox->setPenColor(d_ptr->mPen.color());
    d_ptr->comboBox->setCurrentPenStyle(d_ptr->mPen.style());
    d_ptr->comboBox->setPenLineWidth(d_ptr->mPen.width());
    d_ptr->comboBox->updateItems();
    d_ptr->spinBoxWidth->setValue(d_ptr->mPen.width());
    emit penChanged(d_ptr->mPen);
}

void DAPenEditWidget::retranslateUi()
{
    setWindowTitle(QCoreApplication::translate("DAPenEditWidget", "Pen Editor", nullptr));
    d_ptr->spinBoxWidth->setSuffix(QCoreApplication::translate("DAPenEditWidget", " px", nullptr));
}

QPen DAPenEditWidget::getCurrentPen() const
{
    return d_ptr->mPen;
}

void DAPenEditWidget::onColorChanged(const QColor& c)
{
    d_ptr->mPen.setColor(c);
    d_ptr->comboBox->setPenColor(c);
    d_ptr->comboBox->updateItems();
    emit penChanged(d_ptr->mPen);
}

void DAPenEditWidget::onPenWidthValueChanged(int w)
{
    d_ptr->mPen.setWidth(w);
    d_ptr->comboBox->setPenLineWidth(w);
    d_ptr->comboBox->updateItems();
    emit penChanged(d_ptr->mPen);
}

void DAPenEditWidget::onPenStyleChanged(Qt::PenStyle s)
{
    d_ptr->mPen.setStyle(s);
    emit penChanged(d_ptr->mPen);
}

void DAPenEditWidget::init()
{
    d_ptr->comboBox->setPenColor(d_ptr->mPen.color());
    d_ptr->comboBox->setPenLineWidth(d_ptr->mPen.width());
    d_ptr->comboBox->updateItems();
    connect(d_ptr->colorButton, &DAColorPickerButton::colorChanged, this, &DAPenEditWidget::onColorChanged);
    connect(d_ptr->spinBoxWidth, QOverload< int >::of(&QSpinBox::valueChanged), this, &DAPenEditWidget::onPenWidthValueChanged);
    connect(d_ptr->comboBox, &DAPenStyleComboBox::currentPenStyleChanged, this, &DAPenEditWidget::onPenStyleChanged);
}
}  // namespace DA
