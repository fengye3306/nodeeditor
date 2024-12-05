#pragma once

#include <QtCore/QUuid>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QMenu>

#include <functional>
#include <memory>
#include <tuple>
#include <unordered_map>

#include "AbstractGraphModel.hpp"
#include "AbstractNodeGeometry.hpp"
#include "ConnectionIdHash.hpp"
#include "Definitions.hpp"
#include "Export.hpp"

#include "QUuidStdHash.hpp"

class QUndoStack;

namespace QtNodes {

class AbstractGraphModel;
class AbstractNodePainter;
class ConnectionGraphicsObject;
class NodeGraphicsObject;
class NodeStyle;

/// QGraphicsScene 的一个实例，持有连接和节点的图形表示。
class NODE_EDITOR_PUBLIC BasicGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    /// 构造函数，传入一个图形模型
    BasicGraphicsScene(AbstractGraphModel &graphModel, QObject *parent = nullptr);

    // 不支持没有模型的场景
    BasicGraphicsScene() = delete;

    ~BasicGraphicsScene();

public:
    /// 返回与场景关联的 AbstractGraphModel。
    AbstractGraphModel const &graphModel() const;

    /// 返回可修改的 AbstractGraphModel 引用。
    AbstractGraphModel &graphModel();

    /// 返回与场景关联的 AbstractNodeGeometry。
    AbstractNodeGeometry &nodeGeometry();

    /// 返回与场景关联的 AbstractNodePainter。
    AbstractNodePainter &nodePainter();

    /// 设置 NodePainter。
    void setNodePainter(std::unique_ptr<AbstractNodePainter> newPainter);

    /// 返回场景的撤销堆栈。
    QUndoStack &undoStack();

public:
    /// 创建一个“草稿”状态的 ConnectionGraphicsObject。
    /**
     * 场景缓存一个“草稿”连接，该连接只有一个松散的端点。
     * 在连接完成后，“草稿”连接会被删除，并创建一个正常的“完整”连接。
     * 返回“草稿”连接，以便进行进一步的几何操作。
     */
    std::unique_ptr<ConnectionGraphicsObject> const &makeDraftConnection(
        ConnectionId const newConnectionId);

    /// 删除“草稿”连接。
    /**
     * 当用户在构建新连接时没有将其连接到任何节点，
     * 而释放了鼠标按钮时，会调用此函数。
    */
    void resetDraftConnection();

    /// 删除所有节点，连接会自动删除。
    void clearScene();

public:
    /// 根据节点ID返回对应的 NodeGraphicsObject。
    /**
     * 如果未找到，返回 nullptr
     */
    NodeGraphicsObject *nodeGraphicsObject(NodeId nodeId);

    /// 根据连接ID返回对应的 ConnectionGraphicsObject。
    /**
     * 如果未找到，返回 nullptr。
     */
    ConnectionGraphicsObject *connectionGraphicsObject(ConnectionId connectionId);

    /// 返回场景的方向。
    Qt::Orientation orientation() const { return _orientation; }

    /// 设置场景的方向。
    void setOrientation(Qt::Orientation const orientation);

public:
    /// 可在子类中实现，返回场景的上下文菜单。
    /**
     * 默认实现返回 nullptr。
     */
    virtual QMenu *createSceneMenu(QPointF const scenePos);

Q_SIGNALS:
    /// 场景被修改时触发信号。
    void modified(BasicGraphicsScene *);

    /// 节点位置发生变化时触发信号。
    void nodeMoved(NodeId const nodeId, QPointF const &newLocation);

    /// 节点被点击时触发信号。
    void nodeClicked(NodeId const nodeId);

    /// 节点被选中时触发信号。
    void nodeSelected(NodeId const nodeId);

    /// 节点被双击时触发信号。
    void nodeDoubleClicked(NodeId const nodeId);

    /// 节点被悬停时触发信号。
    void nodeHovered(NodeId const nodeId, QPoint const screenPos);

    /// 鼠标离开节点时触发信号。
    void nodeHoverLeft(NodeId const nodeId);

    /// 连接被悬停时触发信号。
    void connectionHovered(ConnectionId const connectionId, QPoint const screenPos);

    /// 鼠标离开连接时触发信号。
    void connectionHoverLeft(ConnectionId const connectionId);

    /// 当用户右键点击节点时，触发上下文菜单信号。
    void nodeContextMenu(NodeId const nodeId, QPointF const pos);

private:
    /// 创建节点和连接的图形对象。
    /**
     * 该函数用于在构造函数中填充一个空的场景。
     * 我们通过深度优先遍历 AbstractGraphModel，检查节点的“Out”端口来创建连接。
     */
    void traverseGraphAndPopulateGraphicsObjects();

    /// 更新与指定连接ID关联的节点图形。
    void updateAttachedNodes(ConnectionId const connectionId, PortType const portType);

public Q_SLOTS:
    /// 当连接ID从 AbstractGraphModel 中删除时，调用此槽函数。
    void onConnectionDeleted(ConnectionId const connectionId);

    /// 当连接ID在 AbstractGraphModel 中创建时，调用此槽函数。
    void onConnectionCreated(ConnectionId const connectionId);

    /// 当节点ID被删除时，调用此槽函数。
    void onNodeDeleted(NodeId const nodeId);

    /// 当节点ID被创建时，调用此槽函数。
    void onNodeCreated(NodeId const nodeId);

    /// 当节点位置被更新时，调用此槽函数。
    void onNodePositionUpdated(NodeId const nodeId);

    /// 当节点被更新时，调用此槽函数。
    void onNodeUpdated(NodeId const nodeId);

    /// 当节点被点击时，调用此槽函数。
    void onNodeClicked(NodeId const nodeId);

    /// 当模型被重置时，调用此槽函数。
    void onModelReset();

private:
    // 引用关联的 AbstractGraphModel
    AbstractGraphModel &_graphModel;

    // 使用智能指针管理 NodeGraphicsObject
    using UniqueNodeGraphicsObject = std::unique_ptr<NodeGraphicsObject>;

    // 使用智能指针管理 ConnectionGraphicsObject
    using UniqueConnectionGraphicsObject = std::unique_ptr<ConnectionGraphicsObject>;

    // 存储 NodeGraphicsObject 的映射表
    std::unordered_map<NodeId, UniqueNodeGraphicsObject> _nodeGraphicsObjects;

    // 存储 ConnectionGraphicsObject 的映射表
    std::unordered_map<ConnectionId, UniqueConnectionGraphicsObject> _connectionGraphicsObjects;

    // 当前草稿连接
    std::unique_ptr<ConnectionGraphicsObject> _draftConnection;

    // 与场景关联的节点几何数据
    std::unique_ptr<AbstractNodeGeometry> _nodeGeometry;

    // 与场景关联的节点绘制器
    std::unique_ptr<AbstractNodePainter> _nodePainter;

    // 是否正在拖动节点
    bool _nodeDrag;

    // 撤销堆栈
    QUndoStack *_undoStack;

    // 场景的方向
    Qt::Orientation _orientation;
};

} // namespace QtNodes
