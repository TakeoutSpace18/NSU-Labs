#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <qcolor.h>

class Canvas : public QWidget
{
    Q_OBJECT

public:
    Canvas(const QSize& size, QWidget *parent = nullptr);
    Canvas(const QImage& image, QWidget *parent = nullptr) {
        throw std::runtime_error("Not implemented yet");
    };

    Canvas(QWidget *parent = nullptr) : Canvas(QSize(640, 480), parent) {}

    void setImage(const QImage& image);
    void drawLine(QPoint a, QPoint b, QColor color = Qt::black, int width = 5);
    void setPixelColor(int x, int y, const QColor& color);
    void clear();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QImage m_image;
};

#endif
