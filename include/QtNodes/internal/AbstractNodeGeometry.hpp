#pragma once

#include "Definitions.hpp"
#include "Export.hpp"

#include <QRectF>
#include <QSize>
#include <QTransform>

namespace QtNodes {

class AbstractGraphModel;

class NODE_EDITOR_PUBLIC AbstractNodeGeometry
{
public:
    AbstractNodeGeometry(AbstractGraphModel &);
    virtual ~AbstractNodeGeometry() {}

    /**
     * 返回节点的大小以及其周围的额外边距，用于考虑绘制效果
     *（例如阴影）或节点的某些部分超出大小矩形的部分
     *（例如端口点）。
     *
     * 默认实现返回 QSize，并在矩形的每一侧增加 20% 的宽度和高度。
     */
    virtual QRectF boundingRect(NodeId const nodeId) const;

    /// 定义节点矩形的直接边界矩形。
    virtual QSize size(NodeId const nodeId) const = 0;

    /**
     * 当端口数量发生变化或嵌入式小部件需要更新时触发此函数。
     */
    virtual void recomputeSize(NodeId const nodeId) const = 0;

    /// 节点坐标系中的端口位置。
    virtual QPointF portPosition(NodeId const nodeId,
                                 PortType const portType,
                                 PortIndex const index) const = 0;

    /// 使用 `portPosition` 和给定的转换矩阵的便利函数。
    virtual QPointF portScenePosition(NodeId const nodeId,
                                      PortType const portType,
                                      PortIndex const index,
                                      QTransform const &t) const;

    /// 定义绘制端口标签的位置。该点对应于字体的基线。
    virtual QPointF portTextPosition(NodeId const nodeId,
                                     PortType const portType,
                                     PortIndex const portIndex) const = 0;

    /** 定义绘制标题的位置。该点对应于字体的基线。*/
    virtual QPointF captionPosition(NodeId const nodeId) const = 0;

    /// 获取标题矩形，估算整个节点的大小时需要用到。
    virtual QRectF captionRect(NodeId const nodeId) const = 0;

    /// 获取嵌入式小部件的位置。如果没有嵌入小部件，可以返回任何值。
    virtual QPointF widgetPosition(NodeId const nodeId) const = 0;
    
    /// 检查端口是否被点击。
    virtual PortIndex checkPortHit(NodeId const nodeId,
                                   PortType const portType,
                                   QPointF const nodePoint) const;
    
    /// 获取调整大小的手柄矩形。
    virtual QRect resizeHandleRect(NodeId const nodeId) const = 0;

protected:
    AbstractGraphModel &_graphModel;
};

} // namespace QtNodes
