#ifndef APPLICATION_H
#define APPLICATION_H

#include "WaveEquation.h"
#include <QMainWindow>
#include <QImage>
#include <QTimer>
#include <memory>

class Application : public QMainWindow
{
    Q_OBJECT

public:
    Application(std::unique_ptr<WaveEquation> equation,
                QWidget *parent = nullptr);

    ~Application();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void nextIteration();

private:
    void plotOutput(QImage& image, const WaveEquation::Output& out);

    QImage m_image;
    QTimer m_timer;

    size_t m_step;

    double m_averageTime;

    std::unique_ptr<WaveEquation> m_equation;
};

#endif // APPLICATION_H
