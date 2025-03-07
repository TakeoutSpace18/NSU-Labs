#ifndef FILL_TOOL_OPTIONS_H
#define FILL_TOOL_OPTIONS_H

#include <QObject>
#include <QColor>

class FillToolOptions : public QObject
{
    Q_OBJECT

public:
    explicit FillToolOptions(QObject *parent = nullptr)
        : QObject(parent), m_color(Qt::black) {}

    QColor color() const { return m_color; }
    void setColor(const QColor &color) {
        if (m_color != color) {
            m_color = color;
            emit colorChanged(m_color);
        }
    }

signals:
    void colorChanged(const QColor &color);

private:
    QColor m_color;
};

#endif // !FILL_TOOL_OPTIONS_H
