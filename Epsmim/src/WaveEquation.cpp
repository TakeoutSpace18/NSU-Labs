#include "WaveEquation.h"

#include <cassert>
#include <cmath>
#include <limits>
#include <immintrin.h>

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

    Utils::Vec2f stepInvariant;
    stepInvariant.x = 1 / (2 * m_gridStep.x * m_gridStep.x);
    stepInvariant.y = 1 / (2 * m_gridStep.y * m_gridStep.y);
    m_stepInvariantX = _mm256_set1_ps(stepInvariant.x);
    m_stepInvariantY = _mm256_set1_ps(stepInvariant.y);

    m_tauSquared = _mm256_set1_ps(m_tau * m_tau);
    m_two = _mm256_set1_ps(2.0f);

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

float WaveEquation::hmax256(__m256 vec) {
    // Unpack and max adjacent pairs
    __m128 vlow  = _mm256_castps256_ps128(vec);  // Low 128 bits
    __m128 vhigh = _mm256_extractf128_ps(vec, 1); // High 128 bits
    __m128 max1  = _mm_max_ps(vlow, vhigh);

    // Reduce within 128-bit lane
    __m128 max2 = _mm_movehl_ps(max1, max1); // Move high part to low
    __m128 max3 = _mm_max_ps(max1, max2);    

    __m128 max4 = _mm_shuffle_ps(max3, max3, _MM_SHUFFLE(2, 3, 0, 1)); 
    __m128 max5 = _mm_max_ps(max3, max4);

    return _mm_cvtss_f32(max5); // Extract final max value
}

WaveEquation::Output WaveEquation::nextIteration()
{
    std::swap(m_buf1, m_buf2);

    __m256 maxVector = _mm256_set1_ps(std::numeric_limits<float>::min());

    for (int i = 1; i < m_area.ny - 1; ++i)
    {
        const int rowIdx = i * m_area.nx;
        
        int j;
        for (j = 1; j < m_area.nx - 8; j += 8)
        {
            const int idx = rowIdx + j;
            
            __m256 buf1Center = _mm256_loadu_ps(&m_buf1[idx]);
            
            __m256 buf1Right = _mm256_loadu_ps(&m_buf1[idx + 1]);
            __m256 buf1Left = _mm256_loadu_ps(&m_buf1[idx - 1]);
            
            __m256 phaseSpeedTopLeft = _mm256_loadu_ps(&m_phaseSpeed[idx - m_area.nx - 1]);
            __m256 phaseSpeedTop = _mm256_loadu_ps(&m_phaseSpeed[idx - m_area.nx]);
            __m256 phaseSpeedLeft = _mm256_loadu_ps(&m_phaseSpeed[idx - 1]);
            __m256 phaseSpeed = _mm256_loadu_ps(&m_phaseSpeed[idx]);
            
            __m256 a1 = _mm256_sub_ps(buf1Right, buf1Center);
            __m256 a2 = _mm256_add_ps(phaseSpeedTop, phaseSpeed);
            __m256 a3 = _mm256_sub_ps(buf1Left, buf1Center);
            __m256 a4 = _mm256_add_ps(phaseSpeedTopLeft, phaseSpeedLeft);
            
            // a = (a1 * a2 + a3 * a4) * m_stepInvariant.x
            __m256 a1a2 = _mm256_mul_ps(a1, a2);
            __m256 a3a4 = _mm256_mul_ps(a3, a4);
            __m256 a = _mm256_mul_ps(_mm256_add_ps(a1a2, a3a4), m_stepInvariantX);
            
            __m256 buf1Bottom = _mm256_loadu_ps(&m_buf1[idx + m_area.nx]);
            __m256 buf1Top = _mm256_loadu_ps(&m_buf1[idx - m_area.nx]);
            
            __m256 b1 = _mm256_sub_ps(buf1Bottom, buf1Center);
            __m256 b2 = _mm256_add_ps(phaseSpeedLeft, phaseSpeed);
            __m256 b3 = _mm256_sub_ps(buf1Top, buf1Center);
            __m256 b4 = _mm256_add_ps(phaseSpeedTopLeft, phaseSpeedTop);
            
            // b = (b1 * b2 + b3 * b4) * m_stepInvariant.y
            __m256 b1b2 = _mm256_mul_ps(b1, b2);
            __m256 b3b4 = _mm256_mul_ps(b3, b4);
            __m256 b = _mm256_mul_ps(_mm256_add_ps(b1b2, b3b4), m_stepInvariantY);
            
            // c = m_tau * m_tau * (a + b)
            __m256 c = _mm256_mul_ps(m_tauSquared, _mm256_add_ps(a, b));
            
            // m_buf2[idx] = 2 * m_buf1[idx] - m_buf2[idx] + c
            __m256 twiceBuf1 = _mm256_mul_ps(m_two, buf1Center);
            __m256 buf2Center = _mm256_loadu_ps(&m_buf2[idx]);
            __m256 result = _mm256_add_ps(_mm256_sub_ps(twiceBuf1, buf2Center), c);
            _mm256_storeu_ps(&m_buf2[idx], result);
            
            // update maximum
            __m256 signBit = _mm256_set1_ps(-0.0f);
            __m256 absResult = _mm256_andnot_ps(signBit, result); // clear sign bit
            maxVector = _mm256_max_ps(maxVector, absResult);
        }

        // handle remaining elements
        if (j < m_area.nx - 1)
        {
            const int idx = rowIdx + j;
            const int remainingElements = m_area.nx - 1 - j;
            
            // create mask based on remaining elements
            int mask_values[8] = {0};
            for (int k = 0; k < remainingElements; ++k) {
                mask_values[k] = -1; // Set to all 1s for valid elements
            }
            __m256i mask = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(mask_values));
            
            __m256 buf1Center = _mm256_maskload_ps(&m_buf1[idx], mask);
            
            __m256 buf1Right = _mm256_maskload_ps(&m_buf1[idx + 1], mask);
            __m256 buf1Left = _mm256_maskload_ps(&m_buf1[idx - 1], mask);
            
            __m256 phaseSpeedTopLeft = _mm256_maskload_ps(&m_phaseSpeed[idx - m_area.nx - 1], mask);
            __m256 phaseSpeedTop = _mm256_maskload_ps(&m_phaseSpeed[idx - m_area.nx], mask);
            __m256 phaseSpeedLeft = _mm256_maskload_ps(&m_phaseSpeed[idx - 1], mask);
            __m256 phaseSpeed = _mm256_maskload_ps(&m_phaseSpeed[idx], mask);
            
            __m256 a1 = _mm256_sub_ps(buf1Right, buf1Center);
            __m256 a2 = _mm256_add_ps(phaseSpeedTop, phaseSpeed);
            __m256 a3 = _mm256_sub_ps(buf1Left, buf1Center);
            __m256 a4 = _mm256_add_ps(phaseSpeedTopLeft, phaseSpeedLeft);
            
            // a = (a1 * a2 + a3 * a4) * m_stepInvariant.x
            __m256 a1a2 = _mm256_mul_ps(a1, a2);
            __m256 a3a4 = _mm256_mul_ps(a3, a4);
            __m256 a = _mm256_mul_ps(_mm256_add_ps(a1a2, a3a4), m_stepInvariantX);
            
            __m256 buf1Bottom = _mm256_maskload_ps(&m_buf1[idx + m_area.nx], mask);
            __m256 buf1Top = _mm256_maskload_ps(&m_buf1[idx - m_area.nx], mask);
            
            __m256 b1 = _mm256_sub_ps(buf1Bottom, buf1Center);
            __m256 b2 = _mm256_add_ps(phaseSpeedLeft, phaseSpeed);
            __m256 b3 = _mm256_sub_ps(buf1Top, buf1Center);
            __m256 b4 = _mm256_add_ps(phaseSpeedTopLeft, phaseSpeedTop);

            // b = (b1 * b2 + b3 * b4) * m_stepInvariant.y
            __m256 b1b2 = _mm256_mul_ps(b1, b2);
            __m256 b3b4 = _mm256_mul_ps(b3, b4);
            __m256 b = _mm256_mul_ps(_mm256_add_ps(b1b2, b3b4), m_stepInvariantY);

            // c = m_tau * m_tau * (a + b)
            __m256 c = _mm256_mul_ps(m_tauSquared, _mm256_add_ps(a, b));
            
            // m_buf2[idx] = 2 * m_buf1[idx] - m_buf2[idx] + c
            __m256 twiceBuf1 = _mm256_mul_ps(m_two, buf1Center);
            __m256 buf2Center = _mm256_loadu_ps(&m_buf2[idx]);
            __m256 result = _mm256_add_ps(_mm256_sub_ps(twiceBuf1, buf2Center), c);
            _mm256_maskstore_ps(&m_buf2[idx], mask, result);
            
            // update maximum
            __m256 signBit = _mm256_set1_ps(-0.0f);
            __m256 absResult = _mm256_andnot_ps(signBit, result); // clear sign bit
            maxVector = _mm256_max_ps(maxVector, absResult);
        }
    }

    float maxArray[8];
    _mm256_storeu_ps(maxArray, maxVector);
    m_max = maxArray[0];
    for (int i = 1; i < 8; ++i) {
        m_max = std::max(m_max, maxArray[i]);
    }

    // m_max = hmax256(maxVector);

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

