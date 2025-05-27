#ifndef WAVE_EQUATION_H
#define WAVE_EQUATION_H

#include <atomic>
#include <immintrin.h>

#include "Utils.h"

#define NUM_THREADS 12

class WaveEquation
{
public:
    using ValueType = float;

    struct AreaParams
    {
        // modeling area bounds
        float xa;
        float xb;
        float ya;
        float yb;

        // grid resolution
        int nx;
        int ny;
    };

    struct Output
    {
        const float *data;
        float max;
    };

    WaveEquation(const AreaParams& area, const Utils::Vec2i& source);

    Output nextIteration(int skipSteps);
    Output skipNIterarions(int n, int skipBatch);
    Output getCurrentState();

    int nx() const { return m_area.nx; };
    int ny() const { return m_area.ny; };
    int stride() const { return m_stride; };

private:

    struct ComputeSingleData
    {
        float buf1Left;
        float buf1Right;
        float buf1Top;
        float buf1Bottom;
        float buf1Cur;
        float buf2Cur;
        float phaseSpeedLeft;
        float phaseSpeedCur;
        float phaseSpeedTopLeft;
        float phaseSpeedTop;
    };

    struct ComputeVectorData
    {
        __m256 buf1Prev;
        __m256 buf1Next;
        __m256 buf1Top;
        __m256 buf1Bottom;
        __m256 buf1Cur;
        __m256 buf2Cur;
        __m256 phaseSpeedPrev;
        __m256 phaseSpeedCur;
        __m256 phaseSpeedTopPrev;
        __m256 phaseSpeedTop;
    };

    float hmax256(__m256 vec);
    float sourceFunc(int n);
    float computeSingle(const ComputeSingleData& d);
    __m256 computeVector(const ComputeVectorData& d);
    void computeRow(const float *buf1, float *buf2, const float *phaseSpeed, int rowIdx, __m256& maxVector, float& maxScalar);
    void parallelSection(int skipSteps, int startRowIdx, int stopRowIdx, int threadNum, int& threadStep);

    void stepInitialize(int startRowIdx, int skipSteps, __m256& maxVector, float& maxScalar, int& threadStep);
    void stepMain(int startRowIdx, int stopRowIdx, int skipSteps, __m256& maxVector, float& maxScalar, int& threadStep);
    void stepFinalize(int stopRowIdx, int skipSteps, __m256& maxVector, float& maxScalar, int& threadStep);

    void generatePhaseSpeed();

    void *m_data;

    float *m_buf1;
    float *m_buf2;
    float *m_phaseSpeed;

    AreaParams m_area;
    size_t m_stride; // m_area.nx rounded up to be divisible by 32 (for alignment) 
    Utils::Vec2i m_source;
    Utils::Vec2f m_gridStep;

    Utils::Vec2f m_stepInvariantScalar;
    __m256 m_stepInvariantX;
    __m256 m_stepInvariantY;

    float m_tauSquaredScalar;
    __m256 m_tauSquared;
    __m256 m_two;

    float m_tau;
    float m_max;
    __m256 m_maxVector;
    int m_stepGlobal;

    std::atomic_int32_t m_sectionProgress[NUM_THREADS];
};

#endif
