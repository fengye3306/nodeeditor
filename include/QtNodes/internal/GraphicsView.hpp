#pragma once

#include <QtWidgets/QGraphicsView>
#include "Export.hpp"

namespace QtNodes {

class BasicGraphicsScene;


/**
 * @brief 视口
 */
class NODE_EDITOR_PUBLIC GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    
    /**
     * @brief 缩放范围结构体
     */
    struct ScaleRange
    {
        double minimum = 0; ///< 最小缩放比例
        double maximum = 0; ///< 最大缩放比例
    };
public:

    // construct
    GraphicsView(QWidget *parent = Q_NULLPTR);

    // construct
    GraphicsView(BasicGraphicsScene *scene, QWidget *parent = Q_NULLPTR);
    // 禁止拷贝
    GraphicsView(const GraphicsView &) = delete;
    // 禁止赋值
    GraphicsView operator=(const GraphicsView &) = delete;

    QAction *clearSelectionAction() const;
    QAction *deleteSelectionAction() const;

    // 配置视场
    void setScene(BasicGraphicsScene *scene);
    // 调整视图（GraphicsView），使场景（scene）位于视图的中心
    void centerScene();

    // 设置缩放范围
    void setScaleRange(double minimum = 0, double maximum = 0);
    void setScaleRange(ScaleRange range);

    // 当前缩放比例
    double getScale() const;

public Q_SLOTS:
    /**
     * @brief 放大视图。
     */
    void scaleUp();
    /**
     * @brief 缩小视图。
     */
    void scaleDown();
    /**
     * @brief 设置视图的缩放比例*/
    void setupScale(double scale);
    /**
     * @brief 删除选中的对象。*/
    void onDeleteSelectedObjects();
    /**
     * @brief 复制选中的对象。*/
    void onDuplicateSelectedObjects();
    /**
     * @brief 复制选中的对象到剪贴板 */
    void onCopySelectedObjects();
    /**
     * @brief 从剪贴板粘贴对象*/
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
     * @return 当前场景对象的指针 */
    BasicGraphicsScene *nodeScene();

    /**
     * @brief 计算粘贴对象时的位置。
     * @return 计算得到的场景粘贴位置 */
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
