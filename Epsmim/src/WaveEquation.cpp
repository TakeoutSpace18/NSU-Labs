#include "WaveEquation.h"

#include <cassert>
#include <cmath>
#include <csignal>
#include <cstring>
#include <emmintrin.h>
#include <limits>
#include <immintrin.h>
#include <new>
#include <xmmintrin.h>

// makes [a7, b0, b1, ..., b6]
static FORCEINLINE __m256 shift_left(__m256 a, __m256 b)
{
    __m256 b_shifted = _mm256_permutevar8x32_ps(b, _mm256_setr_epi32(
        7, 0, 1, 2, 3, 4, 5, 6
    ));

    __m256 a_shifted = _mm256_permutevar8x32_ps(a, _mm256_setr_epi32(
        7, 0, 1, 2, 3, 4, 5, 6
    ));
    return _mm256_blend_ps(a_shifted, b_shifted, 0b11111110);
}

// makes [a1, ..., a6, b0]
static FORCEINLINE __m256 shift_right(__m256 a, __m256 b)
{
    __m256 b_shifted = _mm256_permutevar8x32_ps(b, _mm256_setr_epi32(
        1, 2, 3, 4, 5, 6, 7, 0
    ));

    __m256 a_shifted = _mm256_permutevar8x32_ps(a, _mm256_setr_epi32(
        1, 2, 3, 4, 5, 6, 7, 0
    ));

    return _mm256_blend_ps(a_shifted, b_shifted, 0b10000000);
}

WaveEquation::WaveEquation(const AreaParams& area, const Utils::Vec2i& source)
    : m_area(area),
    m_source(source),
    m_step(0),
    m_max(0)
{

    m_stride = area.nx * sizeof(float);
    m_stride = (m_stride + 31) & ~31; // make strobe divisible by 32;
    m_stride /= sizeof(float);

    std::cout << "strobe: " << m_stride << "\n";
    std::cout << "nx: " << m_area.nx << "\n";

    size_t size = m_stride * sizeof(float) * area.ny;
    m_data = ::operator new(3 * size, std::align_val_t(32)); // fit 3 buffers in one allocation
    std::memset(m_data, 0, 3 * size);

    m_buf1 =       (float *) (m_data);
    m_buf2 =       (float *) ((char *)(m_data) + size);
    m_phaseSpeed = (float *) ((char *)(m_data) + 2 * size);

    if (area.nx > 1000 || area.ny > 1000) {
        m_tau = 0.001;
    }
    else {
        m_tau = 0.01;
    }

    m_gridStep.x = (m_area.xb - m_area.xa) / (m_area.nx - 1);
    m_gridStep.y = (m_area.yb - m_area.ya) / (m_area.ny - 1);

    m_stepInvariantScalar.x = 1.0 / (2 * m_gridStep.x * m_gridStep.x);
    m_stepInvariantScalar.y = 1.0 / (2 * m_gridStep.y * m_gridStep.y);
    m_stepInvariantX = _mm256_set1_ps(m_stepInvariantScalar.x);
    m_stepInvariantY = _mm256_set1_ps(m_stepInvariantScalar.y);

    m_tauSquaredScalar = m_tau * m_tau;
    m_tauSquared = _mm256_set1_ps(m_tauSquaredScalar);

    m_two = _mm256_set1_ps(2.0f);

    generatePhaseSpeed();
}

void WaveEquation::generatePhaseSpeed()
{
    for (int i = 0; i < m_area.ny; ++i) {
        for (int j = 0; j < m_area.nx; ++j) {
            if (j < m_area.nx / 2) {
                m_phaseSpeed[i * m_stride + j] = 0.1 * 0.1;
            }
            else {
                m_phaseSpeed[i * m_stride + j] = 0.2 * 0.2;
                // m_phaseSpeed[i * m_strobe + j] = 0.1 * 0.1;
            }
        }
    }
}

void WaveEquation::computeSingle(int idx)
{
    float a1 = m_buf1[idx + 1] - m_buf1[idx];
    float a2 = m_phaseSpeed[idx - m_stride] + m_phaseSpeed[idx];
    float a3 = m_buf1[idx - 1] - m_buf1[idx];
    float a4 = m_phaseSpeed[idx - m_stride - 1] + m_phaseSpeed[idx - 1];
    float a = (a1 * a2 + a3 * a4) * m_stepInvariantScalar.x;

    float b1 = m_buf1[idx + m_stride] - m_buf1[idx];
    float b2 = m_phaseSpeed[idx - 1] + m_phaseSpeed[idx];
    float b3 = m_buf1[idx - m_stride] - m_buf1[idx];
    float b4 = m_phaseSpeed[idx - m_stride - 1] + m_phaseSpeed[idx - m_stride];
    float b = (b1 * b2 + b3 * b4) * m_stepInvariantScalar.y;

    float c = m_tauSquaredScalar * (a + b);

    m_buf2[idx] = 2 * m_buf1[idx] - m_buf2[idx] + c;

    m_max = std::max(m_max, std::abs(m_buf2[idx]));
}

WaveEquation::Output WaveEquation::nextIteration()
{
    std::swap(m_buf1, m_buf2);

    __m256 maxVector = _mm256_set1_ps(std::numeric_limits<float>::min());
    m_max = std::numeric_limits<float>::min();

    for (int i = 1; i < m_area.ny - 1; ++i)
    {
        const int rowIdx = i * m_stride;
        
        int j;
        for (j = 1; j < 8 ; j += 1)
        {
            const int idx = rowIdx + j;
            computeSingle(idx);
        }

        __m256 buf1Prev, buf1Cur, buf1Next;
        __m256 phaseSpeedPrev, phaseSpeedCur, phaseSpeedTopPrev, phaseSpeedTopCur;

        buf1Cur = _mm256_load_ps(&m_buf1[rowIdx]);
        buf1Next = _mm256_load_ps(&m_buf1[rowIdx + 8]);

        phaseSpeedCur = _mm256_load_ps(&m_phaseSpeed[rowIdx]);
        phaseSpeedTopCur = _mm256_load_ps(&m_phaseSpeed[rowIdx - m_stride]);

        for (; j < m_area.nx - 8; j += 8)
        {
            const int idx = rowIdx + j;

            buf1Prev = buf1Cur;
            buf1Cur = buf1Next;
            buf1Next = _mm256_load_ps(&m_buf1[idx + 8]);

            phaseSpeedPrev = phaseSpeedCur;
            phaseSpeedCur = _mm256_load_ps(&m_phaseSpeed[idx]);

            phaseSpeedTopPrev = phaseSpeedTopCur;
            phaseSpeedTopCur = _mm256_load_ps(&m_phaseSpeed[idx - m_stride]);

            __m256 buf1Left = shift_left(buf1Prev, buf1Cur);
            __m256 buf1Right = shift_right(buf1Cur, buf1Next);
            __m256 buf1Top = _mm256_load_ps(&m_buf1[idx - m_stride]);
            __m256 buf1Bottom = _mm256_load_ps(&m_buf1[idx + m_stride]);

            __m256 phaseSpeedLeft = shift_left(phaseSpeedPrev, phaseSpeedCur);
            __m256 phaseSpeedTopLeft = shift_left(phaseSpeedTopPrev, phaseSpeedTopCur);

            __m256 a1 = _mm256_sub_ps(buf1Right, buf1Cur);
            __m256 a2 = _mm256_add_ps(phaseSpeedTopCur, phaseSpeedCur);
            __m256 a3 = _mm256_sub_ps(buf1Left, buf1Cur);
            __m256 a4 = _mm256_add_ps(phaseSpeedTopLeft, phaseSpeedLeft);

            __m256 a1a2 = _mm256_mul_ps(a1, a2);
            __m256 a = _mm256_fmadd_ps(a3, a4, a1a2);
            
            __m256 b1 = _mm256_sub_ps(buf1Bottom, buf1Cur);
            __m256 b2 = _mm256_add_ps(phaseSpeedLeft, phaseSpeedCur);
            __m256 b3 = _mm256_sub_ps(buf1Top, buf1Cur);
            __m256 b4 = _mm256_add_ps(phaseSpeedTopLeft, phaseSpeedTopCur);

            __m256 b1b2 = _mm256_mul_ps(b1, b2);
            __m256 b = _mm256_fmadd_ps(b3, b4, b1b2);
            
            __m256 b_inv = _mm256_mul_ps(b, m_stepInvariantY);
            __m256 c = _mm256_fmadd_ps(a, m_stepInvariantX, b_inv);

            __m256 buf2Cur = _mm256_load_ps(&m_buf2[idx]);

            __m256 result = _mm256_fmsub_ps(m_two, buf1Cur, buf2Cur);
            result = _mm256_fmadd_ps(m_tauSquared, c, result);

            _mm256_store_ps(&m_buf2[idx], result);

            __m256 signBit = _mm256_set1_ps(-0.0f);
            __m256 absResult = _mm256_andnot_ps(signBit, result); // clear sign bit
            maxVector = _mm256_max_ps(maxVector, absResult);
        }

        for (; j < m_area.nx - 1; j += 1)
        {
            const int idx = rowIdx + j;
            computeSingle(idx);
        }
    }

    float maxArray[8];
    _mm256_storeu_ps(maxArray, maxVector);
    m_max = maxArray[0];
    for (int i = 1; i < 8; ++i) {
        m_max = std::max(m_max, maxArray[i]);
    }

    m_buf2[m_source.y * m_stride + m_source.x] += m_tauSquaredScalar * sourceFunc(m_step);

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

