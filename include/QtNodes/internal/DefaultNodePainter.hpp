#pragma once

#include <QtGui/QPainter>

#include "AbstractNodePainter.hpp"
#include "Definitions.hpp"

namespace QtNodes {

class BasicGraphicsScene;
class GraphModel;
class NodeGeometry;
class NodeGraphicsObject;
class NodeState;

/// @ Lightweight class incapsulating paint code.
class NODE_EDITOR_PUBLIC DefaultNodePainter : public AbstractNodePainter
{
public:
    // 重写的 paint 函数，负责绘制节点
    void paint(QPainter *painter, NodeGraphicsObject &ngo) const override;
    // 绘制节点的矩形边框
    void drawNodeRect(QPainter *painter, NodeGraphicsObject &ngo) const;
    // 绘制连接点
    void drawConnectionPoints(QPainter *painter, NodeGraphicsObject &ngo) const;
    // 绘制填充的连接点
    void drawFilledConnectionPoints(QPainter *painter, NodeGraphicsObject &ngo) const;
    // 绘制节点的标题
    void drawNodeCaption(QPainter *painter, NodeGraphicsObject &ngo) const;
    // 绘制入口标签
    void drawEntryLabels(QPainter *painter, NodeGraphicsObject &ngo) const;
    // 绘制调整大小矩形
    void drawResizeRect(QPainter *painter, NodeGraphicsObject &ngo) const;
};
} // namespace QtNodes
