#ifndef SPLINEVIEWPORT_H
#define SPLINEVIEWPORT_H

#include <QGraphicsView>
#include <QWheelEvent>
#include <QKeyEvent>

class SplineViewport : public QGraphicsView
{
    Q_OBJECT

public:
    explicit SplineViewport(QWidget* parent = nullptr);
    ~SplineViewport() override = default;

private slots:

    void zoomIn();
    void zoomOut();

    void scaleView(qreal scaleFactor);

protected:
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

};

#endif // !SPLINEVIEWPORT_H
