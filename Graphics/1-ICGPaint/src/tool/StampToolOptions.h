#ifndef STAMP_TOOL_OPTIONS_H
#define STAMP_TOOL_OPTIONS_H

#include <QObject>
#include <QColor>

class StampToolOptions : public QObject
{
    Q_OBJECT

public:
    enum class Shape {
        Polygon,
        Star
    };

    explicit StampToolOptions(QObject *parent = nullptr)
        : QObject(parent), m_color(Qt::black), m_width(3),
        m_shape(Shape::Polygon), m_radius(60),
        m_innerRadius(30), m_rotation(0), m_vertices(5) {}

    QColor color() const { return m_color; }
    void setColor(const QColor &color) {
        if (m_color != color) {
            m_color = color;
            emit colorChanged(m_color);
        }
    }

    int width() const { return m_width; }
    void setWidth(int width) {
        if (m_width != width) {
            m_width = width;
            emit widthChanged(m_width);
        }
    }

    Shape shape() const { return m_shape; }
    void setShape(Shape shape) {
        if (m_shape != shape) {
            m_shape = shape;
            emit shapeChanged(m_shape);
        }
    }

    int radius() const { return m_radius; }
    void setRadius(int radius) {
        if (m_radius != radius) {
            m_radius = radius;
            emit radiusChanged(m_radius);
        }
    }

    int innerRadius() const { return m_innerRadius; }
    void setInnerRadius(int innerRadius) {
        if (m_innerRadius != innerRadius) {
            m_innerRadius = innerRadius;
            emit innerRadiusChanged(m_innerRadius);
        }
    }

    int rotation() const { return m_rotation; }
    void setRotation(int rotation) {
        if (m_rotation != rotation) {
            m_rotation = rotation;
            emit rotationChanged(m_rotation);
        }
    }

    int vertices() const { return m_vertices; }
    void setVertices(int vertices) {
        if (m_vertices != vertices) {
            m_vertices = vertices;
            emit verticesChanged(m_vertices);
        }
    }

signals:
    void colorChanged(const QColor &color);
    void widthChanged(int width);
    void shapeChanged(Shape shape);
    void radiusChanged(int radius);
    void innerRadiusChanged(int innerRadius);
    void rotationChanged(int rotation);
    void verticesChanged(int vertices);

private:
    QColor m_color;
    int m_width;
    Shape m_shape;
    int m_radius;
    int m_innerRadius;
    int m_rotation;
    int m_vertices;
};

#endif // !STAMP_TOOL_OPTIONS_H

