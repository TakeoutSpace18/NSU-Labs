#include "Application.h"

#include <chrono>
#include <iostream>

#include <QPainter>
#include <QCoreApplication>

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
    connect(&m_updateTimer, &QTimer::timeout, this, &Application::nextIteration);
    m_updateTimer.start(0);
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
    const WaveEquation::ValueType *buf = out.data;

    uchar* pixelData = image.bits();
    const int stride = m_image.bytesPerLine();

    for (int i = 0; i < m_equation->ny(); ++i) {
        #pragma omp simd
        for (int j = 0; j < m_equation->nx(); ++j) {
            const WaveEquation::ValueType value = buf[i * m_equation->stride() + j];
            const int normalizedValue = static_cast<int>(255 * (value + out.max) / (2 * out.max));

            pixelData[i * stride + j] = static_cast<uchar>(std::clamp(normalizedValue, 0, 255));
        }
    }
}

Application::~Application()
{
    std::cout << "\naverage time: " << m_averageTime << " us\n";
}
