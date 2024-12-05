#pragma once

#include <QtCore/QUuid>
#include <QtWidgets/QGraphicsObject>

#include "NodeState.hpp"

class QGraphicsProxyWidget;

namespace QtNodes {

class BasicGraphicsScene;
class AbstractGraphModel;

/** 
 * @class NodeGraphicsObject 
 * @brief 代表场景中的一个图形对象，与图形模型中的节点相关联。
 * 这个类管理一个节点在场景中的视觉表示，处理事件（如鼠标和悬停事件），
 * 并与底层图形模型进行交互。
 */
class NodeGraphicsObject : public QGraphicsObject
{
    Q_OBJECT
public:
    /* @brief 需要用于 qgraphicsitem_cast
     * 这用于识别自定义 QGraphicsObject 的类型，以便进行正确的类型转换。
     */
    enum { Type = UserType + 1 };

    /* @brief 返回项目的类型。
     * @return 项目的类型，用于类型转换。
     */
    int type() const override { return Type; }

public:
    /**
     * @brief 构造一个 NodeGraphicsObject 对象。
     * @param scene 此对象所属的图形场景。
     * @param node 此对象代表的节点的ID。
    */
    NodeGraphicsObject(BasicGraphicsScene &scene, NodeId node);

    /** 
     * @brief 析构函数，默认实现。
    */
    ~NodeGraphicsObject() override = default;

public:
    /**
     * @brief  获取图形模型的引用。
     * @return 返回关联的图形模型。
     */
    AbstractGraphModel &graphModel() const;

    /**
     * @brief  获取节点所在的场景。
     * @return 返回节点所在的基本图形场景的指针。
     */
    BasicGraphicsScene *nodeScene() const;

    /**
     * @brief  获取节点ID。
     * @return 返回此图形对象表示的节点的ID。
     */
    NodeId nodeId() { return _nodeId; }

    /**
     * @brief 获取节点ID的常量版本。
     * @return 返回此图形对象表示的节点的ID。
     */
    NodeId nodeId() const { return _nodeId; }

    /**
     * @brief  获取节点状态的引用。
     * @return 返回此节点的状态的引用。
     */
    NodeState &nodeState() { return _nodeState; }

    /**
     * @brief  获取节点状态的常量引用。
     * @return 返回此节点的状态的常量引用。
     */
    NodeState const &nodeState() const { return _nodeState; }

    /**
     * @brief 定义节点的边界矩形。
     * @return 返回用于绘制和碰撞检测的节点的边界矩形。
     */
    QRectF boundingRect() const override;

    /**
     * @brief 设置几何变化标志。
     * 标记节点的几何形状已发生变化，需要更新。
     */
    void setGeometryChanged();

    /**
     * @brief 访问所有附加的连接，并修正它们的对应端点。
     */
    void moveConnections() const;

    /**
     * @brief 根据响应端口重绘一次节点。
     * @param cgo 连接图形对象的常量指针。
     */
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
