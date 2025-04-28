#ifndef WAVE_EQUATION_H
#define WAVE_EQUATION_H

#include <vector>
#include <immintrin.h>

#include "Utils.h"

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

    Output nextIteration();
    Output skipNIterarions(int n);
    Output getCurrentState();

    int nx() const { return m_area.nx; };
    int ny() const { return m_area.ny; };
    int stride() const { return m_stride; };

private:
    float hmax256(__m256 vec);
    float sourceFunc(int n);
    void computeSingle(int idx);

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
    int m_step;
};

#endif
