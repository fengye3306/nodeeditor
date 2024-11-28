#include "DefaultNodePainter.hpp"

#include <cmath>

#include <QtCore/QMargins>

#include "AbstractGraphModel.hpp"
#include "AbstractNodeGeometry.hpp"
#include "BasicGraphicsScene.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "ConnectionIdUtils.hpp"
#include "NodeGraphicsObject.hpp"
#include "NodeState.hpp"
#include "StyleCollection.hpp"

namespace QtNodes {

void DefaultNodePainter::paint(QPainter *painter, NodeGraphicsObject &ngo) const
{
    // TODO?
    //AbstractNodeGeometry & geometry = ngo.nodeScene()->nodeGeometry();
    //geometry.recomputeSizeIfFontChanged(painter->font());

    // 绘制节点外边框
    drawNodeRect(painter, ngo);
    
    drawConnectionPoints(painter, ngo);

    drawFilledConnectionPoints(painter, ngo);

    drawNodeCaption(painter, ngo);

    drawEntryLabels(painter, ngo);

    drawResizeRect(painter, ngo);
}


// 绘制节点的外边框 —— 纯纯的外边框 
void DefaultNodePainter::drawNodeRect
    (QPainter *painter, NodeGraphicsObject &ngo) const
{
    AbstractGraphModel &model = ngo.graphModel();

    NodeId const nodeId = ngo.nodeId();

    AbstractNodeGeometry &geometry = ngo.nodeScene()->nodeGeometry();

    QSize size = geometry.size(nodeId);

    QJsonDocument json = QJsonDocument::fromVariant(model.nodeData(nodeId, NodeRole::Style));

    NodeStyle nodeStyle(json.object());

    auto color = ngo.isSelected() ? nodeStyle.SelectedBoundaryColor : nodeStyle.NormalBoundaryColor;

    if (ngo.nodeState().hovered()) {
        QPen p(color, nodeStyle.HoveredPenWidth);
        painter->setPen(p);
    } else {
        QPen p(color, nodeStyle.PenWidth);
        painter->setPen(p);
    }

    QLinearGradient gradient(QPointF(0.0, 0.0), QPointF(2.0, size.height()));

    gradient.setColorAt(0.0, nodeStyle.GradientColor0);
    gradient.setColorAt(0.10, nodeStyle.GradientColor1);
    gradient.setColorAt(0.90, nodeStyle.GradientColor2);
    gradient.setColorAt(1.0, nodeStyle.GradientColor3);

    painter->setBrush(gradient);

    QRectF boundary(0, 0, size.width(), size.height());

    double const radius = 3.0;

    painter->drawRoundedRect(boundary, radius, radius);
}

void DefaultNodePainter::drawConnectionPoints(QPainter *painter, NodeGraphicsObject &ngo) const
{
    AbstractGraphModel &model = ngo.graphModel();
    NodeId const nodeId = ngo.nodeId();
    AbstractNodeGeometry &geometry = ngo.nodeScene()->nodeGeometry();

    QJsonDocument json = QJsonDocument::fromVariant(model.nodeData(nodeId, NodeRole::Style));
    NodeStyle nodeStyle(json.object());

    auto const &connectionStyle = StyleCollection::connectionStyle();

    float diameter = nodeStyle.ConnectionPointDiameter;
    auto reducedDiameter = diameter * 0.6;

    for (PortType portType : {PortType::Out, PortType::In}) {
        size_t const n = model
                             .nodeData(nodeId,
                                       (portType == PortType::Out) ? NodeRole::OutPortCount
                                                                   : NodeRole::InPortCount)
                             .toUInt();

        for (PortIndex portIndex = 0; portIndex < n; ++portIndex) {
            QPointF p = geometry.portPosition(nodeId, portType, portIndex);

            auto const &dataType = model.portData(nodeId, portType, portIndex, PortRole::DataType)
                                       .value<NodeDataType>();

            double r = 1.0;

            NodeState const &state = ngo.nodeState();

            if (auto const *cgo = state.connectionForReaction()) {
                PortType requiredPort = cgo->connectionState().requiredPort();

                if (requiredPort == portType) {
                    ConnectionId possibleConnectionId = makeCompleteConnectionId(cgo->connectionId(),
                                                                                 nodeId,
                                                                                 portIndex);

                    bool const possible = model.connectionPossible(possibleConnectionId);

                    auto cp = cgo->sceneTransform().map(cgo->endPoint(requiredPort));
                    cp = ngo.sceneTransform().inverted().map(cp);

                    auto diff = cp - p;
                    double dist = std::sqrt(QPointF::dotProduct(diff, diff));

                    if (possible) {
                        double const thres = 40.0;
                        r = (dist < thres) ? (2.0 - dist / thres) : 1.0;
                    } else {
                        double const thres = 80.0;
                        r = (dist < thres) ? (dist / thres) : 1.0;
                    }
                }
            }

            if (connectionStyle.useDataDefinedColors()) {
                painter->setBrush(connectionStyle.normalColor(dataType.id));
            } else {
                painter->setBrush(nodeStyle.ConnectionPointColor);
            }

            painter->drawEllipse(p, reducedDiameter * r, reducedDiameter * r);
        }
    }

    if (ngo.nodeState().connectionForReaction()) {
        ngo.nodeState().resetConnectionForReaction();
    }
}

void DefaultNodePainter::drawFilledConnectionPoints(QPainter *painter, NodeGraphicsObject &ngo) const
{
    AbstractGraphModel &model = ngo.graphModel();
    NodeId const nodeId = ngo.nodeId();
    AbstractNodeGeometry &geometry = ngo.nodeScene()->nodeGeometry();

    QJsonDocument json = QJsonDocument::fromVariant(model.nodeData(nodeId, NodeRole::Style));
    NodeStyle nodeStyle(json.object());

    auto diameter = nodeStyle.ConnectionPointDiameter;

    for (PortType portType : {PortType::Out, PortType::In}) {
        size_t const n = model
                             .nodeData(nodeId,
                                       (portType == PortType::Out) ? NodeRole::OutPortCount
                                                                   : NodeRole::InPortCount)
                             .toUInt();

        for (PortIndex portIndex = 0; portIndex < n; ++portIndex) {
            QPointF p = geometry.portPosition(nodeId, portType, portIndex);

            auto const &connected = model.connections(nodeId, portType, portIndex);

            if (!connected.empty()) {
                auto const &dataType = model
                                           .portData(nodeId, portType, portIndex, PortRole::DataType)
                                           .value<NodeDataType>();

                auto const &connectionStyle = StyleCollection::connectionStyle();
                if (connectionStyle.useDataDefinedColors()) {
                    QColor const c = connectionStyle.normalColor(dataType.id);
                    painter->setPen(c);
                    painter->setBrush(c);
                } else {
                    painter->setPen(nodeStyle.FilledConnectionPointColor);
                    painter->setBrush(nodeStyle.FilledConnectionPointColor);
                }

                painter->drawEllipse(p, diameter * 0.4, diameter * 0.4);
            }
        }
    }
}
void DefaultNodePainter::drawNodeCaption(QPainter *painter, NodeGraphicsObject &ngo) const
{
    // 获取与节点相关的模型和几何信息
    AbstractGraphModel &model = ngo.graphModel();
    NodeId const nodeId = ngo.nodeId();
    AbstractNodeGeometry &geometry = ngo.nodeScene()->nodeGeometry();


    // 从节点数据中获取样式信息，构造 NodeStyle 对象
    QJsonDocument json = QJsonDocument::fromVariant(model.nodeData(nodeId, NodeRole::Style));
    NodeStyle nodeStyle(json.object());

    // 检查节点是否应显示标题
    if (!model.nodeData(nodeId, NodeRole::CaptionVisible).toBool())
        return;

    // 获取节点的标题文本
    QString const name = model.nodeData(nodeId, NodeRole::Caption).toString();

    // 设置字体样式：粗体和大小
    QFont f = painter->font();
    f.setBold(true);
    float float_PainterFontSize = f.pointSize();
    f.setPointSize(nodeStyle.FontSize_Node_Title); // 设置标题字体大小

    // 获取标题的位置
    QPointF position = geometry.captionPosition(nodeId);

    // 设置画笔字体和颜色，并绘制标题文本
    painter->setFont(f);
    painter->setPen(nodeStyle.FontColor);
    painter->drawText(position, name);
    

    // 取消粗体设置，恢复原始字体
    f.setBold(false);
    f.setPointSize(float_PainterFontSize);
    painter->setFont(f);
}


void DefaultNodePainter::drawEntryLabels(QPainter *painter, NodeGraphicsObject &ngo) const
{
    AbstractGraphModel &model = ngo.graphModel();
    NodeId const nodeId = ngo.nodeId();
    AbstractNodeGeometry &geometry = ngo.nodeScene()->nodeGeometry();

    QJsonDocument json = QJsonDocument::fromVariant(model.nodeData(nodeId, NodeRole::Style));
    NodeStyle nodeStyle(json.object());

    for (PortType portType : {PortType::Out, PortType::In}) {
        unsigned int n = model.nodeData<unsigned int>(nodeId,
                                                      (portType == PortType::Out)
                                                          ? NodeRole::OutPortCount
                                                          : NodeRole::InPortCount);

        for (PortIndex portIndex = 0; portIndex < n; ++portIndex) {
            auto const &connected = model.connections(nodeId, portType, portIndex);

            QPointF p = geometry.portTextPosition(nodeId, portType, portIndex);

            if (connected.empty())
                painter->setPen(nodeStyle.FontColorFaded);
            else
                painter->setPen(nodeStyle.FontColor);

            QString s;

            if (model.portData<bool>(nodeId, portType, portIndex, PortRole::CaptionVisible)) {
                s = model.portData<QString>(nodeId, portType, portIndex, PortRole::Caption);
            } else {
                auto portData = model.portData(nodeId, portType, portIndex, PortRole::DataType);

                s = portData.value<NodeDataType>().name;
            }

            // 设置字体样式：粗体和大小
            QFont f = painter->font();
            // float old_Fontsize = f.pixelSize();

            f.setPixelSize(nodeStyle.FontSize_Node_LinkCirMsg);
            painter->setFont(f);

            painter->drawText(p, s);
            // f.setPixelSize(old_Fontsize);
            painter->setFont(f);
        }
    }
}

void DefaultNodePainter::drawResizeRect(QPainter *painter, NodeGraphicsObject &ngo) const
{
    AbstractGraphModel &model = ngo.graphModel();
    NodeId const nodeId = ngo.nodeId();
    AbstractNodeGeometry &geometry = ngo.nodeScene()->nodeGeometry();

    if (model.nodeFlags(nodeId) & NodeFlag::Resizable) {
        painter->setBrush(Qt::gray);

        painter->drawEllipse(geometry.resizeHandleRect(nodeId));
    }
}

} // namespace QtNodes
