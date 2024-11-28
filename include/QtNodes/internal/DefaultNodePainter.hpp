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
/// @brief 轻量级类，封装节点的绘制代码。
class NODE_EDITOR_PUBLIC DefaultNodePainter : public AbstractNodePainter
{
public:
    /**
     * @brief 绘制整个节点，包括边框、连接点、标题等。
     * @param painter QPainter对象，用于绘制节点。
     * @param ngo 要绘制的节点图形对象。
     */
    void paint(QPainter *painter, NodeGraphicsObject &ngo) const override;

    /**
     * @brief 绘制节点的矩形边框。
     * @param painter QPainter对象，用于绘制矩形边框。
     * @param ngo 要绘制的节点图形对象。
     */
    void drawNodeRect(QPainter *painter, NodeGraphicsObject &ngo) const;

    /**
     * @brief 绘制节点的连接点，用于连接其他节点。
     * @param painter QPainter对象，用于绘制连接点。
     * @param ngo 要绘制的节点图形对象。
     */
    void drawConnectionPoints(QPainter *painter, NodeGraphicsObject &ngo) const;

    /**
     * @brief 绘制填充的连接点，用于高亮或强调连接点。
     * @param painter QPainter对象，用于绘制填充连接点。
     * @param ngo 要绘制的节点图形对象。
     */
    void drawFilledConnectionPoints(QPainter *painter, NodeGraphicsObject &ngo) const;

    /**
     * @brief 绘制节点的标题栏，用于显示节点的名称或描述。
     * @param painter QPainter对象，用于绘制标题。
     * @param ngo 要绘制的节点图形对象。
     */
    void drawNodeCaption(QPainter *painter, NodeGraphicsObject &ngo) const;

    /**
     * @brief 绘制节点的入口标签，显示连接点的标签或说明。
     * @param painter QPainter对象，用于绘制入口标签。
     * @param ngo 要绘制的节点图形对象。
     */
    void drawEntryLabels(QPainter *painter, NodeGraphicsObject &ngo) const;

    /**
     * @brief 绘制节点的调整矩形，用于支持节点大小调整的功能。
     * @param painter QPainter对象，用于绘制调整矩形。
     * @param ngo 要绘制的节点图形对象。
     */
    void drawResizeRect(QPainter *painter, NodeGraphicsObject &ngo) const;
};

} // namespace QtNodes
