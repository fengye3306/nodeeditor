#pragma once

#include <QtCore/QUuid>
#include <QtWidgets/QGraphicsObject>

#include "NodeState.hpp"

class QGraphicsProxyWidget;

namespace QtNodes {

class BasicGraphicsScene;
class AbstractGraphModel;


/**
 * @brief 节点模块 本体，负责管理节点在场景中的图形表示和交互。
 */
class NodeGraphicsObject : public QGraphicsObject
{
    Q_OBJECT
public:
    // 用于 qgraphicsitem_cast
    enum { Type = UserType + 1 };

    /**
     * @brief 获取节点图形对象的类型，用于类型识别。
     * @return 类型ID。
     */
    int type() const override { return Type; }

public:
    /**
     * @brief 构造函数，初始化节点图形对象。
     * @param scene 图形场景对象。
     * @param node 节点的唯一标识符。
     */
    NodeGraphicsObject(BasicGraphicsScene &scene, NodeId node);

    /**
     * @brief 析构函数，清理节点图形对象。
     */
    ~NodeGraphicsObject() override = default;

public:
    /**
     * @brief 获取节点所属的图形模型。
     * @return 引用图形模型对象。
     */
    AbstractGraphModel &graphModel() const;

    /**
     * @brief 获取节点所在的图形场景。
     * @return 指向节点场景的指针。
     */
    BasicGraphicsScene *nodeScene() const;

    /**
     * @brief 获取节点的唯一标识符（可修改版本）。
     * @return 节点ID。
     */
    NodeId nodeId() { return _nodeId; }

    /**
     * @brief 获取节点的唯一标识符（只读版本）。
     * @return 节点ID。
     */
    NodeId nodeId() const { return _nodeId; }

    /**
     * @brief 获取节点的状态信息（可修改版本）。
     * @return 节点状态对象的引用。
     */
    NodeState &nodeState() { return _nodeState; }

    /**
     * @brief 获取节点的状态信息（只读版本）。
     * @return 节点状态对象的引用。
     */
    NodeState const &nodeState() const { return _nodeState; }

    /**
     * @brief 获取节点的边界矩形，用于确定绘制区域。
     * @return 边界矩形。
     */
    QRectF boundingRect() const override;

    /**
     * @brief 设置节点的几何状态为已更改。
     */
    void setGeometryChanged();

    /**
     * @brief 更新所有连接的端点，使连接线正确定位。
     */
    void moveConnections() const;

    /**
     * @brief 当节点与连接对象交互时，重绘节点，显示响应端口。
     * @param cgo 连接图形对象的指针。
     */
    void reactToConnection(ConnectionGraphicsObject const *cgo);

protected:
    /**
     * @brief 绘制节点的图形表示。
     * @param painter 用于绘制的 QPainter 对象。
     * @param option 绘图选项。
     * @param widget 绘图小部件。
     */
    void paint(QPainter *painter,
               QStyleOptionGraphicsItem const *option,
               QWidget *widget = 0) override;

    /**
     * @brief 处理节点属性更改的事件。
     * @param change 更改的类型。
     * @param value 更改的值。
     * @return 更改后的值。
     */
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    /**
     * @brief 处理鼠标按下事件。
     * @param event 鼠标事件信息。
     */
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    /**
     * @brief 处理鼠标移动事件。
     * @param event 鼠标事件信息。
     */
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    /**
     * @brief 处理鼠标释放事件。
     * @param event 鼠标事件信息。
     */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    /**
     * @brief 处理鼠标悬停进入事件。
     * @param event 鼠标事件信息。
     */
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;

    /**
     * @brief 处理鼠标悬停离开事件。
     * @param event 鼠标事件信息。
     */
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    /**
     * @brief 处理鼠标悬停移动事件。
     */
    void hoverMoveEvent(QGraphicsSceneHoverEvent *) override;

    /**
     * @brief 处理鼠标双击事件。
     * @param event 鼠标事件信息。
     */
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    /**
     * @brief 处理右键点击上下文菜单事件。
     * @param event 鼠标事件信息。
     */
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

private:
    /**
     * @brief 嵌入QWidget至节点图形对象中。
     */
    void embedQWidget();

    /**
     * @brief 设置节点图形对象为锁定状态。
     */
    void setLockedState();

private:
    NodeId _nodeId;                         ///< 节点的唯一标识符
    AbstractGraphModel &_graphModel;        ///< 引用图形模型对象
    NodeState _nodeState;                   ///< 节点的状态信息
    QGraphicsProxyWidget *_proxyWidget;     ///< 嵌入的QWidget的代理，可能为空或由父对象管理
};

} // namespace QtNodes
