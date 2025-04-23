#ifndef SCENEVIEWPORT_H
#define SCENEVIEWPORT_H

#include <QEvent>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QWheelEvent>

class SceneViewport : public QGraphicsView
{
    Q_OBJECT

public:
    explicit SceneViewport(QWidget *parent = nullptr);

    void updateImage(const QImage &image);

    void fitToViewport();
    void originalSize();

    void setTransformationMode(Qt::TransformationMode mode);

signals:
    void scaleChanged(double scale);
    void mousePressed();
    void mouseDblPressed();
    void mouseReleased();

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QGraphicsScene *scene;
    QGraphicsPixmapItem *pixmapItem;
    double scaleFactor = 1.15;
    double scaleCurrent = 1.0;
};

#endif  // !_3DVIEWPORT_H
