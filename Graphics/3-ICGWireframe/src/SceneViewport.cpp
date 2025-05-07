#include "SceneViewport.h"

#include <qbrush.h>
#include <qglobal.h>
#include <qmath.h>
#include <qmatrix4x4.h>
#include <qnamespace.h>
#include <qvectornd.h>

#include <QMouseEvent>
#include <QPainter>
#include <QWheelEvent>
SceneViewport::SceneViewport(QWidget *parent)
    : QWidget(parent),
      m_image(size(), QImage::Format_ARGB32),
      m_rotation(0, 0, 0),
      m_zoom(-2)
{}

void SceneViewport::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    float aspectRatio =
        static_cast<float>(m_image.size().width()) / m_image.size().height();

    QMatrix4x4 normalize = normalizeMatrix(m_vertices);
    QMatrix4x4 rotate = rotationMatrix(m_rotation);
    QMatrix4x4 translate = translationMatrix(QVector3D(0, 0, 20));
    QMatrix4x4 projection =
        perspectiveProjectionMatrix(aspectRatio, 1, m_zoom, 1000);

    QMatrix4x4 modelView = translate * rotate * normalize;
    QVector<QVector<QVector3D>> projectedShapes;
    projectedShapes.reserve(m_vertices.size());

    for (const QVector<QVector3D> &shape : m_vertices) {
        projectedShapes.append(QVector<QVector3D>());
        projectedShapes.back().reserve(shape.size());

        for (const QVector3D &v : shape) {
            projectedShapes.back().append(
                v.project(modelView, projection, m_image.rect()));
        }
    }

    m_image.fill(Qt::white);

    QPainter imagePainter(&m_image);
    imagePainter.setRenderHint(QPainter::Antialiasing, true);
    imagePainter.setCompositionMode(QPainter::CompositionMode_Darken);

    float maxZ = projectedShapes.at(0).at(0).z();
    float minZ = projectedShapes.at(0).at(0).z();

    for (const QVector<QVector3D> &shape : projectedShapes) {
        for (const QVector3D &v : shape) {
            maxZ = qMax(maxZ, v.z());
            minZ = qMin(minZ, v.z());
        }
    }
    auto map = [](double value, double in_min, double in_max, double out_min,
                  double out_max) -> double {
        return ((value - in_min) / (in_max - in_min)) * (out_max - out_min) +
               out_min;
    };

    for (const QVector<QVector3D> &shape : projectedShapes) {
        for (int i = 0; i < shape.size() - 1; ++i) {
            QPoint a(shape[i].x(), shape[i].y());
            QPoint b(shape[i + 1].x(), shape[i + 1].y());

            double intensity_a = map(shape[i].z(), minZ, maxZ, 170, 0);
            double intensity_b = map(shape[i + 1].z(), minZ, maxZ, 170, 0);

            QLinearGradient gradient(a, b);
            gradient.setColorAt(0.0,
                                QColor(intensity_a, intensity_a, intensity_a));
            gradient.setColorAt(1.0,
                                QColor(intensity_b, intensity_b, intensity_b));

            imagePainter.setPen(QPen(QBrush(gradient), 2));

            imagePainter.drawLine(a, b);
        }
    }

    QPainter painter(this);
    if (!m_image.isNull()) {
        painter.drawImage(QPoint(0, 0), m_image);
    }

    paintAxes(rotate);
}

void SceneViewport::paintAxes(const QMatrix4x4 &rotation)
{
    QVector3D origin(0, 0, 0);
    QVector3D xAxis(3, 0, 0);
    QVector3D yAxis(0, 3, 0);
    QVector3D zAxis(0, 0, 3);

    QMatrix4x4 projection = perspectiveProjectionMatrix(1, 1, 7, 10);
    QMatrix4x4 translate = translationMatrix(QVector3D(0, 0, 20));
    QMatrix4x4 modelView = translate * rotation;

    QRect viewport(0, 0, 40, 40);
    origin = origin.project(modelView, projection, viewport);
    xAxis = xAxis.project(modelView, projection, viewport);
    yAxis = yAxis.project(modelView, projection, viewport);
    zAxis = zAxis.project(modelView, projection, viewport);

    QPainter painter(this);

    painter.translate(QPoint(40, 40));
    painter.setPen(QPen(Qt::green, 2));
    painter.drawLine(QPoint(origin.x(), origin.y()),
                     QPoint(xAxis.x(), xAxis.y()));
    painter.setPen(QPen(Qt::red, 2));
    painter.drawLine(QPoint(origin.x(), origin.y()),
                     QPoint(yAxis.x(), yAxis.y()));
    painter.setPen(QPen(Qt::blue, 2));
    painter.drawLine(QPoint(origin.x(), origin.y()),
                     QPoint(zAxis.x(), zAxis.y()));
}

void SceneViewport::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    resizeImageToFit();
    update();
}

void SceneViewport::resizeImageToFit()
{
    if (!m_image.isNull() && size().isValid()) {
        m_image = m_image.scaled(size(), Qt::IgnoreAspectRatio,
                                 Qt::SmoothTransformation);
    }
}

void SceneViewport::wheelEvent(QWheelEvent *event)
{
    m_zoom -= event->angleDelta().y() / 500.f;
    if (m_zoom > -0.1) {
        m_zoom = -0.1;
    }

    update();
    emit zoomChanged(m_zoom);
}

void SceneViewport::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = true;
        m_lastMousePos = event->pos();
    }
}

void SceneViewport::mouseMoveEvent(QMouseEvent *event)
{
    auto sign = [](float val) -> float {
        if (qFuzzyIsNull(val)) {
            return 0.0f;
        }
        return val < 0 ? -1.0f : 1.0f;
    };

    if (m_isDragging) {
        QPoint delta = event->pos() - m_lastMousePos;
        float slowFactor = 70.0f;
        QVector2D v = QVector2D(delta.x(), delta.y()) / slowFactor;
        m_rotation += QVector3D(-v.y(), v.x(), 0);

        update();

        emit rotationChanged(m_rotation);
        m_lastMousePos = event->pos();
    }
}

QMatrix4x4 SceneViewport::normalizeMatrix(QVector<QVector<QVector3D>> &vertices)
{
    if (vertices.empty()) {
        return QMatrix4x4();
    }

    QVector3D min = vertices.at(0).at(0);
    QVector3D max = vertices.at(0).at(0);

    for (const QVector<QVector3D> &shape : vertices) {
        for (const QVector3D &v : shape) {
            min.setX(qMin(min.x(), v.x()));
            min.setY(qMin(min.y(), v.y()));
            min.setZ(qMin(min.z(), v.z()));

            max.setX(qMax(max.x(), v.x()));
            max.setY(qMax(max.y(), v.y()));
            max.setZ(qMax(max.z(), v.z()));
        }
    }

    QVector3D t;
    t.setX(-(max.x() - min.x()) / 2 - min.x());
    t.setY(-(max.y() - min.y()) / 2 - min.y());
    t.setZ(-(max.z() - min.z()) / 2 - min.z());

    QMatrix4x4 translate = translationMatrix(t);

    QVector3D dim;
    dim.setX(max.x() - min.x());
    dim.setY(max.y() - min.y());
    dim.setZ(max.z() - min.z());

    float maxDim = qMax(dim.x(), qMax(dim.y(), dim.z()));

    QVector3D s(1, 1, 1);
    if (!qFuzzyIsNull(maxDim)) {
        s.setX(2 / maxDim);
        s.setY(2 / maxDim);
        s.setZ(2 / maxDim);
    }

    QMatrix4x4 scale = scaleMatrix(s);

    return scale * translate;
}

QMatrix4x4 SceneViewport::rotationMatrix(const QVector3D &rotation)
{
    // clang-format off
    QMatrix4x4 Rx(1, 0, 0, 0,
                  0, qCos(rotation.x()), -qSin(rotation.x()), 0,
                  0, qSin(rotation.x()), qCos(rotation.x()), 0,
                  0, 0, 0, 1);

    QMatrix4x4 Ry(qCos(rotation.y()), 0, qSin(rotation.y()), 0,
                  0, 1, 0, 0,
                  -qSin(rotation.y()), 0, qCos(rotation.y()), 0,
                  0, 0, 0, 1);

    QMatrix4x4 Rz(qCos(rotation.z()), -qSin(rotation.z()), 0, 0,
                  qSin(rotation.z()), qCos(rotation.z()), 0, 0,
                  0, 0, 1, 0,
                  0, 0, 0, 1);
    // clang-format on

    return Rx * Ry * Rz;
}

QMatrix4x4 SceneViewport::translationMatrix(const QVector3D &t)
{
    // clang-format off
    return QMatrix4x4(1, 0, 0, t.x(),
                      0, 1, 0, t.y(),
                      0, 0, 1, t.z(),
                      0, 0, 0, 1);
    // clang-format on 
}

QMatrix4x4 SceneViewport::scaleMatrix(const QVector3D& s)
{
    // clang-format off
    return QMatrix4x4 (s.x(), 0, 0, 0,
                       0, s.y(), 0, 0,
                       0, 0, s.z(), 0,
                       0, 0, 0, 1);
    // clang-format on 
}

QMatrix4x4 SceneViewport::perspectiveProjectionMatrix(float sw, float sh, float zf, float zb)
{
    // clang-format off
    return QMatrix4x4(2 * zf / sw, 0, 0, 0,
                      0, 2 * zf / sh, 0, 0,
                      0, 0, zb / (zb - zf), -zf * zb / (zb - zf),
                      0, 0, 1, 0);
    // clang-format on
}

QMatrix4x4 SceneViewport::orthographicProjectionMatrix(float sw, float sh,
                                                       float zf, float zb)
{
    // clang-format off
    return QMatrix4x4(2 / sw, 0, 0, 0,
                      0, 2 / sh, 0, 0,
                      0, 0, 1 / (zb - zf), -zf / (zb - zf),
                      0, 0, 0, 1);
    // clang-format on
}

void SceneViewport::setSceneVertices(
    const QVector<QVector<QVector3D>> &vertices)
{
    m_vertices = vertices;
    update();
}

void SceneViewport::setRotation(const QVector3D& rotation)
{
    m_rotation = rotation;
    update();
}
void SceneViewport::setZoom(qreal zoom)
{
    m_zoom = zoom;
    update();
}
