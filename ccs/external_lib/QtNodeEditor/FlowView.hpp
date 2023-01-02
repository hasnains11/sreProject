#pragma once

#include <QtWidgets/QGraphicsView>

namespace QtNodes
{

class FlowScene;

class FlowView
  : public QGraphicsView
{
public:
    FlowView(QWidget *parent = Q_NULLPTR);
    FlowView(FlowScene *scene);

public slots:

  void scaleUp();

  void scaleDown();

protected:

  void contextMenuEvent(QContextMenuEvent *event) override;

  void wheelEvent(QWheelEvent *event) override;

  void keyPressEvent(QKeyEvent *event) override;

  void keyReleaseEvent(QKeyEvent *event) override;

  void drawBackground(QPainter* painter, const QRectF& r) override;

  void showEvent(QShowEvent *event) override;

  void mouseMoveEvent(QMouseEvent* event) override;

  void mousePressEvent(QMouseEvent* event) override;

private:

  //FlowScene* _scene;
};
}
