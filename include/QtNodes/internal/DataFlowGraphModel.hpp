#pragma once

#include "AbstractGraphModel.hpp"
#include "ConnectionIdUtils.hpp"
#include "NodeDelegateModelRegistry.hpp"
#include "Serializable.hpp"
#include "StyleCollection.hpp"

#include "Export.hpp"

#include <QJsonObject>

#include <memory>


namespace QtNodes {

/**
 * 数据流图模型，扩展自抽象图模型，并实现了可序列化接口。
 */
class NODE_EDITOR_PUBLIC DataFlowGraphModel : public AbstractGraphModel, public Serializable
{
    Q_OBJECT

public:
    // 节点几何数据结构，包括尺寸和位置。
    struct NodeGeometryData
    {
        QSize size;
        QPointF pos;
    };

public:
    // 构造函数，接受一个节点代理模型注册表的共享指针。
    DataFlowGraphModel(std::shared_ptr<NodeDelegateModelRegistry> registry);

    // 获取数据模型注册表。
    std::shared_ptr<NodeDelegateModelRegistry> dataModelRegistry() { return _registry; }
public:
    // 覆盖的方法，返回所有节点的ID。
    std::unordered_set<NodeId> allNodeIds() const override;

    // 返回指定节点的所有连接ID。
    std::unordered_set<ConnectionId> allConnectionIds(NodeId const nodeId) const override;

    // 返回指定节点的所有连接。
    std::unordered_set<ConnectionId> connections(NodeId nodeId,
                                                 PortType portType,
                                                 PortIndex portIndex) const override;
                                                 
    // 检查指定的连接是否存在。
    bool connectionExists(ConnectionId const connectionId) const override;

    // 添加一个新节点。
    NodeId addNode(QString const nodeType) override;

    // 检查是否可以创建连接。
    bool connectionPossible(ConnectionId const connectionId) const override;

    // 添加一个新的链接
    void addConnection(ConnectionId const connectionId) override;

    // 检查节点是否存在。
    bool nodeExists(NodeId const nodeId) const override;

    // 获取节点相关数据。
    QVariant nodeData(NodeId nodeId, NodeRole role) const override;

    // 获取节点标志。
    NodeFlags nodeFlags(NodeId nodeId) const override;

    // 设置节点数据。
    bool setNodeData(NodeId nodeId, NodeRole role, QVariant value) override;

    // 获取端口相关数据。
    QVariant portData(NodeId nodeId,
                      PortType portType,
                      PortIndex portIndex,
                      PortRole role) const override;

    // 设置端口数据。
    bool setPortData(NodeId nodeId,
                     PortType portType,
                     PortIndex portIndex,
                     QVariant const &value,
                     PortRole role = PortRole::Data) override;

    // 删除连接。
    bool deleteConnection(ConnectionId const connectionId) override;

    // 删除节点。
    bool deleteNode(NodeId const nodeId) override;

    // 保存节点的状态。
    QJsonObject saveNode(NodeId const) const override;

    // 保存整个模型的状态。
    QJsonObject save() const override;

    // 从JSON加载一个节点。
    void loadNode(QJsonObject const &nodeJson) override;

    // 从JSON加载整个模型。
    void load(QJsonObject const &json) override;

    /**
     * 获取给定 nodeId 的 NodeDelegateModel，并尝试将存储的指针转换为给定类型。
     */
    template<typename NodeDelegateModelType>
    NodeDelegateModelType *delegateModel(NodeId const nodeId)
    {
        auto it = _models.find(nodeId);
        if (it == _models.end())
            return nullptr;

        auto model = dynamic_cast<NodeDelegateModelType *>(it->second.get());

        return model;
    }

Q_SIGNALS:
    // 当输入端口设置了数据时发出的信号。
    void inPortDataWasSet(NodeId const, PortType const, PortIndex const);

private:
    // 生成一个新的 NodeId。
    NodeId newNodeId() override { return _nextNodeId++; }

    // 发送连接创建信号。
    void sendConnectionCreation(ConnectionId const connectionId);

    // 发送连接删除信号。
    void sendConnectionDeletion(ConnectionId const connectionId);

private Q_SLOTS:
    /**
   * 此函数在三种情况下被调用：
   *
   * - 当底层 NodeDelegateModel 有新数据传播时。
   *   @see DataFlowGraphModel::addNode
   * - 当创建了新连接时。
   *   @see DataFlowGraphModel::addConnection
   * - 当从JSON恢复节点并需要向下游发送数据时。
   *   @see DataFlowGraphModel::loadNode
   */
    void onOutPortDataUpdated(NodeId const nodeId, PortIndex const portIndex);

    // 在分离连接后调用，将空数据传播到指定节点。
    void propagateEmptyDataTo(NodeId const nodeId, PortIndex const portIndex);

private:
    std::shared_ptr<NodeDelegateModelRegistry> _registry;

    NodeId _nextNodeId;

    std::unordered_map<NodeId, std::unique_ptr<NodeDelegateModel>> _models;

    std::unordered_set<ConnectionId> _connectivity;

    mutable std::unordered_map<NodeId, NodeGeometryData> _nodeGeometryData;
};

} // namespace QtNodes
