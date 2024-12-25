#pragma once

#include "BasicGraphicsScene.hpp"
#include "DataFlowGraphModel.hpp"
#include "Export.hpp"

namespace QtNodes {

// 视图类
class NODE_EDITOR_PUBLIC DataFlowGraphicsScene : public BasicGraphicsScene
{
    Q_OBJECT
public:
    DataFlowGraphicsScene(DataFlowGraphModel &graphModel, QObject *parent = nullptr);
    ~DataFlowGraphicsScene() = default;
public:
    std::vector<NodeId> selectedNodes() const;

public:
    // 菜单
    QMenu *createSceneMenu(QPointF const scenePos) override;

public Q_SLOTS:
    bool save() const;
    bool load();

    QJsonObject save_GetJson() const{
        QJsonObject  jsonObject =  _graphModel.save();
        return jsonObject;
    }
    void load_SetJson(QJsonObject const &json) {
        clearScene();
        _graphModel.load(json);
        Q_EMIT sceneLoaded();
    }

Q_SIGNALS:
    void sceneLoaded();

private:
    DataFlowGraphModel &_graphModel;
};

} // namespace QtNodes
