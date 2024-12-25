#pragma once

#include "Export.hpp"

#include <unordered_map>
#include <unordered_set>

#include <QtCore/QJsonObject>
#include <QtCore/QObject>
#include <QtCore/QVariant>

#include "ConnectionIdHash.hpp"
#include "Definitions.hpp"

namespace QtNodes {

/**
 * 模型-视图方法中的核心类。它从代表图的用户数据结构中提供各种信息。
 * 该类允许修改图结构：创建和移除节点和连接。
 *
 * 我们使用两种类型的唯一标识符进行图操作：
 *   - NodeId
 *   - ConnectionId
 */
class NODE_EDITOR_PUBLIC AbstractGraphModel : public QObject
{
    Q_OBJECT
public:
    /// 生成一个新的唯一 NodeId。
    virtual NodeId newNodeId() = 0;

    /// 返回所有唯一 Node Ids 的完整集合。
    virtual std::unordered_set<NodeId> allNodeIds() const = 0;

    /// 返回给定 nodeId 的所有输入和输出连接的集合。
    virtual std::unordered_set<ConnectionId> allConnectionIds(NodeId const nodeId) const = 0;

    /// 返回给定端口的所有已连接 Node Ids。
    virtual std::unordered_set<ConnectionId> connections(NodeId nodeId, PortType portType, PortIndex index) const = 0;

    /// 检查两个给定 connectionId 的节点是否已连接。
    virtual bool connectionExists(ConnectionId const connectionId) const = 0;

    /// 在派生类中创建一个新的节点实例。
    virtual NodeId addNode(QString const nodeType = QString()) = 0;

    /// 模型决定给定连接Id的连接是否可能。
    virtual bool connectionPossible(ConnectionId const connectionId) const = 0;

    /// 定义是否可以分离连接。
    virtual bool detachPossible(ConnectionId const) const { return true; }

    /// 在两个节点之间创建一个新的连接。
    virtual void addConnection(ConnectionId const connectionId) = 0;

    /// 如果模型中存在与给定 nodeId 关联的数据，则返回 `true`。
    virtual bool nodeExists(NodeId const nodeId) const = 0;

    /// 返回请求 NodeRole 的节点相关数据。
    virtual QVariant nodeData(NodeId nodeId, NodeRole role) const = 0;

    /// 一个实用函数，它解包从标准 `QVariant AbstractGraphModel::nodeData(NodeId, NodeRole)` 函数返回的 `QVariant` 值。
    template<typename T>
    T nodeData(NodeId nodeId, NodeRole role) const
    {
        return nodeData(nodeId, role).value<T>();
    }

    virtual NodeFlags nodeFlags(NodeId nodeId) const
    {
        Q_UNUSED(nodeId);
        return NodeFlag::NoFlags;
    }

    /// 设置节点属性。
    virtual bool setNodeData(NodeId nodeId, NodeRole role, QVariant value) = 0;

    /// 返回请求 NodeRole 的端口相关数据。
    virtual QVariant portData(NodeId nodeId, PortType portType, PortIndex index, PortRole role) const = 0;

    /// 一个实用函数，它解包从标准 `QVariant AbstractGraphModel::portData(...)` 函数返回的 `QVariant` 值。
    template<typename T>
    T portData(NodeId nodeId, PortType portType, PortIndex index, PortRole role) const
    {
        return portData(nodeId, portType, index, role).value<T>();
    }

    virtual bool setPortData(NodeId nodeId,
                             PortType portType,
                             PortIndex index,
                             QVariant const &value,
                             PortRole role = PortRole::Data) = 0;

    virtual bool deleteConnection(ConnectionId const connectionId) = 0;

    virtual bool deleteNode(NodeId const nodeId) = 0;

    /**
   * 如果你想在撤销/重做节点删除操作期间保存/恢复节点的内部状态，请重写此函数。
   */
    virtual QJsonObject saveNode(NodeId const) const { return {}; }

    virtual void loadNode(QJsonObject const &) {}

public:
    /// 删除端口前清除连接到即将删除的端口的连接。在模型删除其旧的端口数据之前，必须调用此函数。
    void portsAboutToBeDeleted(NodeId const nodeId,
                               PortType const portType,
                               PortIndex const first,
                               PortIndex const last);

    /// 当模型不再有与给定端口索引关联的旧数据，并且节点必须重新绘制时，发出此信号。
    void portsDeleted();

    // 此方法主要处理节点上端口数量的动态增加，确保因端口索引变化而受影响的连接能够被适当地更新。
    void portsAboutToBeInserted(NodeId const nodeId,
                                PortType const portType,
                                PortIndex const first,
                                PortIndex const last);

    /// 重新创建在端口插入期间移动的连接。之后更新节点。
    void portsInserted();

Q_SIGNALS:

    // 当新的连接被创建时发出此信号。
    void connectionCreated(ConnectionId const connectionId);

    // 当一个连接被删除时发出此信号。
    void connectionDeleted(ConnectionId const connectionId);

    // 当一个新节点被创建时发出此信号。
    void nodeCreated(NodeId const nodeId);

    // 当一个节点被删除时发出此信号。
    void nodeDeleted(NodeId const nodeId);

    // 当一个节点的数据更新时发出此信号。
    void nodeUpdated(NodeId const nodeId);

    // 当一个节点的标志状态更新时发出此信号。
    void nodeFlagsUpdated(NodeId const nodeId);

    // 当一个节点的位置更新时发出此信号。
    void nodePositionUpdated(NodeId const nodeId);

    // 当模型重置时发出此信号。
    void modelReset();

private:
    std::vector<ConnectionId> _shiftedByDynamicPortsConnections;
};

} // namespace QtNodes
