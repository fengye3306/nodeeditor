#pragma once

#include <QtWidgets/QGraphicsView>

#include "Export.hpp"

namespace QtNodes {

// 前向声明
class BasicGraphicsScene;

/**
 * @brief 图形视图类，能够渲染 `BasicGraphicsScene` 中的对象。
 * 该类是一个中央视图，提供对图形场景的交互和展示功能。
 */
class NODE_EDITOR_PUBLIC GraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    /// 缩放范围结构体
    struct ScaleRange
    {
        double minimum = 0; ///< 最小缩放比例
        double maximum = 0; ///< 最大缩放比例
    };

public:
    /**
     * @brief 默认构造函数。
     * @param parent 父控件，默认为 nullptr。
     */
    GraphicsView(QWidget *parent = Q_NULLPTR);

    /**
     * @brief 构造函数，接受一个场景对象。
     * @param scene 需要展示的 BasicGraphicsScene 对象。
     * @param parent 父控件，默认为 nullptr。
     */
    GraphicsView(BasicGraphicsScene *scene, QWidget *parent = Q_NULLPTR);

    /// 禁止拷贝构造函数
    GraphicsView(const GraphicsView &) = delete;

    /// 禁止赋值操作符
    GraphicsView operator=(const GraphicsView &) = delete;

    /**
     * @brief 获取清除选中项的操作。
     * @return 返回对应的 QAction。
     */
    QAction *clearSelectionAction() const;

    /**
     * @brief 获取删除选中项的操作。
     * @return 返回对应的 QAction。
     */
    QAction *deleteSelectionAction() const;

    /**
     * @brief 设置视图的场景。
     * @param scene 需要设置的场景对象。
     */
    void setScene(BasicGraphicsScene *scene);

    /**
     * @brief 居中视图中的场景。
     * 将视图中的场景内容居中显示。
     */
    void centerScene();

    /**
     * @brief 设置缩放范围。
     * @param minimum 最小缩放比例，默认为 0，表示无限缩小。
     * @param maximum 最大缩放比例，默认为 0，表示无限放大。
     */
    void setScaleRange(double minimum = 0, double maximum = 0);

    /**
     * @brief 设置缩放范围。
     * @param range 缩放范围结构体，包含最小和最大缩放比例。
     */
    void setScaleRange(ScaleRange range);

    /**
     * @brief 获取当前视图的缩放比例。
     * @return 当前缩放比例。
     */
    double getScale() const;

public Q_SLOTS:
    /**
     * @brief 放大视图。
     * 调用该函数会增大当前视图的缩放比例。
     */
    void scaleUp();

    /**
     * @brief 缩小视图。
     * 调用该函数会减小当前视图的缩放比例。
     */
    void scaleDown();

    /**
     * @brief 设置视图的缩放比例。
     * @param scale 缩放比例。
     */
    void setupScale(double scale);

    /**
     * @brief 删除选中的对象。
     * 调用该函数会删除当前视图中选中的对象。
     */
    void onDeleteSelectedObjects();

    /**
     * @brief 复制选中的对象。
     * 调用该函数会复制当前视图中选中的对象。
     */
    void onDuplicateSelectedObjects();

    /**
     * @brief 复制选中的对象到剪贴板。
     * 调用该函数会将选中的对象复制到剪贴板中。
     */
    void onCopySelectedObjects();

    /**
     * @brief 从剪贴板粘贴对象。
     * 调用该函数会将剪贴板中的对象粘贴到视图中。
     */
    void onPasteObjects();

Q_SIGNALS:
    /**
     * @brief 缩放比例变化时触发的信号。
     * @param scale 当前的缩放比例。
     */
    void scaleChanged(double scale);

protected:
    /**
     * @brief 重写右键菜单事件。
     * @param event 右键菜单事件。
     */
    void contextMenuEvent(QContextMenuEvent *event) override;

    /**
     * @brief 重写滚轮事件，用于缩放。
     * @param event 滚轮事件。
     */
    void wheelEvent(QWheelEvent *event) override;

    /**
     * @brief 重写按键按下事件。
     * @param event 按键按下事件。
     */
    void keyPressEvent(QKeyEvent *event) override;

    /**
     * @brief 重写按键释放事件。
     * @param event 按键释放事件。
     */
    void keyReleaseEvent(QKeyEvent *event) override;

    /**
     * @brief 重写鼠标按下事件。
     * @param event 鼠标按下事件。
     */
    void mousePressEvent(QMouseEvent *event) override;

    /**
     * @brief 重写鼠标移动事件。
     * @param event 鼠标移动事件。
     */
    void mouseMoveEvent(QMouseEvent *event) override;

    /**
     * @brief 重写绘制背景事件。
     * @param painter 绘图对象。
     * @param r 绘制区域。
     */
    void drawBackground(QPainter *painter, const QRectF &r) override;

    /**
     * @brief 重写显示事件。
     * @param event 显示事件。
     */
    void showEvent(QShowEvent *event) override;

protected:
    /**
     * @brief 获取当前场景对象。
     * @return 当前场景对象的指针。
     */
    BasicGraphicsScene *nodeScene();

    /**
     * @brief 计算粘贴对象时的位置。
     * @return 计算得到的场景粘贴位置。
     */
    QPointF scenePastePosition();

private:
    QAction *_clearSelectionAction     = nullptr;  ///< 清除选中项的动作
    QAction *_deleteSelectionAction    = nullptr;  ///< 删除选中项的动作
    QAction *_duplicateSelectionAction = nullptr;  ///< 复制选中项的动作
    QAction *_copySelectionAction      = nullptr;  ///< 复制选中项到剪贴板的动作
    QAction *_pasteAction              = nullptr;  ///< 从剪贴板粘贴的动作

    QPointF    _clickPos;    ///< 鼠标点击位置
    ScaleRange _scaleRange;  ///< 缩放范围
};

} // namespace QtNodes
