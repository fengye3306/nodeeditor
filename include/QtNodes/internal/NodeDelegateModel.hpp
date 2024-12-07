#pragma once

#include <memory>

#include <QtWidgets/QWidget>

#include "Definitions.hpp"
#include "Export.hpp"
#include "NodeData.hpp"
#include "NodeStyle.hpp"
#include "Serializable.hpp"

namespace QtNodes {

class StyleCollection;

/**
 * The class wraps Node-specific data operations and propagates it to
 * the nesting DataFlowGraphModel which is a subclass of
 * AbstractGraphModel.
 * This class is the same what has been called NodeDataModel before v3.
 * 这个类包装了节点（Node）相关的数据操作，并将其传播到嵌套的
 * DataFlowGraphModel，该模型是 AbstractGraphModel 的子类。
 * 此类在 v3 版本之前被称为 NodeDataModel。
 */
class NODE_EDITOR_PUBLIC NodeDelegateModel : public QObject, public Serializable
{
    Q_OBJECT

public:
    NodeDelegateModel();

    virtual ~NodeDelegateModel() = default;

    /// It is possible to hide caption in GUI
    /// 是否展示标题（在 GUI 中隐藏或展示标题）
    virtual bool captionVisible() const { return true; }

    /// Caption is used in GUI
    /// 获取标题文本（用于在 GUI 中显示）
    virtual QString caption() const = 0;

    /// It is possible to hide port caption in GUI
    /// 是否在 GUI 中展示端口的标题（如输入输出端口）
    virtual bool portCaptionVisible(PortType, PortIndex) const { return false; }

    /// Port caption is used in GUI to label individual ports
    /// 获取端口的标题文本（用于在 GUI 中标记每个端口）
    virtual QString portCaption(PortType, PortIndex) const { return QString(); }

    /// Name makes this model unique
    /// 获取模型名称，使此模型唯一（用来区分不同的模型）
    virtual QString name() const = 0;

public:
    /// @brief 保存模型数据为 JSON 对象
    /// @return 
    QJsonObject save() const override;

    /// @brief 从 JSON 对象加载模型数据
    /// @param  
    void load(QJsonObject const &) override;

public:
    /// @brief  获取指定端口类型的端口数量（如输入端口或输出端口）
    /// @param portType 
    /// @return 
    virtual unsigned int nPorts(PortType portType) const = 0;

    /// @brief 获取指定端口类型和端口索引的数据类型
    /// @param portType 
    /// @param portIndex 
    /// @return 
    virtual NodeDataType dataType(PortType portType, PortIndex portIndex) const = 0;

public:

    /// @brief 获取端口的连接策略（决定该端口是否允许多个连接）
    /// @param  
    /// @param  
    /// @return 
    virtual ConnectionPolicy portConnectionPolicy(PortType, PortIndex) const;

    /// @brief 获取节点的样式
    /// @return 
    NodeStyle const &nodeStyle() const;

    /// @brief 设置节点的样式
    /// @param style 
    void setNodeStyle(NodeStyle const &style);

public:

    /// @brief 设置输入数据到指定的端口索引
    /// @param nodeData 
    /// @param portIndex 
    virtual void setInData(std::shared_ptr<NodeData> nodeData, PortIndex const portIndex) = 0;

    /// @brief 获取指定端口的输出数据
    /// @param port 
    /// @return 
    virtual std::shared_ptr<NodeData> outData(PortIndex const port) = 0;

    /**
   * It is recommented to preform a lazy initialization for the
   * embedded widget and create it inside this function, not in the
   * constructor of the current model.
   *
   * Our Model Registry is able to shortly instantiate models in order
   * to call the non-static `Model::name()`. If the embedded widget is
   * allocated in the constructor but not actually embedded into some
   * QGraphicsProxyWidget, we'll gonna have a dangling pointer.
   * 
   * 懒初始化建议在此函数中创建嵌入式控件，而不是在构造函数中创建。
   *
   * 模型注册表（Model Registry）在调用非静态方法 `Model::name()` 时会短暂实例化模型。
   * 如果嵌入式控件在构造函数中分配但未嵌入到 QGraphicsProxyWidget 中，会导致悬空指针。
   */
    virtual QWidget *embeddedWidget() = 0;

    // 用于展示详细配置界面
    virtual QWidget *detailedSettingsWidget() = 0;


    /// @brief 判断控件是否可调整大小
    /// @return 
    virtual bool resizable() const { return false; }


public Q_SLOTS:
    
    /// 当输入连接创建时调用（可选重载）
    virtual void inputConnectionCreated(ConnectionId const &) {}

    /// 当输入连接删除时调用（可选重载）
    virtual void inputConnectionDeleted(ConnectionId const &) {}

    /// 当输出连接创建时调用（可选重载）
    virtual void outputConnectionCreated(ConnectionId const &) {}

    /// 当输出连接删除时调用（可选重载）
    virtual void outputConnectionDeleted(ConnectionId const &) {}

Q_SIGNALS:

    /// Triggers the updates in the nodes downstream.
    /// 当数据更新时触发此信号，更新下游的节点
    void dataUpdated(PortIndex const index);

    /// Triggers the propagation of the empty data downstream.
    /// 当数据无效时，触发此信号，传播空数据到下游
    void dataInvalidated(PortIndex const index);


    /// @brief  计算开始时触发此信号
    void computingStarted();

    /// @brief 计算结束时触发此信号
    void computingFinished();

    /// @brief 当嵌入式控件大小更新时触发此信号
    void embeddedWidgetSizeUpdated();

    /// Call this function before deleting the data associated with ports.
    /**
   * The function notifies the Graph Model and makes it remove and recompute the
   * affected connection addresses.
   * 
   * 在删除与端口相关的数据之前调用此函数，通知图形模型移除并重新计算受影响的连接地址
   */
    void portsAboutToBeDeleted(PortType const portType, PortIndex const first, PortIndex const last);

    /// Call this function when data and port moditications are finished.
    /// 在端口和数据修改完成后调用此函数
    void portsDeleted();

    /// Call this function before inserting the data associated with ports.
    /**
   * The function notifies the Graph Model and makes it recompute the affected
   * connection addresses.
   * 在插入与端口相关的数据之前调用此函数，通知图形模型重新计算受影响的连接地址
   */
    void portsAboutToBeInserted(PortType const portType,
                                PortIndex const first,
                                PortIndex const last);

    /// Call this function when data and port moditications are finished.
    /// 在端口和数据插入完成后调用此函数
    void portsInserted();

private:
    /// @brief 保存节点样式
    NodeStyle _nodeStyle;
};

} // namespace QtNodes
