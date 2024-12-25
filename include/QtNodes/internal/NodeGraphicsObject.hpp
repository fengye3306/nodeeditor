#pragma once

#include <QtCore/QUuid>
#include <QtWidgets/QGraphicsObject>

#include "NodeState.hpp"

class QGraphicsProxyWidget;

namespace QtNodes {

class BasicGraphicsScene;
class AbstractGraphModel;

/** 
 * 代表场景中的一个图形对象，是底层节点模型的视图层封装
 * 这个类管理一个节点在场景中的视觉表示，处理事件（如鼠标和悬停事件）并与底层图形模型进行交互。
 */
class NodeGraphicsObject : public QGraphicsObject
{
    Q_OBJECT
public:

    enum { Type = UserType + 1 };

    int type() const override { return Type; }

public:
    /** 
     * @brief 构造函数。
     * @param scene 场景的指针。
     * @param node  节点的ID。
     */
    NodeGraphicsObject(BasicGraphicsScene &scene, NodeId node);

    ~NodeGraphicsObject() override = default;

public:
    // 所在的节点管理器
    AbstractGraphModel &graphModel() const;
    // 所在场景
    BasicGraphicsScene *nodeScene() const;
    // 节点ID
    NodeId nodeId() { return _nodeId; }
    // 节点ID
    NodeId nodeId() const { return _nodeId; }

    // 节点状态引用
    NodeState &nodeState() { return _nodeState; }
    NodeState const &nodeState() const { return _nodeState; }

    // 边界矩形
    QRectF boundingRect() const override;
    // 节点几何变化标志
    void setGeometryChanged();
    // 访问所有附加的连接
    void moveConnections() const;

    void reactToConnection(ConnectionGraphicsObject const *cgo);
protected:
    /**
     * @brief 绘制节点的方法。
     * @param painter QPainter 对象，用于绘制。
     * @param option 绘制选项。
     * @param widget 可选的部件，通常为空。
     */
    void paint(QPainter *painter, QStyleOptionGraphicsItem const *option, QWidget *widget = 0) override;

    /**
     * @brief        处理项改变事件。
     * @param change 改变的类型。
     * @param value  改变的值。
     * @return       处理后的值。
     */
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    /**
     * @brief 处理鼠标按下事件。
     * @param event 鼠标事件对象。 
     */
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    /**
     * @brief 处理鼠标移动事件。
     * @param event 鼠标事件对象。
     */
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    /**
     * @brief 处理鼠标释放事件。
     * @param event 鼠标事件对象。
     */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    /**
     * @brief 处理鼠标进入悬停事件。
     * @param event 悬停事件对象。
     */
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;

    /**
     * @brief 处理鼠标离开悬停事件。
     * @param event 悬停事件对象。
     */
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    /**
     * @brief 处理鼠标移动悬停事件。
     * @param event 悬停事件对象。
     */
    void hoverMoveEvent(QGraphicsSceneHoverEvent *) override;

    /**
     * @brief 处理鼠标双击事件。
     * @param event 鼠标事件对象。 */
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    /**
     * @brief 处理上下文菜单事件。
     * @param event 菜单事件对象。
     */
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

private:
    /** @brief 嵌入 QWidget。
     *  将 QWidget 嵌入到图形项中，通常用于添加交互界面。
     */
    void embedQWidget();

    /** @brief 设置锁定状态。
     *  锁定或解锁节点，以防止或允许用户交互。
     */
    void setLockedState();

private:
    NodeId _nodeId;                   ///< 节点的唯一标识符。
    AbstractGraphModel &_graphModel;  ///< 引用图形模型。
    NodeState _nodeState;             ///< 节点的状态。
    
    /// 要么是 nullptr，要么由父类 QGraphicsItem 所拥有
    QGraphicsProxyWidget *_proxyWidget; 
};
} // namespace QtNodes
