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

/** @brief 节点管理器 */
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
    
    /**
     * @brief 构造函数
     * 
     * @param registry 节点构造器注册表
     */
    DataFlowGraphModel(std::shared_ptr<NodeDelegateModelRegistry> registry);
    
    /**
     * @brief 获取节点构造器注册表
     * 
     * @return std::shared_ptr<NodeDelegateModelRegistry> 
     */
    std::shared_ptr<NodeDelegateModelRegistry> dataModelRegistry() { return _registry; }
public:
    
    /**
     * @brief 获取节点集合，存放于 std::unordered_set<NodeId> 中
     * 
     * @return std::unordered_set<NodeId> 节点集合
     */
    std::unordered_set<NodeId> allNodeIds() const override;

    /**
     * @brief 某ID节点，返回它所有的连接
     * 
     * @param nodeId 节点ID 
     * @return std::unordered_set<ConnectionId> 
     */
    std::unordered_set<ConnectionId> allConnectionIds (NodeId const nodeId) const override;

    /**
     * @brief 某ID节点，返回它某端口下所有的连接
     * 
     * @param nodeId    节点ID
     * @param portType  端口类型
     * @param portIndex 端口索引
     * @return std::unordered_set<ConnectionId> 
     */
    std::unordered_set<ConnectionId> connections (NodeId nodeId, PortType portType, PortIndex portIndex) const override;
                                                 
    /**
     * @brief 某ID链接是否存在？
     * 
     * @param connectionId 链接ID
     * @return true  存在
     * @return false 不存在
     */
    bool connectionExists(ConnectionId const connectionId) const override;

    /**
     * @brief 添加新节点
     *  例如 “乘法”，在注册表中查找到对应的构造器“乘法”，然后创建一个新的“乘法”节点
     * @param nodeType 新节点 节点名 例如“乘法”
     * @return NodeId  新建的节点ID
     */
    NodeId addNode(QString const nodeType) override;

    /**
     * @brief 连接是否可能？
     * 
     * @param connectionId 链接ID
     * @return true  可能
     * @return false 不可能
     */
    bool connectionPossible(ConnectionId const connectionId) const override;

    /** 添加一个链接 */
    void addConnection(ConnectionId const connectionId) override;

    /** 某ID节点是否存在 */
    bool nodeExists(NodeId const nodeId) const override;

    /**
     * @brief 获取节点标志
     * 
     * @param nodeId 节点ID
     * @return NodeFlags 用于规定节点特性和外观的特定标志。
     */
    NodeFlags nodeFlags(NodeId nodeId) const override;

    /** 从节点中抽取数据 */
    QVariant nodeData(NodeId nodeId, NodeRole role) const override;
    /** 为节点配置数据 */
    bool setNodeData(NodeId nodeId, NodeRole role, QVariant value) override;

    /** 获取节点 某端口 的数据 */
    QVariant portData(NodeId nodeId,
                      PortType portType,
                      PortIndex portIndex,
                      PortRole role) const override;

    /** 设置节点 某端口 数据 */
    bool setPortData(NodeId nodeId,
                     PortType portType,
                     PortIndex portIndex,
                     QVariant const &value,
                     PortRole role = PortRole::Data) override;

    /** 删除链接 */
    bool deleteConnection(ConnectionId const connectionId) override;
    /** 删除节点 */
    bool deleteNode(NodeId const nodeId) override;

    // 单个节点序列化
    QJsonObject saveNode(NodeId const) const override;
    // 单个节点的反序列化
    void loadNode(QJsonObject const &nodeJson) override;
    // save
    QJsonObject save() const override;
    // load
    void load(QJsonObject const &json) override;

    /**
      * 对于某节点， 构造其同类节点
    */
    template<typename NodeDelegateModelType>
    NodeDelegateModelType *delegateModel(NodeId const nodeId)
    {
        auto it = _models.find(nodeId);
        if (it == _models.end())
        {
            return nullptr;
        }
        auto model = dynamic_cast<NodeDelegateModelType *>(it->second.get());
        return model;
    }

    /** 添加节点 */
    void addPort(NodeId nodeId, PortType portType, PortIndex portIndex);
    /** 移除节点 */
    void removePort(NodeId nodeId, PortType portType, PortIndex first);


Q_SIGNALS:
    // 节点数据已经更新， DataFlowGraphModel::setPortData 调用
    void inPortDataWasSet(NodeId const, PortType const, PortIndex const);
private:
    // 生成一个新节点时，节点ID
    NodeId newNodeId() override { return _nextNodeId++; }
    
    /**
     * @brief 链接产生时 触发
     * AbstractGraphModel::connectionCreated
     * 调用对应被链接节点的
     * NodeDelegateModel::inputConnectionCreated
     * 调用对应输出节点的
     * NodeDelegateModel::outputConnectionCreated
     * @param connectionId 
     */
    void sendConnectionCreation(ConnectionId const connectionId);

    /**
     * @brief 链接产生时 触发
     * AbstractGraphModel::connectionDeleted
     * 调用对应被链接节点的
     * NodeDelegateModel::inputConnectionDeleted
     * 调用对应输出节点的
     * NodeDelegateModel::outputConnectionDeleted
     * @param connectionId 
     */
    void sendConnectionDeletion(ConnectionId const connectionId);
    
private Q_SLOTS:
    /** 对于某节点，触发其下游数据 更新 
    */
    void onOutPortDataUpdated(NodeId const nodeId, PortIndex const portIndex);
    
    /** 在分离连接后调用，将空数据传播到指定节点。
     *  是的，链接断开时 触发的正是它
     */
    void propagateEmptyDataTo(NodeId const nodeId, PortIndex const portIndex);

private:
    std::shared_ptr<NodeDelegateModelRegistry> _registry;   // 存放节点名与构造器的映射
    NodeId _nextNodeId;
    std::unordered_map<NodeId       , std::unique_ptr<NodeDelegateModel>> _models;                    // 节点 数组
    std::unordered_set<ConnectionId> _connectivity;                                                   // 链接 数组
    mutable                          std::unordered_map<NodeId, NodeGeometryData> _nodeGeometryData;  // 节点ID 及其对应的几何数据
};

} // namespace QtNodes
