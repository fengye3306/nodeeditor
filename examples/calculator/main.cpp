#include <QtNodes/ConnectionStyle>
#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/GraphicsView>
#include <QtNodes/NodeData>
#include <QtNodes/NodeDelegateModelRegistry>

#include <QtGui/QScreen>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QVBoxLayout>

#include <QtGui/QScreen>



#include <vector>
#include <string>
#include <QDebug>
#include <QIcon>

#include <QGraphicsView>
#include <QGraphicsScene>

#include <QtNodes/NodeDelegateModelRegistry>
#include <QLibrary>
#include <QDebug>
#include <QDir>
#include <QFileInfo>



#include "AdditionModel.hpp"
#include "DivisionModel.hpp"
#include "MultiplicationModel.hpp"
#include "NumberDisplayDataModel.hpp"
#include "NumberSourceDataModel.hpp"
#include "SubtractionModel.hpp"

using QtNodes::ConnectionStyle;
using QtNodes::DataFlowGraphicsScene;
using QtNodes::DataFlowGraphModel;
using QtNodes::GraphicsView;
using QtNodes::NodeDelegateModelRegistry;

// static std::shared_ptr<NodeDelegateModelRegistry> registerDataModels()
// {
//     auto ret = std::make_shared<NodeDelegateModelRegistry>();
//     ret->registerModel<NumberSourceDataModel>("Sources");

//     ret->registerModel<NumberDisplayDataModel>("Displays");

//     ret->registerModel<AdditionModel>("Operators");

//     ret->registerModel<SubtractionModel>("Operators");

//     ret->registerModel<MultiplicationModel>("Operators");

//     ret->registerModel<DivisionModel>("Operators");

//     return ret;
// }

static void setStyle()
{
    ConnectionStyle::setConnectionStyle(
        R"(
  {
    "ConnectionStyle": {
      "ConstructionColor": "gray",
      "NormalColor": "black",
      "SelectedColor": "gray",
      "SelectedHaloColor": "deepskyblue",
      "HoveredColor": "deepskyblue",

      "LineWidth": 3.0,
      "ConstructionLineWidth": 2.0,
      "PointDiameter": 10.0,

      "UseDataDefinedColors": true
    }
  }
  )");
}



/// 节点安装
std::shared_ptr<QtNodes::NodeDelegateModelRegistry> 
    registerDataModels(QString str_Plugin_path){

    auto ret =  // 节点
      std::make_shared<QtNodes::NodeDelegateModelRegistry>();
    
    QDir pluginDir(str_Plugin_path);
    if (!pluginDir.exists()) {
        qDebug() << "目录不存在：" << str_Plugin_path;
        return ret;
    }

    // 遍历目录中的所有动态链接库文件
    QStringList filters;
    filters << "*.so" << "*.dll" << "*.dylib"; // 定义需要查找的文件类型
    
    QFileInfoList entries = pluginDir.entryInfoList(filters, QDir::Files); // 只列出文件
    for (const QFileInfo& entry : entries) {
        QLibrary library(entry.absoluteFilePath());

        // 尝试加载库
        if (!library.load()) {
            qDebug() << "库加载失败：" << library.errorString();
            continue;
        }

        // 获取 PLUGIN_OUTPUT 函数的地址
        using PluginOutputFunc 
          = bool(*)(std::shared_ptr<QtNodes::NodeDelegateModelRegistry>);
        PluginOutputFunc pluginOutput 
          = reinterpret_cast<PluginOutputFunc>(library.resolve("PLUGIN_OUTPUT"));
        if (!pluginOutput) {
            qDebug() << "未找到 PLUGIN_OUTPUT 符号：" << library.errorString();
        } else {
            // 调用 PLUGIN_OUTPUT 函数
            pluginOutput(ret);
        }

        // 即使函数调用失败，也应确保正确卸载库
        // library.unload();
    }
    
    return ret;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    setStyle();

    std::shared_ptr<NodeDelegateModelRegistry> registry 
      = registerDataModels("./framework_plugin/");

    QWidget mainWidget;

    auto menuBar = new QMenuBar();
    QMenu *menu = menuBar->addMenu("File");

    auto saveAction = menu->addAction("Save Scene");
    saveAction->setShortcut(QKeySequence::Save);

    auto loadAction = menu->addAction("Load Scene");
    loadAction->setShortcut(QKeySequence::Open);

    QVBoxLayout *l = new QVBoxLayout(&mainWidget);

    DataFlowGraphModel dataFlowGraphModel(registry);

    l->addWidget(menuBar);
    auto scene = new DataFlowGraphicsScene(dataFlowGraphModel, &mainWidget);

    auto view = new GraphicsView(scene);
    l->addWidget(view);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);

    QObject::connect(saveAction, &QAction::triggered, scene, [scene, &mainWidget]() {
        if (scene->save())
            mainWidget.setWindowModified(false);
    });
    
    QObject::connect(loadAction, &QAction::triggered, scene, &DataFlowGraphicsScene::load);

    QObject::connect(scene, &DataFlowGraphicsScene::sceneLoaded, view, &GraphicsView::centerScene);

    QObject::connect(scene, &DataFlowGraphicsScene::modified, &mainWidget, [&mainWidget]() {
        mainWidget.setWindowModified(true);
    });

    mainWidget.setWindowTitle("[*]Data Flow: simplest calculator");
    mainWidget.resize(800, 600);
    // Center window.
    mainWidget.move(QApplication::primaryScreen()->availableGeometry().center()
                    - mainWidget.rect().center());
    mainWidget.showNormal();

    return app.exec();
}
