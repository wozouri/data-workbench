﻿#include "DAGraphicsMouseCrossLineViewAction.h"
#include <QPaintEvent>
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QRegion>
#include "DAGraphicsView.h"
namespace DA
{
DAGraphicsMouseCrossLineViewAction::DAGraphicsMouseCrossLineViewAction(DAGraphicsView* v, ActionTypes type)
	: DAAbstractGraphicsViewAction(v), mType(type)
{
}

DAGraphicsMouseCrossLineViewAction::~DAGraphicsMouseCrossLineViewAction()
{
}

void DAGraphicsMouseCrossLineViewAction::setActionTypes(ActionTypes t)
{
	mType = t;
}

DAGraphicsMouseCrossLineViewAction::ActionTypes DAGraphicsMouseCrossLineViewAction::getActionTypes() const
{
	return mType;
}

void DAGraphicsMouseCrossLineViewAction::setDrawPen(const QPen& c)
{
	mDrawPen = c;
}

QPen DAGraphicsMouseCrossLineViewAction::getDrawPen() const
{
	return mDrawPen;
}

void DAGraphicsMouseCrossLineViewAction::setDrawColor(const QColor& c)
{
	mDrawPen.setColor(c);
}

QColor DAGraphicsMouseCrossLineViewAction::getDrawColor() const
{
	return mDrawPen.color();
}

void DAGraphicsMouseCrossLineViewAction::setCrossViewPos(const QPoint& p)
{
	mPos = p;
}

QPoint DAGraphicsMouseCrossLineViewAction::getCrossViewPos() const
{
	return mPos;
}

void DAGraphicsMouseCrossLineViewAction::setCrossScenePos(const QPointF& p)
{
	DAGraphicsView* v = view();
	QRectF f(0, 0, 100, 100);
	v->ensureVisible(f);
	QPoint vp = v->mapFromScene(p);
	f.moveCenter(p);
	mPos = vp;
}

void DAGraphicsMouseCrossLineViewAction::setValid(bool on)
{
	mValid = on;
}

bool DAGraphicsMouseCrossLineViewAction::isValid() const
{
	return mValid;
}

void DAGraphicsMouseCrossLineViewAction::beginActive()
{
	setValid(true);
}

void DAGraphicsMouseCrossLineViewAction::endAction()
{
}

void DAGraphicsMouseCrossLineViewAction::paintCross(QPainter* painter, const QPoint& point, const QRect& viewRect)
{
	// 绘制水平
	painter->drawLine(QPoint(viewRect.x(), point.y()), QPoint(viewRect.x() + viewRect.width(), point.y()));
	// 绘制竖直线
	painter->drawLine(QPoint(point.x(), viewRect.y()), QPoint(point.x(), viewRect.y() + viewRect.height()));
}

void DAGraphicsMouseCrossLineViewAction::paintEvent(QPaintEvent* event)
{
	if (!mValid) {
		return;
	}
	QRect viewRect = view()->rect();
	if (mPos.isNull() || !viewRect.contains(mPos)) {
		return;
	}
	QPainter painter(view()->viewport());
	painter.setPen(mDrawPen);
	paintCross(&painter, mPos, viewRect);
}

bool DAGraphicsMouseCrossLineViewAction::keyPressEvent(QKeyEvent* event)
{
	if (OneTimeMarking == mType) {
		setValid(false);
	}
	return false;
}

bool DAGraphicsMouseCrossLineViewAction::mousePressEvent(QMouseEvent* event)
{
	if (OneTimeMarking == mType) {
		setValid(false);
	} else if (FollowMouse == mType) {
		// 更新位置
		mPos = event->pos();
	}
	return false;
}

bool DAGraphicsMouseCrossLineViewAction::mouseReleaseEvent(QMouseEvent* event)
{
	if (OneTimeMarking == mType) {
		setValid(false);
	} else if (FollowMouse == mType) {
		// 更新位置
		mPos = event->pos();
	}
	return false;
}

bool DAGraphicsMouseCrossLineViewAction::mouseMoveEvent(QMouseEvent* event)
{
	if (FollowMouse == mType) {
		// 更新位置
		mPos = event->pos();
	}
	return false;
}

}  // end DA
