#include "Application.h"

#include <QPainter>

#include <chrono>
#include <iostream>
#include <vector>

Application::Application(std::unique_ptr<WaveEquation> equation, QWidget *parent)
    : QMainWindow(parent),
    m_image(equation->nx(), equation->ny(), QImage::Format_Grayscale8),
    m_equation(std::move(equation)),
    m_step(0),
    m_averageTime(0)
{
    m_image.fill(Qt::white);
    setFixedSize(m_image.size());

    // timer with timeout = 0 will update image as fast as possible
    connect(&m_timer, &QTimer::timeout, this, &Application::nextIteration);
    m_timer.start(0);
}

void Application::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawImage(0, 0, m_image);
}

void Application::nextIteration()
{
    using namespace std::chrono;

    m_step++;

    auto start = high_resolution_clock::now();
    WaveEquation::Output out = m_equation->nextIteration();
    auto end = high_resolution_clock::now();

    uint64_t elapsed = duration_cast<microseconds>(end - start).count();

    std::cout << "\rstep " << m_step << ": time = " << elapsed << " us";
    std::flush(std::cout);

    m_averageTime = (m_averageTime * (m_step - 1) + static_cast<double>(elapsed)) / m_step;

    plotOutput(m_image, out);

    update();
}

void Application::plotOutput(QImage& image, const WaveEquation::Output& out)
{
    const WaveEquation::ValueType *buf = out.data.data();

    for (int i = 0; i < m_equation->ny(); ++i) {
        for (int j = 0; j < m_equation->nx(); ++j) {
            float value = 255 * (buf[i * m_equation->nx() + j] + out.max) / (2 * out.max); 
            image.setPixel(i, j, qRgb(value, value, value));
        }
    }
}

Application::~Application()
{
    std::cout << "\naverage time: " << m_averageTime << " us\n";
}
