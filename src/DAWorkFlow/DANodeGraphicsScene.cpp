﻿#include "DANodeGraphicsScene.h"
#include <QGraphicsSceneMouseEvent>
#include <QScopedPointer>
#include <QPointer>
#include <QPainter>
#include "DAGraphicsPixmapItem.h"
#include "DAStandardNodeLinkGraphicsItem.h"
#include "DACommandsForGraphics.h"
#include "DACommandsForWorkFlowNodeGraphics.h"
#include "DAGraphicsTextItem.h"
#include "DAGraphicsStandardTextItem.h"
#include "DAAbstractNode.h"
#include "DAWorkFlow.h"
#include "DAAbstractNodeFactory.h"
#include "DAGraphicsRectItem.h"
#include "DANodeGraphicsSceneEventListener.h"
#include <QPointer>

#define DANODEGRAPHICSSCENE_EVENTLISTENER(eventName, e)                                                                \
    for (int i = 0; i < d_ptr->mEventListeners.size(); ++i) {                                                          \
        if (d_ptr->mEventListeners[ i ]) {                                                                             \
            d_ptr->mEventListeners[ i ]->eventName(this, e);                                                           \
        }                                                                                                              \
    }

namespace DA
{
class DANodeGraphicsScene::PrivateData
{
    DA_DECLARE_PUBLIC(DANodeGraphicsScene)
public:
    PrivateData(DANodeGraphicsScene* p);

public:
    QPointer< DAWorkFlow > mWorkflow;
    QVector< QPointer< DANodeGraphicsSceneEventListener > > mEventListeners;
};

DANodeGraphicsScene::PrivateData::PrivateData(DANodeGraphicsScene* p) : q_ptr(p)
{
}

////////////////////////////////////////////////
/// DANodeGraphicsScene
////////////////////////////////////////////////

DANodeGraphicsScene::DANodeGraphicsScene(QObject* p) : DAGraphicsScene(p), DA_PIMPL_CONSTRUCT
{
    initConnect();
}

DANodeGraphicsScene::DANodeGraphicsScene(const QRectF& sceneRect, QObject* p)
    : DAGraphicsScene(sceneRect, p), DA_PIMPL_CONSTRUCT
{
    initConnect();
}

DANodeGraphicsScene::DANodeGraphicsScene(qreal x, qreal y, qreal width, qreal height, QObject* p)
    : DAGraphicsScene(x, y, width, height, p), DA_PIMPL_CONSTRUCT
{
    initConnect();
}

DANodeGraphicsScene::~DANodeGraphicsScene()
{
}

/**
 * @brief 取消链接，如果此时没在进行链接，不做处理
 */
void DANodeGraphicsScene::cancelLink()
{
    DAAbstractNodeLinkGraphicsItem* linkItem = dynamic_cast< DAAbstractNodeLinkGraphicsItem* >(getCurrentLinkItem());
    if (linkItem) {
        DAAbstractNodeGraphicsItem* it = linkItem->fromNodeItem();
        if (it) {
            it->finishLink(linkItem->fromNodeLinkPoint(), linkItem, DANodeLinkPoint::Output, false);
        }
        it = linkItem->toNodeItem();
        if (it) {
            it->finishLink(linkItem->toNodeLinkPoint(), linkItem, DANodeLinkPoint::Input, false);
        }
    }
    DAGraphicsScene::cancelLink();
}

/**
 * @brief 设置工作流
 * @param wf
 * @note DANodeGraphicsScene不负责管理DAWorkFlow的所有权
 */
void DANodeGraphicsScene::setWorkFlow(DAWorkFlow* wf)
{
    if (d_ptr->mWorkflow) {
        disconnect(d_ptr->mWorkflow.data(), &DAWorkFlow::nodeNameChanged, this, &DANodeGraphicsScene::onNodeNameChanged);
    }
    d_ptr->mWorkflow = wf;
    if (wf) {
        connect(wf, &DAWorkFlow::nodeNameChanged, this, &DANodeGraphicsScene::onNodeNameChanged);
        QList< DAAbstractNodeFactory* > factorys = wf->getAllFactorys();
        for (DAAbstractNodeFactory* f : factorys) {
            DANodeGraphicsSceneEventListener* listen = f->createNodeGraphicsSceneEventListener();
            if (listen) {
                listen->factoryAddedToScene(this);
                d_ptr->mEventListeners.append(listen);
            }
        }
    }
}

/**
 * @brief 获取工作流
 * @return
 */
DAWorkFlow* DANodeGraphicsScene::getWorkflow()
{
    return d_ptr->mWorkflow.data();
}

/**
 * @brief 通过node找到item
 * @param n
 * @return
 */
DAAbstractNodeGraphicsItem* DANodeGraphicsScene::findItemByNode(DAAbstractNode* n)
{
    QList< QGraphicsItem* > its = items();
    for (QGraphicsItem* i : qAsConst(its)) {
        DAAbstractNodeGraphicsItem* ni = dynamic_cast< DAAbstractNodeGraphicsItem* >(i);
        if (ni) {
            if (ni->rawNode() == n) {
                return ni;
            }
        }
    }
    return nullptr;
}

/**
 * @brief 获取选中的NodeGraphicsItem,如果没有返回nullptr，如果选中多个，返回第一个
 * @return
 */
DAAbstractNodeGraphicsItem* DANodeGraphicsScene::getSelectedNodeGraphicsItem() const
{
    QList< QGraphicsItem* > sits = selectedItems();
    for (QGraphicsItem* i : qAsConst(sits)) {
        if (DAAbstractNodeGraphicsItem* gi = dynamic_cast< DAAbstractNodeGraphicsItem* >(i)) {
            return gi;
        }
    }
    return nullptr;
}

/**
 * @brief 获取所有的NodeGraphicsItems
 * @return
 */
QList< DAAbstractNodeGraphicsItem* > DANodeGraphicsScene::getNodeGraphicsItems() const
{
    QList< DAAbstractNodeGraphicsItem* > res;
    QList< QGraphicsItem* > its = topItems();
    for (QGraphicsItem* i : qAsConst(its)) {
        DAAbstractNodeGraphicsItem* ni = dynamic_cast< DAAbstractNodeGraphicsItem* >(i);
        if (ni) {
            res.append(ni);
        }
    }
    return res;
}

/**
 * @brief 获取所有的文本
 * @return
 * @sa createText_
 */
QList< DAGraphicsStandardTextItem* > DANodeGraphicsScene::getTextGraphicsItems() const
{
    QList< DAGraphicsStandardTextItem* > res;
    QList< QGraphicsItem* > its = topItems();
    for (QGraphicsItem* i : qAsConst(its)) {
        DAGraphicsStandardTextItem* ni = dynamic_cast< DAGraphicsStandardTextItem* >(i);
        if (ni) {
            res.append(ni);
        }
    }
    return res;
}

/**
 * @brief 获取选中的NodeGraphicsItem,如果没有返回一个空list
 * @return
 */
QList< DAAbstractNodeGraphicsItem* > DANodeGraphicsScene::getSelectedNodeGraphicsItems() const
{
    QList< DAAbstractNodeGraphicsItem* > res;
    QList< QGraphicsItem* > sits = selectedItems();
    for (QGraphicsItem* i : qAsConst(sits)) {
        if (DAAbstractNodeGraphicsItem* gi = dynamic_cast< DAAbstractNodeGraphicsItem* >(i)) {
            res.append(gi);
        }
    }
    return res;
}
/**
 * @brief 获取选中的NodeLinkGraphicsItem,如果没有返回nullptr，如果选中多个，返回第一个
 * @return
 */
DAAbstractNodeLinkGraphicsItem* DANodeGraphicsScene::getSelectedNodeLinkGraphicsItem() const
{
    QList< QGraphicsItem* > sits = selectedItems();
    for (QGraphicsItem* i : qAsConst(sits)) {
        if (DAAbstractNodeLinkGraphicsItem* gi = dynamic_cast< DAAbstractNodeLinkGraphicsItem* >(i)) {
            return gi;
        }
    }
    return nullptr;
}

/**
 * @brief 获取除了连接线以外的item
 * @return
 */
QList< QGraphicsItem* > DANodeGraphicsScene::getGraphicsItemsWithoutLink() const
{
    QList< QGraphicsItem* > res;
    QList< QGraphicsItem* > ites = topItems();
    for (QGraphicsItem* i : qAsConst(ites)) {
        if (nullptr == dynamic_cast< DAAbstractNodeLinkGraphicsItem* >(i)) {
            res.append(i);
        }
    }
    return res;
}

/**
 * @brief 删除选中的item，此函数支持redo/undo
 *
 * @return 返回删除的数量，如果没有删除，返回0
 *
 * @note 有些连带删除，例如选择了一个node，它有连线，删除节点会把连线也删除，这时候返回的删除结果会比较多
 */
int DANodeGraphicsScene::removeSelectedItems_()
{
    // 此操作比较复杂，首先要找到非node的item，这些直接就可以删除，然后找到nodeitem，先删除link，再删除node
    //  QGraphicsItem不进行处理,因此普通的item需要调用addItem_而不是addItem
    // 需要先找到节点下面所以得link，和选中的link进行分组
    //! 注意： 如果处于链接状态（isStartLink() == true）那么不能删除正在链接的那个元素的节点，否则会导致异常
    qDebug() << "removeSelectedItems_";
    // 移除元素过程中，先要删除链接
    cancelLink();
    QScopedPointer< DA::DACommandsForWorkFlowRemoveSelectNodes > cmd(new DA::DACommandsForWorkFlowRemoveSelectNodes(this));
    int rc = cmd->removeCount();
    if (!cmd->isValid()) {
        qDebug() << "remove select is invalid";
        return 0;
    }
    DA::DACommandsForWorkFlowRemoveSelectNodes* rawcmd = cmd.take();
    push(rawcmd);
    QList< DAAbstractNodeGraphicsItem* > rn = rawcmd->getRemovedNodeItems();
    if (rn.size() > 0) {
        emit nodeItemsRemoved(rn);
    }
    QList< DAAbstractNodeLinkGraphicsItem* > rl = rawcmd->getRemovedNodeLinkItems();
    if (rl.size() > 0) {
        emit nodeLinksRemoved(rl);
    }
    QList< QGraphicsItem* > gl = rawcmd->getRemovedItems();
    if (gl.size() > 0) {
        emit itemRemoved(gl);
    }
    return rc;
}

/**
 * @brief 创建节点（不带回退功能）
 * @note 注意，节点会记录在工作流中,如果返回的是nullptr，则不会记录
 * @param md
 * @param pos
 * @return
 */
DAAbstractNodeGraphicsItem* DANodeGraphicsScene::createNode(const DANodeMetaData& md, const QPointF& pos)
{
    if (nullptr == d_ptr->mWorkflow) {
        return nullptr;
    }
    DAAbstractNode::SharedPointer n = d_ptr->mWorkflow->createNode(md);

    DAAbstractNodeGraphicsItem* nodeitem = nullptr;
    if (nullptr == n) {
        qCritical() << QObject::tr("can not create node,metadata name=%1(%2)").arg(md.getNodeName(), md.getNodePrototype());
    } else {
        nodeitem = n->createGraphicsItem();
        if (nodeitem) {
            nodeitem->setPos(pos);
            d_ptr->mWorkflow->addNode(n);
        }
    }
    return nodeitem;
}

/**
 * @brief 通过node元对象创建工作流节点
 * @param md
 * @return 如果创建失败，返回nullptr
 */
DAAbstractNodeGraphicsItem* DANodeGraphicsScene::createNode_(const DANodeMetaData& md, const QPointF& pos)
{
    QScopedPointer< DA::DACommandsForWorkFlowCreateNode > cmd(new DA::DACommandsForWorkFlowCreateNode(md, this, pos));
    DAAbstractNodeGraphicsItem* item = cmd->item();
    if (item == nullptr) {
        return nullptr;
    }
    push(cmd.take());
    return item;
}

/**
 * @brief 创建并加入一个文本框
 * @param pos
 * @return
 * @sa getTextGraphicsItems
 */
DAGraphicsStandardTextItem* DANodeGraphicsScene::createText_(const QString& str)
{
    DAGraphicsStandardTextItem* item = new DAGraphicsStandardTextItem();
    if (!str.isEmpty()) {
        item->setPlainText(str);
    }
    addItem_(item);
    return (item);
}

/**
 * @brief 在画布中创建一个矩形
 * @param p 矩形的位置
 * @return
 */
DAGraphicsRectItem* DANodeGraphicsScene::createRect_(const QPointF& p)
{
    DAGraphicsRectItem* item = new DAGraphicsRectItem();
    addItem_(item);
    if (!p.isNull()) {
        item->setPos(p);
    }
    return (item);
}

/**
   @brief 通过位置获取DAAbstractNodeGraphicsItem

    此函数是加强版的itemAt
   @param scenePos
   @return
 */
DAAbstractNodeGraphicsItem* DANodeGraphicsScene::nodeItemAt(const QPointF& scenePos) const
{
    DAAbstractNodeGraphicsItem* nodeItem = dynamic_cast< DAAbstractNodeGraphicsItem* >(itemAt(scenePos, QTransform()));
    if (nodeItem) {
        return nodeItem;
    }
    // 如果没找到，就要看看此点下的所有item，有可能被分组了
    QList< QGraphicsItem* > its = topItems(scenePos);
    for (QGraphicsItem* i : std::as_const(its)) {
        if (DAAbstractNodeGraphicsItem* n = dynamic_cast< DAAbstractNodeGraphicsItem* >(i)) {
            return n;
        }
    }
    return nullptr;
}

void DANodeGraphicsScene::initConnect()
{
    qRegisterMetaType< DANodeLinkPoint >("DANodeLinkPoint");
    //    connect(this, &QGraphicsScene::selectionChanged, this, &DANodeGraphicsScene::onItemSelectionChanged);
    connect(this, &DAGraphicsScene::selectItemChanged, this, &DANodeGraphicsScene::onSelectItemChanged);
    connect(this, &DAGraphicsScene::selectLinkChanged, this, &DANodeGraphicsScene::onSelectLinkChanged);
}

/**
 * @brief itemlink都没用节点连接时会调用这个函数，发出
 * @param link
 */
void DANodeGraphicsScene::callNodeItemLinkIsEmpty(DAAbstractNodeLinkGraphicsItem* link)
{
    removeItem(link);
    emit nodeLinkItemIsEmpty(link);
}

/**
 * @brief item选择改变
 *
 * @note 注意，多选只会传递最后选中的那个，避免批量选择时的卡顿
 */
// void DANodeGraphicsScene::onItemSelectionChanged()
//{
//     QList< QGraphicsItem* > sits = selectedItems();
//     if (sits.isEmpty()) {
//         return;
//     }
//     QGraphicsItem* i = sits.last();
//     if (DAAbstractNodeGraphicsItem* gi = dynamic_cast< DAAbstractNodeGraphicsItem* >(i)) {
//         emit selectNodeItemChanged(gi);
//     } else if (DAAbstractNodeLinkGraphicsItem* gi = dynamic_cast< DAAbstractNodeLinkGraphicsItem* >(i)) {
//         emit selectNodeLinkChanged(gi);
//     }
// }
void DANodeGraphicsScene::onSelectItemChanged(DAGraphicsItem* item)
{
    if (DAAbstractNodeGraphicsItem* gi = dynamic_cast< DAAbstractNodeGraphicsItem* >(item)) {
        emit selectNodeItemChanged(gi);
    }
}
void DANodeGraphicsScene::onSelectLinkChanged(DAGraphicsLinkItem* item)
{
    if (DAAbstractNodeLinkGraphicsItem* gi = dynamic_cast< DAAbstractNodeLinkGraphicsItem* >(item)) {
        emit selectNodeLinkChanged(gi);
    }
}
/**
 * @brief 节点名字改变触发的槽
 * @param node
 * @param oldName
 * @param newName
 */
void DANodeGraphicsScene::onNodeNameChanged(DAAbstractNode::SharedPointer node, const QString& oldName, const QString& newName)
{
    // 查找对应的item并让其改变文字
    Q_UNUSED(oldName);
    DAAbstractNodeGraphicsItem* it = findItemByNode(node.get());
    if (it) {
        it->prepareNodeNameChanged(newName);
    }
}

void DANodeGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    if (mouseEvent->isAccepted()) {
        // 说明上级已经接受了鼠标事件，这里不应该处理
        if (isStartLink()) {
            cancelLink();
        }
        DAGraphicsScene::mousePressEvent(mouseEvent);
        DANODEGRAPHICSSCENE_EVENTLISTENER(mousePressEvent, mouseEvent)
        return;
    }
    // 先检查是否点击到了连接点
    if (mouseEvent->buttons().testFlag(Qt::LeftButton)) {

        // 左键点击
        //         QGraphicsItem* positem = itemAt(mouseEvent->scenePos(), QTransform());
        //         if (nullptr == positem) {
        //             DAGraphicsSceneWithUndoStack::mousePressEvent(mouseEvent);
        //             return;
        //         }
        // 看看是否点击到了节点item
        DAAbstractNodeGraphicsItem* nodeItem = nodeItemAt(mouseEvent->scenePos());
        if (nullptr == nodeItem) {
            // 注意，有可能进行了分组，这时候，点击的是分组
            // 点击的不是节点item就退出
            DAGraphicsScene::mousePressEvent(mouseEvent);
            DANODEGRAPHICSSCENE_EVENTLISTENER(mousePressEvent, mouseEvent)
            return;
        }
        // 如果点击到了DAAbstractNodeGraphicsItem，要看看是否点击到了连接点
        QPointF itempos = nodeItem->mapFromScene(mouseEvent->scenePos());
        if (isStartLink()) {
            // 开始链接状态，此时理论要点击的是input
            DAAbstractNodeLinkGraphicsItem* linkItem = dynamic_cast< DAAbstractNodeLinkGraphicsItem* >(getCurrentLinkItem());
            if (linkItem) {
                // 调用nodeitem的tryLinkOnItemPos用于构建一些响应式连接点
                nodeItem->tryLinkOnItemPos(itempos, linkItem, DANodeLinkPoint::Input);
            } else {
                qDebug() << "is start link,but link item can not cast to DAAbstractNodeLinkGraphicsItem";
                DAGraphicsScene::mousePressEvent(mouseEvent);
                DANODEGRAPHICSSCENE_EVENTLISTENER(mousePressEvent, mouseEvent)
                return;
            }
            DANodeLinkPoint lp;
            lp = nodeItem->getLinkPointByPos(itempos, DANodeLinkPoint::Input);
            if (!lp.isValid() || lp.isOutput()) {  // 正常时要到input，链接到output点就忽略
                //! 说明没有点击到连接点，正常传递到DAGraphicsSceneWithUndoStack
                //! DAGraphicsSceneWithUndoStack的连线在这时候就结束，但这里并不想结束，
                //! 因此需要调用setIgnoreLinkEvent忽略掉链接模式的事件
                setIgnoreLinkEvent(true);
                DAGraphicsScene::mousePressEvent(mouseEvent);
                setIgnoreLinkEvent(false);
                DANODEGRAPHICSSCENE_EVENTLISTENER(mousePressEvent, mouseEvent)
                return;
            }
            // 点击了连接点
            emit nodeItemLinkPointSelected(nodeItem, lp, mouseEvent);

            // 此时连接到to点
            if (!(linkItem->attachTo(nodeItem, lp))) {
                // 连接失败
                setIgnoreLinkEvent(true);
                DAGraphicsScene::mousePressEvent(mouseEvent);
                setIgnoreLinkEvent(false);
                DANODEGRAPHICSSCENE_EVENTLISTENER(mousePressEvent, mouseEvent)
                return;
            }
            // 连接成功，把item脱离管理
            //! 记录到redo/undo中
            DACommandsForWorkFlowCreateLink* cmd = new DACommandsForWorkFlowCreateLink(linkItem, this);
            push(cmd);
            // 跳出if 到DAGraphicsSceneWithUndoStack::mousePressEvent(mouseEvent);
        } else {
            // 非开始链接状态，此时点击的是output
            nodeItem->tryLinkOnItemPos(itempos, nullptr, DANodeLinkPoint::Output);
            DANodeLinkPoint lp;
            lp = nodeItem->getLinkPointByPos(itempos, DANodeLinkPoint::Output);
            if (lp.isValid() && lp.isOutput()) {
                emit nodeItemLinkPointSelected(nodeItem, lp, mouseEvent);
                // 此时说明开始进行连线
                std::unique_ptr< DAAbstractNodeLinkGraphicsItem > linkItem(nodeItem->createLinkItem(lp));
                if (linkItem) {
                    if (linkItem->attachFrom(nodeItem, lp)) {
                        linkItem->setStartScenePosition(nodeItem->mapToScene(lp.position));
                        beginLink(linkItem.release());
                        // 这里不需要return，因为beginLink后如果不移动，DAGraphicsSceneWithUndoStack::mousePressEvent不会触发结束
                    }
                }
            }
        }
    }
    DAGraphicsScene::mousePressEvent(mouseEvent);
    DANODEGRAPHICSSCENE_EVENTLISTENER(mousePressEvent, mouseEvent)
}

void DANodeGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    DAGraphicsScene::mouseMoveEvent(mouseEvent);
    DANODEGRAPHICSSCENE_EVENTLISTENER(mouseMoveEvent, mouseEvent)
}

void DANodeGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    DAGraphicsScene::mouseReleaseEvent(mouseEvent);
    DANODEGRAPHICSSCENE_EVENTLISTENER(mouseReleaseEvent, mouseEvent)
}
}  // end DA
