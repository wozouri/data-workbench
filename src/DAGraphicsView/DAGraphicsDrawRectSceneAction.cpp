﻿#include "DAGraphicsDrawRectSceneAction.h"
#include "DAGraphicsScene.h"
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include "DAGraphicsRectItem.h"
#include "DAGraphicsRubberBandItem.h"
// 通过任意两点，获取构成矩形的尺寸
namespace DA
{
DAGraphicsDrawRectSceneAction::DAGraphicsDrawRectSceneAction(DAGraphicsScene* sc) : DAAbstractGraphicsSceneAction(sc)
{
	static QPixmap s_default_cursor_pixmap = svgToPixmap(":/DAGraphicsView/svg/draw-rect.svg", QSize(20, 20));
	setCursorPixmap(s_default_cursor_pixmap);
}

DAGraphicsDrawRectSceneAction::~DAGraphicsDrawRectSceneAction()
{
	restoreCursor();
}

void DAGraphicsDrawRectSceneAction::beginActive()
{
	// 开始激活，把cursor设置为绘制矩形样式
	setupCursor();
}

bool DAGraphicsDrawRectSceneAction::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
	if (mouseEvent->button() == Qt::LeftButton) {
		// 左键点击
		if (!mHaveBeingPressed) {
			mStartPoint       = mouseEvent->scenePos();
			mHaveBeingPressed = true;
			if (!mRubberBand) {
				mRubberBand = std::make_unique< DAGraphicsRubberBandItem >();
				scene()->addItem(mRubberBand.get());
			}
			mRubberBand->setBeginScenePos(mStartPoint);

			return true;
		} else {
			// 说明已经点击过，这里是要完成矩形的创建
			QPointF endPos           = mouseEvent->scenePos();
			QPointF rectPos          = topLeftPoint(mStartPoint, endPos);
			DAGraphicsRectItem* item = scene()->createRect_(rectPos);
			item->setBodySize(pointRectSize(mStartPoint, endPos));
			item->setSelected(true);
			mRubberBand->hide();
			scene()->removeItem(mRubberBand.get());
			mRubberBand.reset();
			end();
			return true;
		}
	} else if (mouseEvent->button() == Qt::RightButton) {
		end();
		return false;
	}
	return false;
}

bool DAGraphicsDrawRectSceneAction::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
	if (mRubberBand) {
		mRubberBand->setCurrentMousePos(mouseEvent->scenePos());
	}
	return DAAbstractGraphicsSceneAction::mouseMoveEvent(mouseEvent);
}

}  // end namespace DA
