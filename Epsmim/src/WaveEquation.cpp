#include "WaveEquation.h"

#include <cassert>
#include <cmath>
#include <limits>

WaveEquation::WaveEquation(const AreaParams& area, const Utils::Vec2i& source)
    : m_area(area),
    m_source(source),
    m_step(0),
    m_max(0),
    m_data(area.nx * area.ny * 2, 0),
    m_buf1(m_data.data()),
    m_buf2(m_data.data() + area.nx * area.ny),
    m_phaseSpeed(area.nx * area.ny, 0)
{
    if (area.nx > 1000 || area.ny > 1000) {
        m_tau = 0.001;
    }
    else {
        m_tau = 0.01;
    }

    m_gridStep.x = (m_area.xb - m_area.xa) / (m_area.nx - 1);
    m_gridStep.y = (m_area.yb - m_area.ya) / (m_area.ny - 1);

    m_stepInvariant.x = 1 / (2 * m_gridStep.x * m_gridStep.x);
    m_stepInvariant.y = 1 / (2 * m_gridStep.y * m_gridStep.y);

    generatePhaseSpeed();
}

void WaveEquation::generatePhaseSpeed()
{
    for (int i = 0; i < m_area.nx; ++i) {
        for (int j = 0; j < m_area.ny; ++j) {
            if (j < m_area.nx / 2) {
                m_phaseSpeed[i * m_area.nx + j] = 0.1 * 0.1;
            }
            else {
                m_phaseSpeed[i * m_area.nx + j] = 0.2 * 0.2;
            }
        }
    }
}

WaveEquation::Output WaveEquation::nextIteration()
{
    std::swap(m_buf1, m_buf2);

    m_max = std::numeric_limits<ValueType>::min();
        
    for (int i = 1; i < m_area.ny - 1; ++i)
    {
        #pragma omp simd reduction(max: m_max)
        for (int j = 1; j < m_area.nx - 1; ++j)
        {
            const int idx = i * m_area.nx + j;

            const ValueType a1 = m_buf1[idx + 1] - m_buf1[idx];
            const ValueType a2 = m_phaseSpeed[idx - m_area.nx] + m_phaseSpeed[idx];
            const ValueType a3 = m_buf1[idx - 1] - m_buf1[idx];
            const ValueType a4 = m_phaseSpeed[idx - m_area.nx - 1] + m_phaseSpeed[idx - 1];
            const ValueType a = (a1 * a2 + a3 * a4) * m_stepInvariant.x;

            const ValueType b1 = m_buf1[idx + m_area.nx] - m_buf1[idx];
            const ValueType b2 = m_phaseSpeed[idx - 1] + m_phaseSpeed[idx];
            const ValueType b3 = m_buf1[idx - m_area.nx] - m_buf1[idx];
            const ValueType b4 = m_phaseSpeed[idx - m_area.nx - 1] + m_phaseSpeed[idx - m_area.nx];

            const ValueType b = (b1 * b2 + b3 * b4) * m_stepInvariant.y;
            const ValueType c = m_tau * m_tau * (a + b);

            m_buf2[idx] = 2 * m_buf1[idx] - m_buf2[idx] + c;
            m_max = std::max(m_max, std::abs(m_buf2[idx]));
        }
    }

    m_buf2[m_source.y * m_area.nx + m_source.x] += m_tau * m_tau * sourceFunc(m_step);

    m_step++;
    return {m_buf2, m_max};
}

WaveEquation::Output WaveEquation::getCurrentState()
{
    return {m_buf2, m_max};
}

WaveEquation::Output WaveEquation::skipNIterarions(int n)
{
    for (int i = 0; i < n; ++i) {
        nextIteration();
    }

    return getCurrentState();
}

WaveEquation::ValueType WaveEquation::sourceFunc(int n)
{
    const ValueType f0 = 1.0;
    const ValueType t0 = 1.5;
    const ValueType gamma = 4.0;
    
    const ValueType tmp = 2 * M_PI * f0 * (n * m_tau - t0);

    return std::exp(-(tmp * tmp) / (gamma * gamma)) * std::sin(tmp);
}

