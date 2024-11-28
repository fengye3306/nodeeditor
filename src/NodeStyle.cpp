#include "NodeStyle.hpp"

#include <iostream>

#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValueRef>

#include <QtCore/QDebug>

#include "StyleCollection.hpp"

using QtNodes::NodeStyle;

inline void initResources()
{
    Q_INIT_RESOURCE(resources);
}

NodeStyle::NodeStyle()
{
    // Explicit resources inialization for preventing the static initialization
    // order fiasco: https://isocpp.org/wiki/faq/ctors#static-init-order
    initResources();

    // This configuration is stored inside the compiled unit and is loaded statically
    loadJsonFile(":DefaultStyle.json");
}

NodeStyle::NodeStyle(QString jsonText)
{
    loadJsonText(jsonText);
}

NodeStyle::NodeStyle(QJsonObject const &json)
{
    loadJson(json);
}

void NodeStyle::setNodeStyle(QString jsonText)
{
    NodeStyle style(jsonText);

    StyleCollection::setNodeStyle(style);
}

#ifdef STYLE_DEBUG
#define NODE_STYLE_CHECK_UNDEFINED_VALUE(v, variable) \
    { \
        if (v.type() == QJsonValue::Undefined || v.type() == QJsonValue::Null) \
            qWarning() << "Undefined value for parameter:" << #variable; \
    }
#else
#define NODE_STYLE_CHECK_UNDEFINED_VALUE(v, variable)
#endif

#define NODE_STYLE_READ_COLOR(values, variable) \
    { \
        auto valueRef = values[#variable]; \
        NODE_STYLE_CHECK_UNDEFINED_VALUE(valueRef, variable) \
        if (valueRef.isArray()) { \
            auto colorArray = valueRef.toArray(); \
            std::vector<int> rgb; \
            rgb.reserve(3); \
            for (auto it = colorArray.begin(); it != colorArray.end(); ++it) { \
                rgb.push_back((*it).toInt()); \
            } \
            variable = QColor(rgb[0], rgb[1], rgb[2]); \
        } else { \
            variable = QColor(valueRef.toString()); \
        } \
    }

#define NODE_STYLE_WRITE_COLOR(values, variable) \
    { \
        values[#variable] = variable.name(); \
    }

// 这段宏代码利用了 C++ 预处理器的特性，特别是 # 操作符，用来将变量名转换成字符串
#define NODE_STYLE_READ_FLOAT(values, variable) \
    { \
        auto valueRef = values[#variable]; \
        NODE_STYLE_CHECK_UNDEFINED_VALUE(valueRef, variable) \
        variable = valueRef.toDouble(); \
    }

#define NODE_STYLE_WRITE_FLOAT(values, variable) \
    { \
        values[#variable] = variable; \
    }

void NodeStyle::loadJson(QJsonObject const &json)
{
    QJsonValue nodeStyleValues = json["NodeStyle"];

    QJsonObject obj = nodeStyleValues.toObject();

    NODE_STYLE_READ_COLOR(obj, NormalBoundaryColor);
    NODE_STYLE_READ_COLOR(obj, SelectedBoundaryColor);
    NODE_STYLE_READ_COLOR(obj, GradientColor0);
    NODE_STYLE_READ_COLOR(obj, GradientColor1);
    NODE_STYLE_READ_COLOR(obj, GradientColor2);
    NODE_STYLE_READ_COLOR(obj, GradientColor3);
    NODE_STYLE_READ_COLOR(obj, ShadowColor);
    NODE_STYLE_READ_COLOR(obj, FontColor);
    NODE_STYLE_READ_COLOR(obj, FontColorFaded);
    NODE_STYLE_READ_COLOR(obj, ConnectionPointColor);
    NODE_STYLE_READ_COLOR(obj, FilledConnectionPointColor);
    NODE_STYLE_READ_COLOR(obj, WarningColor);
    NODE_STYLE_READ_COLOR(obj, ErrorColor);

    NODE_STYLE_READ_FLOAT(obj, PenWidth);
    NODE_STYLE_READ_FLOAT(obj, HoveredPenWidth);
    NODE_STYLE_READ_FLOAT(obj, ConnectionPointDiameter);

    NODE_STYLE_READ_FLOAT(obj, Opacity);

    // 自定义添加
    NODE_STYLE_READ_FLOAT(obj, FontSize_Node_Title);        // 节点标题文字大小
    NODE_STYLE_READ_FLOAT(obj, FontSize_Node_LinkCirMsg); 
}

QJsonObject NodeStyle::toJson() const
{
    QJsonObject obj;

    NODE_STYLE_WRITE_COLOR(obj, NormalBoundaryColor);
    NODE_STYLE_WRITE_COLOR(obj, SelectedBoundaryColor);
    NODE_STYLE_WRITE_COLOR(obj, GradientColor0);
    NODE_STYLE_WRITE_COLOR(obj, GradientColor1);
    NODE_STYLE_WRITE_COLOR(obj, GradientColor2);
    NODE_STYLE_WRITE_COLOR(obj, GradientColor3);
    NODE_STYLE_WRITE_COLOR(obj, ShadowColor);
    NODE_STYLE_WRITE_COLOR(obj, FontColor);
    NODE_STYLE_WRITE_COLOR(obj, FontColorFaded);
    NODE_STYLE_WRITE_COLOR(obj, ConnectionPointColor);
    NODE_STYLE_WRITE_COLOR(obj, FilledConnectionPointColor);
    NODE_STYLE_WRITE_COLOR(obj, WarningColor);
    NODE_STYLE_WRITE_COLOR(obj, ErrorColor);

    NODE_STYLE_WRITE_FLOAT(obj, PenWidth);
    NODE_STYLE_WRITE_FLOAT(obj, HoveredPenWidth);
    NODE_STYLE_WRITE_FLOAT(obj, ConnectionPointDiameter);

    NODE_STYLE_WRITE_FLOAT(obj, Opacity);

     // 自定义添加
    NODE_STYLE_WRITE_FLOAT(obj, FontSize_Node_Title);        // 节点标题文字大小
    NODE_STYLE_WRITE_FLOAT(obj, FontSize_Node_LinkCirMsg);   // 节点 连接点 描述文字大小

    QJsonObject root;
    root["NodeStyle"] = obj;

    return root;
}
