#ifndef DAGRAPHICSTEXTITEM_H
#define DAGRAPHICSTEXTITEM_H

#include <QGraphicsTextItem>
#include "DAGraphicsResizeableItem.h"
namespace DA
{
class DAGraphicsStandardTextItem;
/**
 * @brief 支持缩放编辑的文本框Item
 */
class DAGRAPHICSVIEW_API DAGraphicsTextItem : public DAGraphicsResizeableItem
{
public:
    /**
     * @brief 适用qgraphicsitem_cast
     */
    enum
    {
        Type = DA::ItemType_DAGraphicsTextItem
    };
    int type() const override
    {
        return (Type);
    }

public:
    DAGraphicsTextItem(QGraphicsItem* parent = nullptr);
    DAGraphicsTextItem(const QFont& f, QGraphicsItem* parent = nullptr);
    DAGraphicsTextItem(const QString& str, const QFont& f, QGraphicsItem* parent = nullptr);
    // 设置编辑模式
    void setEditMode(bool on = true);
    // 保存到xml中
    virtual bool saveToXml(QDomDocument* doc, QDomElement* parentElement) const override;
    virtual bool loadFromXml(const QDomElement* itemElement) override;

    // 获取内部的文本item
    DAGraphicsStandardTextItem* textItem() const;

protected:
    void setBodySize(const QSizeF& s) override;
    void paintBody(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget, const QRectF& bodyRect) override;

private:
    DAGraphicsStandardTextItem* m_textItem;
};
}
#endif  // DAGRAPHICSTEXTITEM_H
