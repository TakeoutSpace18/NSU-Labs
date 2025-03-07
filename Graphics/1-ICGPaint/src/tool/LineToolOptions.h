#ifndef LINE_TOOL_OPTIONS_H
#define LINE_TOOL_OPTIONS_H

#include <QObject>
#include <QColor>

class LineToolOptions : public QObject
{
    Q_OBJECT

public:
    explicit LineToolOptions(QObject *parent = nullptr)
        : QObject(parent), m_color(Qt::black), m_width(3) {}

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

signals:
    void colorChanged(const QColor &color);
    void widthChanged(int width);

private:
    QColor m_color;
    int m_width;
};

#endif // !LINE_TOOL_OPTIONS_H

