#ifndef SCENEVIEWPORT_H
#define SCENEVIEWPORT_H

#include <qglobal.h>
#include <qmatrix4x4.h>
#include <qsharedpointer.h>
#include <qvectornd.h>

#include <QImage>
#include <QPoint>
#include <QWidget>

class SceneViewport : public QWidget
{
    Q_OBJECT

public:
    explicit SceneViewport(QWidget *parent = nullptr);

    const QImage &image() const { return m_image; }

    void setSceneVertices(const QVector<QVector<QVector3D>> &vertices);
    void setRotation(const QVector3D& rotation);
    void setZoom(qreal zoom);

signals:
    void rotationChanged(QVector3D rotation);
    void zoomChanged(qreal zoom);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    void resizeImageToFit();
    void paintAxes(const QMatrix4x4 &rotation);

    QMatrix4x4 normalizeMatrix(QVector<QVector<QVector3D>> &vertices);
    QMatrix4x4 rotationMatrix(const QVector3D &rotation);
    QMatrix4x4 perspectiveProjectionMatrix(float sw, float sh, float zf,
                                           float zb);
    QMatrix4x4 orthographicProjectionMatrix(float sw, float sh, float zf,
                                            float zb);
    QMatrix4x4 translationMatrix(const QVector3D &t);
    QMatrix4x4 scaleMatrix(const QVector3D &s);

    QVector<QVector<QVector3D>> m_vertices;
    QVector3D m_rotation;
    qreal m_zoom;

    QImage m_image;
    QPoint m_lastMousePos;
    bool m_isDragging = false;
};

#endif  // !SCENEVIEWPORT_H
