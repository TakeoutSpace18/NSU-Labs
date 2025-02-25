#include "WaveEquation.h"

#include <cassert>
#include <cmath>
#include <limits>

WaveEquation::WaveEquation(const AreaParams& area, const Utils::Vec2i& source)
    : m_area(area),
    m_source(source),
    m_step(0),
    m_max(0),
    m_buf1(area.nx * area.ny, 0),
    m_buf2(area.nx * area.ny, 0),
    m_buf3(area.nx * area.ny, 0),
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

    for (int i = 0; i < m_area.nx; ++i) {
        for (int j = 0; j < m_area.ny; ++j) {
            if (j < area.nx / 2) {
                accessValue(m_phaseSpeed, i, j) = 0.1 * 0.1;
            }
            else {
                accessValue(m_phaseSpeed, i, j) = 0.2 * 0.2;
            }
        }
    }
}

WaveEquation::Output WaveEquation::nextIteration()
{
    std::swap(m_buf3, m_buf2);
    std::swap(m_buf3, m_buf1);

    m_max = std::numeric_limits<ValueType>::min();
        
    for (int i = 1; i < m_area.ny - 1; ++i)
    {
        for (int j = 1; j < m_area.nx - 1; ++j)
        {
            ValueType a1 = accessValue(m_buf2, i, j+1) - accessValue(m_buf2, i, j);
            ValueType a2 = accessValue(m_phaseSpeed, i-1, j) + accessValue(m_phaseSpeed, i, j);
            ValueType a3 = accessValue(m_buf2, i, j-1) - accessValue(m_buf2, i, j);
            ValueType a4 = accessValue(m_phaseSpeed, i-1, j-1) + accessValue(m_phaseSpeed, i, j-1);
            ValueType a = (a1 * a2 + a3 * a4) / (2 * m_gridStep.x * m_gridStep.x);

            ValueType b1 = accessValue(m_buf2, i+1, j) - accessValue(m_buf2, i, j);
            ValueType b2 = accessValue(m_phaseSpeed, i, j-1) + accessValue(m_phaseSpeed, i, j);
            ValueType b3 = accessValue(m_buf2, i-1, j) - accessValue(m_buf2, i, j);
            ValueType b4 = accessValue(m_phaseSpeed, i-1, j-1) + accessValue(m_phaseSpeed, i-1, j);
            ValueType b = (b1 * b2 + b3 * b4) / (2 * m_gridStep.y * m_gridStep.y);

            ValueType c = m_tau * m_tau * (sourceFunc(i, j, m_step) + a + b);

            accessValue(m_buf3, i, j) = 2 * accessValue(m_buf2, i, j) - accessValue(m_buf1, i, j) + c;

            m_max = std::max(m_max, std::abs(accessValue(m_buf3, i, j)));
        }
    }

    m_step++;

    return {m_buf3, m_max};
}

WaveEquation::Output WaveEquation::getCurrentState()
{
    return {m_buf3, m_max};
}

WaveEquation::Output WaveEquation::skipNIterarions(int n)
{
    for (int i = 0; i < n; ++i) {
        nextIteration();
    }

    return getCurrentState();
}

WaveEquation::ValueType WaveEquation::sourceFunc(int i, int j, int n)
{
    if (i != m_source.x || j != m_source.y) {
        return 0;
    }

    const ValueType f0 = 1.0;
    const ValueType t0 = 1.5;
    const ValueType gamma = 4.0;
    
    const ValueType tmp = 2 * M_PI * f0 * (n * m_tau - t0);

    return std::exp(-(tmp * tmp) / (gamma * gamma)) * std::sin(tmp);
}

WaveEquation::ValueType& WaveEquation::accessValue(std::vector<ValueType>& vec, int i, int j)
{
    assert(0 <= i && i < m_area.ny);
    assert(0 <= j && j < m_area.nx);

    return vec[i * m_area.nx + j];
}
