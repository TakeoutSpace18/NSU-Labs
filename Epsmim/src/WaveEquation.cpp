#include "WaveEquation.h"

#include <emmintrin.h>
#include <immintrin.h>
#include <xmmintrin.h>
#include <omp.h>

#include <cassert>
#include <cmath>
#include <csignal>
#include <cstring>
#include <iostream>
#include <limits>
#include <new>
#include <ostream>

// makes [a7, b0, b1, ..., b6]
static FORCEINLINE __m256 shift_left(__m256 a, __m256 b)
{
    const __m256 blended = _mm256_blend_ps(b, a, 0b10000000);
    const __m256 shuffleMask = _mm256_set_epi32(6, 5, 4, 3, 2, 1, 0, 7);
    return _mm256_permutevar8x32_ps(blended, shuffleMask);
}

// makes [a1, ..., a6, b0]
static FORCEINLINE __m256 shift_right(__m256 a, __m256 b)
{
    const __m256 blended = _mm256_blend_ps(a, b, 0b00000001);
    const __m256 shuffleMask = _mm256_set_epi32(0, 7, 6, 5, 4, 3, 2, 1);
    return _mm256_permutevar8x32_ps(blended, shuffleMask);
}

WaveEquation::WaveEquation(const AreaParams &area, const Utils::Vec2i &source)
    : m_area(area), m_source(source), m_stepGlobal(0), m_max(0)
{
    m_stride = area.nx * sizeof(float);
    m_stride = (m_stride + 31) & ~31;  // make strobe divisible by 32;
    m_stride /= sizeof(float);

    size_t size = m_stride * sizeof(float) * area.ny;
    m_data = ::operator new(
        3 * size, std::align_val_t(32));  // fit 3 buffers in one allocation
    std::memset(m_data, 0, 3 * size);

    m_buf1 = (float *)(m_data);
    m_buf2 = (float *)((char *)(m_data) + size);
    m_phaseSpeed = (float *)((char *)(m_data) + 2 * size);

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
            }
        }
    }
}

float WaveEquation::computeSingle(const ComputeSingleData &d)
{
    float a1 = d.buf1Right - d.buf1Cur;
    float a2 = d.phaseSpeedTop + d.phaseSpeedCur;
    float a3 = d.buf1Left - d.buf1Cur;
    float a4 = d.phaseSpeedTopLeft + d.phaseSpeedLeft;
    float a = (a1 * a2 + a3 * a4) * m_stepInvariantScalar.x;

    float b1 = d.buf1Bottom - d.buf1Cur;
    float b2 = d.phaseSpeedLeft + d.phaseSpeedCur;
    float b3 = d.buf1Top - d.buf1Cur;
    float b4 = d.phaseSpeedTopLeft + d.phaseSpeedTop;
    float b = (b1 * b2 + b3 * b4) * m_stepInvariantScalar.y;

    float c = m_tauSquaredScalar * (a + b);

    float ret = 2 * d.buf1Cur - d.buf2Cur + c;
    return ret;
}

__m256 WaveEquation::computeVector(const ComputeVectorData &d)
{
    __m256 buf1Left = shift_left(d.buf1Prev, d.buf1Cur);
    __m256 buf1Right = shift_right(d.buf1Cur, d.buf1Next);

    __m256 phaseSpeedLeft = shift_left(d.phaseSpeedPrev, d.phaseSpeedCur);
    __m256 phaseSpeedTopLeft = shift_left(d.phaseSpeedTopPrev, d.phaseSpeedTop);

    __m256 a1 = _mm256_sub_ps(buf1Right, d.buf1Cur);
    __m256 a2 = _mm256_add_ps(d.phaseSpeedTop, d.phaseSpeedCur);
    __m256 a3 = _mm256_sub_ps(buf1Left, d.buf1Cur);
    __m256 a4 = _mm256_add_ps(phaseSpeedTopLeft, phaseSpeedLeft);

    __m256 a1a2 = _mm256_mul_ps(a1, a2);
    __m256 a = _mm256_fmadd_ps(a3, a4, a1a2);

    __m256 b1 = _mm256_sub_ps(d.buf1Bottom, d.buf1Cur);
    __m256 b2 = _mm256_add_ps(phaseSpeedLeft, d.phaseSpeedCur);
    __m256 b3 = _mm256_sub_ps(d.buf1Top, d.buf1Cur);
    __m256 b4 = _mm256_add_ps(phaseSpeedTopLeft, d.phaseSpeedTop);

    __m256 b1b2 = _mm256_mul_ps(b1, b2);
    __m256 b = _mm256_fmadd_ps(b3, b4, b1b2);

    __m256 b_inv = _mm256_mul_ps(b, m_stepInvariantY);
    __m256 c = _mm256_fmadd_ps(a, m_stepInvariantX, b_inv);

    __m256 result = _mm256_fmsub_ps(m_two, d.buf1Cur, d.buf2Cur);
    result = _mm256_fmadd_ps(m_tauSquared, c, result);

    return result;
}

void WaveEquation::computeRow(const float *buf1, float *buf2,
                              const float *phaseSpeed, int i, __m256 &maxVector,
                              float &maxScalar)
{
    const int rowIdx = i * m_stride;

    int j;
    for (j = 1; j < 8; j += 1) {
        const int idx = rowIdx + j;

        ComputeSingleData data;
        data.buf1Cur = buf1[idx];
        data.buf1Left = buf1[idx - 1];
        data.buf1Right = buf1[idx + 1];
        data.buf1Top = buf1[idx - m_stride];
        data.buf1Bottom = buf1[idx + m_stride];
        data.buf2Cur = buf2[idx];
        data.phaseSpeedCur = phaseSpeed[idx];
        data.phaseSpeedTop = phaseSpeed[idx - m_stride];
        data.phaseSpeedTopLeft = phaseSpeed[idx - m_stride - 1];
        data.phaseSpeedLeft = phaseSpeed[idx - 1];

        buf2[idx] = computeSingle(data);
        maxScalar = std::max(m_max, std::abs(buf2[idx]));
    }

    ComputeVectorData data;

    data.buf1Cur = _mm256_load_ps(&buf1[rowIdx]);
    data.buf1Next = _mm256_load_ps(&buf1[rowIdx + 8]);

    data.phaseSpeedCur = _mm256_load_ps(&phaseSpeed[rowIdx]);
    data.phaseSpeedTop = _mm256_load_ps(&phaseSpeed[rowIdx - m_stride]);

    for (; j < m_area.nx - 8; j += 8) {
        const int idx = rowIdx + j;

        data.buf1Prev = data.buf1Cur;
        data.buf1Cur = data.buf1Next;
        data.buf1Next = _mm256_load_ps(&buf1[idx + 8]);

        data.phaseSpeedPrev = data.phaseSpeedCur;
        data.phaseSpeedCur = _mm256_load_ps(&phaseSpeed[idx]);

        data.phaseSpeedTopPrev = data.phaseSpeedTop;
        data.phaseSpeedTop = _mm256_load_ps(&phaseSpeed[idx - m_stride]);

        data.buf1Top = _mm256_load_ps(&buf1[idx - m_stride]);
        data.buf1Bottom = _mm256_load_ps(&buf1[idx + m_stride]);

        data.buf2Cur = _mm256_load_ps(&buf2[idx]);

        __m256 result = computeVector(data);

        _mm256_store_ps(&buf2[idx], result);

        __m256 signBit = _mm256_set1_ps(-0.0f);
        __m256 absResult = _mm256_andnot_ps(signBit, result);  // clear sign bit
        maxVector = _mm256_max_ps(maxVector, absResult);
    }

    for (; j < m_area.nx - 1; j += 1) {
        const int idx = rowIdx + j;

        ComputeSingleData data;
        data.buf1Cur = buf1[idx];
        data.buf1Left = buf1[idx - 1];
        data.buf1Right = buf1[idx + 1];
        data.buf1Top = buf1[idx - m_stride];
        data.buf1Bottom = buf1[idx + m_stride];
        data.buf2Cur = buf2[idx];
        data.phaseSpeedCur = phaseSpeed[idx];
        data.phaseSpeedTop = phaseSpeed[idx - m_stride];
        data.phaseSpeedTopLeft = phaseSpeed[idx - m_stride - 1];
        data.phaseSpeedLeft = phaseSpeed[idx - 1];

        buf2[idx] = computeSingle(data);
        maxScalar = std::max(m_max, std::abs(buf2[idx]));
    }
}

void WaveEquation::stepInitialize(int startRowIdx, int skipSteps, __m256& maxVector, float& maxScalar)
{
    int w = skipSteps - 1;
    for (int step = 0; step < skipSteps; ++step) {
        for (int i = startRowIdx - w + step; i < startRowIdx + w - step; ++i) {
            if (i <= 1) {
                continue;
            }

            float *buf1, *buf2;
            if ((m_stepGlobal + step) % 2 == 0) {
                buf1 = m_buf1;
                buf2 = m_buf2;
            }
            else {
                buf1 = m_buf2;
                buf2 = m_buf1;
            }

            computeRow(buf1, buf2, m_phaseSpeed, i, maxVector, maxScalar);

            if (i == m_source.y) {
                buf2[m_source.y * m_stride + m_source.x] +=
                    m_tauSquaredScalar * sourceFunc(m_stepGlobal + step);
            }
        }
    }
}

void WaveEquation::stepMain(int startRowIdx, int stopRowIdx, int skipSteps, __m256& maxVector, float& maxScalar)
{
    int w = skipSteps - 1;
    for (int step = 0; step < skipSteps; ++step) {
        for (int i = startRowIdx + w - step; i < stopRowIdx - w - step; ++i) {

            float *buf1, *buf2;
            if ((m_stepGlobal + step) % 2 == 0) {
                buf1 = m_buf1;
                buf2 = m_buf2;
            }
            else {
                buf1 = m_buf2;
                buf2 = m_buf1;
            }

            computeRow(buf1, buf2, m_phaseSpeed, i, maxVector, maxScalar);

            if (i == m_source.y) {
                buf2[m_source.y * m_stride + m_source.x] +=
                    m_tauSquaredScalar * sourceFunc(m_stepGlobal + step);
            }
        }
    }
}

void WaveEquation::stepFinalize(int stopRowIdx, int skipSteps, __m256& maxVector, float& maxScalar)
{
    int w = skipSteps - 1;
    for (int step = 0; step < skipSteps; ++step) {
        for (int i = stopRowIdx - w - step; i < stopRowIdx - w + step; ++i) {
            if (i >= m_area.ny - 1) {
                break;
            }

            float *buf1, *buf2;
            if ((m_stepGlobal + step) % 2 == 0) {
                buf1 = m_buf1;
                buf2 = m_buf2;
            }
            else {
                buf1 = m_buf2;
                buf2 = m_buf1;
            }

            computeRow(buf1, buf2, m_phaseSpeed, i, maxVector, maxScalar);

            if (i == m_source.y) {
                buf2[m_source.y * m_stride + m_source.x] +=
                    m_tauSquaredScalar * sourceFunc(m_stepGlobal + step);
            }
        }
    }
}

WaveEquation::Output WaveEquation::nextIteration(int skipSteps)
{
    m_max = std::numeric_limits<float>::min();

    #pragma omp parallel
    {
        __m256 maxVector = _mm256_set1_ps(std::numeric_limits<float>::min());
        float maxScalar = std::numeric_limits<float>::min();

        int threadNum = omp_get_thread_num();
        int numThreads = omp_get_num_threads();
        int sectionWidth = (m_area.ny - 2) / numThreads;

        int startRowIdx = 1 + sectionWidth * threadNum;
        int stopRowIdx = startRowIdx + sectionWidth;
        if (threadNum == numThreads - 1) {
            stopRowIdx = m_area.ny - 1;
        }

        stepInitialize(startRowIdx, skipSteps, maxVector, maxScalar);
        stepMain(startRowIdx, stopRowIdx, skipSteps, maxVector, maxScalar);
        #pragma omp barrier
        stepFinalize(stopRowIdx, skipSteps, maxVector, maxScalar);

        float maxArray[8];
        _mm256_storeu_ps(maxArray, maxVector);
        for (int i = 0; i < 8; ++i) {
            maxScalar = std::max(maxScalar, maxArray[i]);
        }

        #pragma omp critical
        {
            m_max = std::max(m_max, maxScalar);
        }

        #pragma omp barrier
    }

    m_stepGlobal += skipSteps;
    if (m_stepGlobal % 2 == 0) {
        return {m_buf2, m_max};
    }
    else {
        return {m_buf1, m_max};
    }
}

WaveEquation::Output WaveEquation::getCurrentState()
{
    if (m_stepGlobal % 2 == 0) {
        return {m_buf2, m_max};
    }
    else {
        return {m_buf1, m_max};
    }
}

WaveEquation::Output WaveEquation::skipNIterarions(int n, int skipBatch)
{
    int nBatches = n / skipBatch;

    for (int i = 0; i < nBatches; ++i) {
        nextIteration(skipBatch);
    }

    if (nBatches * skipBatch != n) {
        nextIteration(n - nBatches * skipBatch);
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
